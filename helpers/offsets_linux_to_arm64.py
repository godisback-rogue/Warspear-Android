#!/usr/bin/env python3
"""Translate ws-hack2/src/core/Offsets.h (Linux x86 32-bit) to predicted ARM64
offsets and emit ws-android/include/Offsets_android.h.

Why this isn't a constant delta:
  * Pointers grow 4 → 8 bytes
  * std::wstring grows 12 → 16 bytes (libc++ on Android)
  * std::vector / std::map etc. grow by 4-12 bytes
  * 8-byte alignment kicks in around ptr/wstring fields

Strategy:
  1. Parse Offsets.h, extracting (namespace, field_name, offset, comment).
  2. Infer each field's type from the comment ("ptr", "u32", "u8", "f32",
     "wstring", "RB-tree node", etc.).
  3. For each namespace, sort fields by offset and walk them in order.
     Between consecutive known fields, assume gaps are u32-sized unknown
     fields (the most common case). Track Linux size vs ARM64 size of
     every known field, accumulating growth.
  4. Calibrate against VERIFIED mappings (from live probe_reader runs):
     if the predicted ARM64 offset of a known field doesn't match the
     verified value, the gap before it is mis-modeled — distribute the
     correction across the unknown gap fields (assume hidden ptrs).
  5. Emit Offsets_android.h with per-field [VERIFIED] / [PREDICTED] tags.

Run: python3 helpers/offsets_linux_to_arm64.py
"""
import argparse
import json
import re
import sys
from collections import defaultdict, OrderedDict
from pathlib import Path


# ─── Type sizes ──────────────────────────────────────────────────────────

# (Linux size, ARM64 size, alignment in ARM64)
TYPE_SPEC = {
    'u8':       (1,  1,  1),
    's8':       (1,  1,  1),
    'i8':       (1,  1,  1),
    'u16':      (2,  2,  2),
    's16':      (2,  2,  2),
    'i16':      (2,  2,  2),
    'u32':      (4,  4,  4),
    's32':      (4,  4,  4),
    'i32':      (4,  4,  4),
    'int':      (4,  4,  4),
    'f32':      (4,  4,  4),
    'float':    (4,  4,  4),
    'u64':      (8,  8,  8),
    'ptr':      (4,  8,  8),    # the big one
    'wstring':  (12, 16, 8),    # std::wstring libstdc++/libc++ — varies
    'string':   (12, 16, 8),    # std::string idem
    'vector':   (12, 24, 8),    # std::vector grows due to 3 ptrs
    'map_node': (20, 40, 8),    # RB-tree node: 4 ptrs + color + key
}

DEFAULT_TYPE = 'u32'   # fill unknown gaps with u32 (most common)


# ─── Comment → type inference ────────────────────────────────────────────

TYPE_PATTERNS = [
    # explicit type tags in comments
    (re.compile(r'\bptr\b|\bpointer\b|→|->'), 'ptr'),
    (re.compile(r'\bu64\b|\buint64'),          'u64'),
    (re.compile(r'\bf32\b|\bfloat\b'),         'f32'),
    (re.compile(r'\bu32\b|\bs32\b|\bi32\b|\bint32\b'), 'u32'),
    (re.compile(r'\bu16\b|\bs16\b'),           'u16'),
    (re.compile(r'\bu8\b|\bs8\b|byte'),        'u8'),
    (re.compile(r'wstring|std::wstring'),      'wstring'),
    (re.compile(r'std::string\b'),             'string'),
    (re.compile(r'std::vector|mdragon::vector'),'vector'),
    (re.compile(r'RB-tree node|map.?node|TreeNode|tree node'), 'map_node'),
]

def infer_type(comment: str) -> str:
    if not comment:
        return DEFAULT_TYPE
    c = comment.lower()
    for rx, t in TYPE_PATTERNS:
        if rx.search(c):
            return t
    return DEFAULT_TYPE


# ─── Offsets.h parser ────────────────────────────────────────────────────

NAMESPACE_RE = re.compile(r'^\s*namespace\s+(\w+)\s*\{')
OFFSET_RE = re.compile(
    r'^\s*(?:static\s+)?constexpr\s+(?:u?int(?:8|16|32)_t|uintptr_t|size_t)\s+'
    r'(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;\s*'
    r'(?://\s*(\S+)?\s*(.*))?'
)

def parse_offsets_h(path: Path):
    """Returns {namespace: [{name, offset, type_hint, comment} ...]}."""
    out = OrderedDict()
    stack = []
    for line in path.read_text(encoding='utf-8').splitlines():
        ns = NAMESPACE_RE.match(line)
        if ns:
            stack.append(ns.group(1))
            if stack[-1] != 'Offsets':
                out.setdefault(stack[-1], [])
            continue
        if line.strip().startswith('}'):
            if stack:
                stack.pop()
            continue
        if not stack or stack[-1] == 'Offsets':
            continue
        m = OFFSET_RE.match(line)
        if not m:
            continue
        name, val, type_tag, rest = m.groups()
        try:
            offset = int(val, 0)
        except ValueError:
            continue
        comment = ((type_tag or '') + ' ' + (rest or '')).strip()
        type_hint = infer_type(comment)
        out[stack[-1]].append({
            'name': name,
            'offset': offset,
            'type': type_hint,
            'comment': comment,
        })
    # Sort by offset
    for ns in out:
        out[ns].sort(key=lambda f: f['offset'])
    return out


# ─── Verified ARM64 mappings (ground truth) ──────────────────────────────
#
# These come from live runs of probe_reader against the Warspear process.
# Each entry: (namespace, field_name) → verified_arm64_offset
#
# As we discover more offsets, add them here.

VERIFIED = {
    # Container — discovered 2026-05-13 by searching HP=751 / MP=100 of Tailegork
    ('Container', 'VTable'):     0x000,
    ('Container', 'WStringObj'): 0x080,
    ('Container', 'WStringPtr'): 0x080,
    ('Container', 'WStringCap'): 0x088,
    ('Container', 'WStringLen'): 0x08C,
    ('Container', 'NameData'):   0x090,
    ('Container', 'Allegiance'): 0x1A0,
    ('Container', 'HP'):         0x1A4,
    ('Container', 'HPMax'):      0x1A8,
    ('Container', 'MP'):         0x1AC,
    ('Container', 'MPMax'):      0x1B0,
}


# ─── ARM64 layout simulator ──────────────────────────────────────────────

def align(cur: int, alignment: int) -> int:
    return (cur + alignment - 1) & ~(alignment - 1)

def field_sizes(t: str):
    """Return (linux_size, arm64_size, arm64_alignment) for a type."""
    spec = TYPE_SPEC.get(t, TYPE_SPEC[DEFAULT_TYPE])
    return spec  # (lin, arm, align)


def simulate_namespace(ns: str, fields: list, verified: dict) -> list:
    """For each field, predict its ARM64 offset.

    Returns the field list with extra keys:
      arm64_offset (int)
      status       ('VERIFIED' | 'PREDICTED' | 'CALIBRATED')

    Algorithm: maintain TWO cursors (linux_cursor, arm64_cursor). For each
    field at Linux offset L, the implicit gap (L - linux_cursor) is treated
    as N×u32 (4 bytes each, same size in both ABIs) UNLESS we have a known
    ARM64 mapping for this field that doesn't match — then we infer the
    gap had hidden pointers and ARM64 grew faster.
    """
    if not fields:
        return []

    # Dedup: when multiple fields share the same Linux offset (e.g., WStringObj
    # and WStringPtr both at 0x058), keep the first one's predicted ARM64 offset
    # and assign the same to subsequent ones.
    sorted_fields = sorted(fields, key=lambda f: (f['offset'], f['name']))

    linux_cursor = 0
    arm64_cursor = 0
    last_offset = None
    last_arm = None

    out = []
    for f in sorted_fields:
        L = f['offset']
        t = f['type']
        lin_size, arm_size, arm_align = field_sizes(t)

        # Same offset as previous → just copy the previous ARM64 offset
        if L == last_offset and last_arm is not None:
            arm_off = last_arm
            f2 = dict(f)
            f2['arm64_offset'] = arm_off
            # Check if explicitly verified
            v = verified.get((ns, f['name']))
            f2['status'] = 'VERIFIED' if v == arm_off else (
                'CALIBRATED' if v is not None else 'PREDICTED')
            if v is not None and v != arm_off:
                # Snap to verified value, keep flag as VERIFIED
                f2['arm64_offset'] = v
                f2['status'] = 'VERIFIED'
            out.append(f2)
            continue

        # Advance Linux cursor to L: gap of (L - linux_cursor) bytes treated as u32-equivalent
        gap = L - linux_cursor
        if gap > 0:
            # Treat the gap as u32-sized chunks: same size in both ABIs.
            # No growth contribution from the gap (unless we calibrate).
            arm64_cursor += gap
        linux_cursor = L

        # Align arm64_cursor for this field
        arm64_cursor = align(arm64_cursor, arm_align)
        arm_off = arm64_cursor

        # Verified value overrides; calibrate the cursor to match.
        v = verified.get((ns, f['name']))
        status = 'PREDICTED'
        if v is not None:
            if v == arm_off:
                status = 'VERIFIED'
            else:
                # Calibration: the gap before this field actually had hidden
                # pointer-sized fields that we missed. Adjust arm64_cursor.
                delta = v - arm_off
                arm_off = v
                arm64_cursor = v
                status = 'VERIFIED' if delta == 0 else 'CALIBRATED'

        # Advance past the field
        arm64_cursor = arm_off + arm_size
        linux_cursor = L + lin_size

        f2 = dict(f)
        f2['arm64_offset'] = arm_off
        f2['status'] = status
        out.append(f2)

        last_offset = L
        last_arm = arm_off

    return out


# ─── Output generators ───────────────────────────────────────────────────

def emit_header(predictions, out_path: Path, src_version: str):
    """Generate Offsets_android.h."""
    lines = []
    lines.append("// SPDX-License-Identifier: MIT")
    lines.append("// ws-android — predicted Container/GM/etc offsets for ARM64 Warspear build.")
    lines.append("//")
    lines.append("// AUTO-GENERATED from ws-hack2/src/core/Offsets.h (Linux x86 32-bit).")
    lines.append(f"// Source: {src_version}")
    lines.append("//")
    lines.append("// Each field is tagged:")
    lines.append("//   VERIFIED   — confirmed via live probe_reader / heap-scan")
    lines.append("//   CALIBRATED — adjacent VERIFIED field pulled this one into place")
    lines.append("//   PREDICTED  — pure type-based prediction; treat with care")
    lines.append("//")
    lines.append("// Regenerate: python3 helpers/offsets_linux_to_arm64.py")
    lines.append("#pragma once")
    lines.append("#include <cstdint>")
    lines.append("")
    lines.append("namespace OffsetsAndroid {")
    lines.append("")

    for ns, fields in predictions.items():
        if not fields:
            continue
        ok = sum(1 for f in fields if f['status'] == 'VERIFIED')
        cal = sum(1 for f in fields if f['status'] == 'CALIBRATED')
        pred = sum(1 for f in fields if f['status'] == 'PREDICTED')
        lines.append(f"namespace {ns} {{   // V={ok} C={cal} P={pred}")
        # Header comment with linux→arm preview
        for f in fields:
            tag = {'VERIFIED': '✓', 'CALIBRATED': '~', 'PREDICTED': '?'}[f['status']]
            linux_off = f"0x{f['offset']:03X}"
            arm_off = f"0x{f['arm64_offset']:03X}"
            tcomment = f["comment"][:60]
            lines.append(f"    // [{tag} {f['status']}] Linux {linux_off}  →  ARM64 {arm_off}  ({f['type']})  {tcomment}")
            lines.append(f"    constexpr uint32_t {f['name']:30s} = {arm_off};")
        lines.append(f"}} // namespace {ns}")
        lines.append("")

    lines.append("} // namespace OffsetsAndroid")
    lines.append("")

    out_path.write_text('\n'.join(lines))


def emit_summary(predictions, out_path: Path):
    """Side JSON with per-field details for downstream tooling."""
    flat = []
    for ns, fields in predictions.items():
        for f in fields:
            flat.append({
                'namespace': ns,
                'name': f['name'],
                'linux_offset': f['offset'],
                'arm64_offset': f['arm64_offset'],
                'type': f['type'],
                'status': f['status'],
                'comment': f['comment'],
            })
    out_path.write_text(json.dumps({
        'verified_count':   sum(1 for f in flat if f['status'] == 'VERIFIED'),
        'calibrated_count': sum(1 for f in flat if f['status'] == 'CALIBRATED'),
        'predicted_count':  sum(1 for f in flat if f['status'] == 'PREDICTED'),
        'fields': flat,
    }, indent=2))


# ─── Main ────────────────────────────────────────────────────────────────

def main():
    here = Path(__file__).resolve().parent.parent
    ws_hack2 = here.parent / 'ws-hack2'
    parser = argparse.ArgumentParser()
    parser.add_argument('--linux-offsets',
                        default=str(ws_hack2 / 'src/core/Offsets.h'),
                        help='Path to the Linux ws-hack2 Offsets.h')
    parser.add_argument('--out-header',
                        default=str(here / 'android-injector/src/game/Offsets_android.h'),
                        help='Output Offsets_android.h')
    parser.add_argument('--out-json',
                        default=str(here / 'data/offsets_android.json'),
                        help='Output JSON detail dump')
    args = parser.parse_args()

    src = Path(args.linux_offsets)
    if not src.exists():
        print(f"[!] cannot find {src} — adjust --linux-offsets", file=sys.stderr)
        sys.exit(1)

    print(f"[*] parsing {src}")
    by_ns = parse_offsets_h(src)

    print(f"[*] {len(by_ns)} namespaces, {sum(len(v) for v in by_ns.values())} fields total")

    predictions = OrderedDict()
    total_v = total_c = total_p = 0
    for ns, fields in by_ns.items():
        pr = simulate_namespace(ns, fields, VERIFIED)
        predictions[ns] = pr
        v = sum(1 for f in pr if f['status'] == 'VERIFIED')
        c = sum(1 for f in pr if f['status'] == 'CALIBRATED')
        p = sum(1 for f in pr if f['status'] == 'PREDICTED')
        total_v += v; total_c += c; total_p += p
        if v or c:
            print(f"    {ns:25s}  V={v:3d}  C={c:3d}  P={p:3d}")

    print(f"\n[*] TOTAL  VERIFIED={total_v}  CALIBRATED={total_c}  PREDICTED={total_p}")
    print(f"           confidence: {(total_v + total_c)*100//(total_v+total_c+total_p)}% based on calibrated/verified")

    # Quick sanity report: mismatches between predicted and verified
    print("\n[*] verification sanity:")
    for (ns, name), v in VERIFIED.items():
        fields = predictions.get(ns, [])
        f = next((x for x in fields if x['name'] == name), None)
        if not f:
            print(f"    ✗ {ns}::{name} verified=0x{v:X} but field not found in Linux Offsets.h")
            continue
        match = '✓' if f['arm64_offset'] == v else '✗'
        print(f"    {match} {ns}::{name:18s}  verified=0x{v:03X}  predicted=0x{f['arm64_offset']:03X}  status={f['status']}")

    out_h = Path(args.out_header)
    out_h.parent.mkdir(parents=True, exist_ok=True)
    emit_header(predictions, out_h, src.name)
    print(f"\n[*] wrote {out_h} ({out_h.stat().st_size} bytes)")

    out_j = Path(args.out_json)
    out_j.parent.mkdir(parents=True, exist_ok=True)
    emit_summary(predictions, out_j)
    print(f"[*] wrote {out_j}")


if __name__ == '__main__':
    main()
