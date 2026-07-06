// SPDX-License-Identifier: MIT
// ws-android — predicted Container/GM/etc offsets for ARM64 Warspear build.
//
// AUTO-GENERATED from ws-hack2/src/core/Offsets.h (Linux x86 32-bit).
// Source: Offsets.h
//
// Each field is tagged:
//   VERIFIED   — confirmed via live probe_reader / heap-scan
//   CALIBRATED — adjacent VERIFIED field pulled this one into place
//   PREDICTED  — pure type-based prediction; treat with care
//
// Regenerate: python3 helpers/offsets_linux_to_arm64.py
#pragma once
#include <cstdint>

namespace OffsetsAndroid {

namespace BSS {   // V=0 C=0 P=4
    // [? PREDICTED] Linux 0xDE0730  →  ARM64 0xDE0730  (ptr)  ptr → main game state singleton (Ghidra: DAT_00DF0730)
    constexpr uint32_t AppState                       = 0xDE0730;
    // [? PREDICTED] Linux 0xDED060  →  ARM64 0xDED068  (ptr)  ptr → GSI (vtable RVA 0xDC548C)
    constexpr uint32_t GlobalSystemInstance           = 0xDED068;
    // [? PREDICTED] Linux 0xDED0E8  →  ARM64 0xDED0F8  (ptr)  ptr → instance (vtable RVA 0xDC5B68)
    constexpr uint32_t EntityManager                  = 0xDED0F8;
    // [? PREDICTED] Linux 0xDED950  →  ARM64 0xDED968  (ptr)  ptr → instance (vtable RVA 0xDC81CC)
    constexpr uint32_t OtherManager                   = 0xDED968;
} // namespace BSS

namespace AppState {   // V=0 C=0 P=31
    // [? PREDICTED] Linux 0x014  →  ARM64 0x018  (ptr)  ptr → GM (0x4528 bytes). Same object as GSI+0x28 chain.
    constexpr uint32_t GameManager                    = 0x018;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x020  (ptr)  ptr → Connection/session manager
    constexpr uint32_t SessionMgr                     = 0x020;
    // [? PREDICTED] Linux 0x024  →  ARM64 0x030  (ptr)  ptr → Payment/auth subsystem
    constexpr uint32_t PaymentMgr                     = 0x030;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x040  (ptr)  ptr → Scene/World manager
    constexpr uint32_t SceneManager                   = 0x040;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x048  (ptr)  ptr → Chat/Social manager (+0x57F=enabled, +0x114=channel)
    constexpr uint32_t ChatManager                    = 0x048;
    // [? PREDICTED] Linux 0x048  →  ARM64 0x060  (ptr)  ptr → Item definitions (binary tree, FUN_0096A450)
    constexpr uint32_t ItemDataTable                  = 0x060;
    // [? PREDICTED] Linux 0x050  →  ARM64 0x070  (ptr)  ptr → MonsterData_v2 (binary tree, FUN_0095BA30)
    constexpr uint32_t MonsterDataTable               = 0x070;
    // [? PREDICTED] Linux 0x08C  →  ARM64 0x0B0  (ptr)  ptr → Guild skill info
    constexpr uint32_t GuildSkillDT                   = 0x0B0;
    // [? PREDICTED] Linux 0x098  →  ARM64 0x0C0  (ptr)  ptr → Skill definitions (FUN_00934490)
    constexpr uint32_t SkillDataTable                 = 0x0C0;
    // [? PREDICTED] Linux 0x0CC  →  ARM64 0x0F8  (ptr)  ptr → Achievement groups
    constexpr uint32_t AchievGroupDT                  = 0x0F8;
    // [? PREDICTED] Linux 0x0D0  →  ARM64 0x100  (ptr)  ptr → Achievement goals
    constexpr uint32_t AchievGoalDT                   = 0x100;
    // [? PREDICTED] Linux 0x0D4  →  ARM64 0x108  (ptr)  ptr → Influence/buff definitions (FUN_0093AA20)
    constexpr uint32_t InfluenceDT                    = 0x108;
    // [? PREDICTED] Linux 0x0D8  →  ARM64 0x110  (ptr)  ptr → Influence descriptors extended
    constexpr uint32_t InfluenceDescDT                = 0x110;
    // [? PREDICTED] Linux 0x0E8  →  ARM64 0x128  (ptr)  ptr → Castle buildings
    constexpr uint32_t CastleBuildingDT               = 0x128;
    // [? PREDICTED] Linux 0x108  →  ARM64 0x150  (ptr)  ptr → Talent definitions (FUN_0093F7F0)
    constexpr uint32_t TalentDT                       = 0x150;
    // [? PREDICTED] Linux 0x10C  →  ARM64 0x158  (ptr)  ptr → Talent tree records
    constexpr uint32_t TalentTreeDT                   = 0x158;
    // [? PREDICTED] Linux 0x11C  →  ARM64 0x170  (ptr)  ptr → Territory data
    constexpr uint32_t TerritoryDT                    = 0x170;
    // [? PREDICTED] Linux 0x120  →  ARM64 0x178  (ptr)  ptr → Sector/World DataTable (World_Data.cpp)
    constexpr uint32_t SectorWorldDT                  = 0x178;
    // [? PREDICTED] Linux 0x124  →  ARM64 0x180  (ptr)  ptr → Interactive object definitions
    constexpr uint32_t InteractObjDT                  = 0x180;
    // [? PREDICTED] Linux 0x128  →  ARM64 0x188  (ptr)  ptr → Class definitions (1-20, FUN_00943B60)
    constexpr uint32_t ClassDT                        = 0x188;
    // [? PREDICTED] Linux 0x134  →  ARM64 0x198  (ptr)  ptr → Currency definitions (gold, miracle coins)
    constexpr uint32_t CurrencyDT                     = 0x198;
    // [? PREDICTED] Linux 0x13C  →  ARM64 0x1A8  (ptr)  ptr → Item filter categories
    constexpr uint32_t ItemFilterDT                   = 0x1A8;
    // [? PREDICTED] Linux 0x144  →  ARM64 0x1B8  (ptr)  ptr → Expansion offers
    constexpr uint32_t ExpansionOfferDT               = 0x1B8;
    // [? PREDICTED] Linux 0x15C  →  ARM64 0x1D8  (ptr)  ptr → Summon skill data
    constexpr uint32_t SummonSkillDT                  = 0x1D8;
    // [? PREDICTED] Linux 0x168  →  ARM64 0x1E8  (ptr)  ptr → Generic descriptors
    constexpr uint32_t GenericDescDT                  = 0x1E8;
    // [? PREDICTED] Linux 0x180  →  ARM64 0x208  (ptr)  ptr → Skill DataTable (from packet handlers)
    constexpr uint32_t SkillDT2                       = 0x208;
    // [? PREDICTED] Linux 0x198  →  ARM64 0x228  (ptr)  ptr → Notification subsystem
    constexpr uint32_t NotificationMgr                = 0x228;
    // [? PREDICTED] Linux 0x1E0  →  ARM64 0x274  (u32)  obj embedded account data structure
    constexpr uint32_t AccountInfo                    = 0x274;
    // [? PREDICTED] Linux 0x208  →  ARM64 0x29C  (u32)  u32 auth/login state flag
    constexpr uint32_t AuthState                      = 0x29C;
    // [? PREDICTED] Linux 0x3C8  →  ARM64 0x45C  (u32)  u32 server ID (written by LoginManager from ws_config.xml)
    constexpr uint32_t ServerId                       = 0x45C;
    // [? PREDICTED] Linux 0x3CC  →  ARM64 0x460  (u32)  u32 scene/area data
    constexpr uint32_t SceneData                      = 0x460;
} // namespace AppState

namespace AccountInfo {   // V=0 C=0 P=11
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  u32 game server id
    constexpr uint32_t CD_ServerId                    = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u32)  u32 character id
    constexpr uint32_t CD_CharId                      = 0x004;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x010  (ptr)  ptr → ASCII name (SSO at +0x18)
    constexpr uint32_t CD_NamePtr                     = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x01C  (u32)  u32 name length
    constexpr uint32_t CD_NameSize                    = 0x01C;
    // [? PREDICTED] Linux 0x074  →  ARM64 0x080  (ptr)  ptr → character entry array
    constexpr uint32_t CharArrayBase                  = 0x080;
    // [? PREDICTED] Linux 0x078  →  ARM64 0x088  (u32)  u32 number of characters
    constexpr uint32_t CharCount                      = 0x088;
    // [? PREDICTED] Linux 0x258  →  ARM64 0x268  (ptr)  ptr → server name array (stride 0x5C)
    constexpr uint32_t ServerArray                    = 0x268;
    // [? PREDICTED] Linux 0x25C  →  ARM64 0x270  (u32)  u32 number of servers
    constexpr uint32_t ServerCount                    = 0x270;
    // [? PREDICTED] Linux 0x260  →  ARM64 0x274  (u32)  u32 currently selected index
    constexpr uint32_t SelectedIdx                    = 0x274;
    // [? PREDICTED] Linux 0x264  →  ARM64 0x278  (ptr)  ptr → callback object
    constexpr uint32_t CallbackPtr                    = 0x278;
    // [? PREDICTED] Linux 0x268  →  ARM64 0x280  (ptr)  ptr → callback inner
    constexpr uint32_t CallbackInner                  = 0x280;
} // namespace AccountInfo

namespace SceneMgr {   // V=0 C=0 P=4
    // [? PREDICTED] Linux 0x1C4  →  ARM64 0x1C8  (ptr)  ptr → view controller object
    constexpr uint32_t ViewCtrl                       = 0x1C8;
    // [? PREDICTED] Linux 0x5B0  →  ARM64 0x5B8  (ptr)  ptr → refcounted callback object (for SceneTransition)
    constexpr uint32_t CallbackPtr                    = 0x5B8;
    // [? PREDICTED] Linux 0x5B4  →  ARM64 0x5C0  (ptr)  ptr → callback context
    constexpr uint32_t CallbackCtx                    = 0x5C0;
    // [? PREDICTED] Linux 0x10C0  →  ARM64 0x10D0  (ptr)  ptr → form reference (passed to FormShow)
    constexpr uint32_t FormRef                        = 0x10D0;
} // namespace SceneMgr

namespace ViewCtrl {   // V=0 C=0 P=3
    // [? PREDICTED] Linux 0x321C  →  ARM64 0x321C  (u32)  u32 current view mode param
    constexpr uint32_t ViewMode                       = 0x321C;
    // [? PREDICTED] Linux 0x3234  →  ARM64 0x3234  (u16)  u16 current scene ID (5=news popup, 4=post-popup)
    constexpr uint32_t SceneId                        = 0x3234;
    // [? PREDICTED] Linux 0x3236  →  ARM64 0x3236  (u8)  u8 scene transition flag
    constexpr uint32_t SceneFlag                      = 0x3236;
} // namespace ViewCtrl

namespace GSI {   // V=0 C=0 P=1
    // [? PREDICTED] Linux 0x028  →  ARM64 0x028  (ptr)  ptr → real GameManager (heap)
    constexpr uint32_t GameManager                    = 0x028;
} // namespace GSI

namespace GM {   // V=0 C=0 P=50
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x020  (ptr)  ptr → EntityTreeHeader → [+0] = root node  (real_GM+0x38)
    constexpr uint32_t EntityTree                     = 0x020;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x028  (ptr)  ptr → Container* (LocalPlayer)  (real_GM+0x3C)
    constexpr uint32_t Player                         = 0x028;
    // [? PREDICTED] Linux 0x028  →  ARM64 0x038  (ptr)  ptr → CorpseTreeHeader (dead mobs with loot) (real_GM+0x44)
    constexpr uint32_t CorpseTree                     = 0x038;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x048  (ptr)  ptr → InteractiveObjectsTree (portals, doors, altars)
    constexpr uint32_t ObjTree                        = 0x048;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x050  (ptr)  ptr → container (appears after first quest accept)
    constexpr uint32_t NpcTracking                    = 0x050;
    // [? PREDICTED] Linux 0x03C  →  ARM64 0x060  (ptr)  ptr → Container* (confirmed: Inventory, SkillsManager)
    constexpr uint32_t LocalPlayer                    = 0x060;
    // [? PREDICTED] Linux 0x080  →  ARM64 0x0A8  (u32)  obj combat action queue (ActionQueue.cpp)
    constexpr uint32_t ActionQueue                    = 0x0A8;
    // [? PREDICTED] Linux 0x0A0  →  ARM64 0x0C8  (u32)  u32 local player level
    constexpr uint32_t PlayerLevel                    = 0x0C8;
    // [? PREDICTED] Linux 0x0A4  →  ARM64 0x0CC  (u32)  u32 bag expansion level (< 5) (ItemsManager.cpp)
    constexpr uint32_t BagTier                        = 0x0CC;
    // [? PREDICTED] Linux 0x0A8  →  ARM64 0x0D0  (ptr)  SkillsManager embedded object. Access: player->gm_ptr + 0xA8
    constexpr uint32_t SkillsManager                  = 0x0D0;
    // [? PREDICTED] Linux 0x0A8  →  ARM64 0x0D0  (u32)  obj target selection manager (LocalPlayer.cpp)
    constexpr uint32_t TargetMgr                      = 0x0D0;
    // [? PREDICTED] Linux 0x134  →  ARM64 0x160  (u32)  obj ItemsManager (Ghidra: FUN_00872790)
    constexpr uint32_t ItemsManager                   = 0x160;
    // [? PREDICTED] Linux 0x19C  →  ARM64 0x1C8  (u32)  obj inventory sub-manager (Inventory.cpp)
    constexpr uint32_t InventoryMgr                   = 0x1C8;
    // [? PREDICTED] Linux 0x220  →  ARM64 0x250  (ptr)  ptr* currency amounts BST header (real_GM+0x23C, shifted: -0
    constexpr uint32_t CurrencyBST                    = 0x250;
    // [? PREDICTED] Linux 0x2F4  →  ARM64 0x328  (u32)  obj QuestManager (observer pattern) — real_GM+0x310
    constexpr uint32_t QuestProgressMgr               = 0x328;
    // [? PREDICTED] Linux 0x32C  →  ARM64 0x360  (u32)  u32 quest ID of current dialog (0 if none)
    constexpr uint32_t QuestDialogId                  = 0x360;
    // [? PREDICTED] Linux 0x330  →  ARM64 0x364  (u8)  u8[4] {state, 1, sub_state, 0}
    constexpr uint32_t QuestDialogFlags               = 0x364;
    // [? PREDICTED] Linux 0x334  →  ARM64 0x368  (u32)  vector of UI elements for dialog
    constexpr uint32_t QuestDialogVectors             = 0x368;
    // [? PREDICTED] Linux 0x378  →  ARM64 0x3B0  (ptr)  ptr → struct with quest_id at +0x08
    constexpr uint32_t QuestDialogProgress            = 0x3B0;
    // [? PREDICTED] Linux 0x380  →  ARM64 0x3BC  (u32)  u32 quest ID duplicate (current dialog)
    constexpr uint32_t QuestDialogId2                 = 0x3BC;
    // [? PREDICTED] Linux 0x39C  →  ARM64 0x3D8  (ptr)  ptr → array of quest entry pointers
    constexpr uint32_t QuestListPtr                   = 0x3D8;
    // [? PREDICTED] Linux 0x3A0  →  ARM64 0x3E0  (u32)  u32 count of active quests
    constexpr uint32_t QuestListCount                 = 0x3E0;
    // [? PREDICTED] Linux 0x468  →  ARM64 0x4A8  (u32)  obj combat floating numbers manager
    constexpr uint32_t CombatDisplay                  = 0x4A8;
    // [? PREDICTED] Linux 0x470  →  ARM64 0x4B0  (u32)  obj PartyManager (+0x20=member tree, +0x24=leader key)
    constexpr uint32_t PartyManager                   = 0x4B0;
    // [? PREDICTED] Linux 0x498  →  ARM64 0x4D8  (ptr)  obj MinimapManager (entity_key→hp%, alive, type)
    constexpr uint32_t MinimapMgr                     = 0x4D8;
    // [? PREDICTED] Linux 0x4C8  →  ARM64 0x50C  (u32)  obj LootManager (Ghidra: handler uses real_GM+0x4e4, shifted
    constexpr uint32_t LootManager                    = 0x50C;
    // [? PREDICTED] Linux 0x610  →  ARM64 0x654  (u32)  obj quest progress sub-manager (Ghidra: FUN_00842E90)
    constexpr uint32_t QuestProgressSub               = 0x654;
    // [? PREDICTED] Linux 0x624  →  ARM64 0x668  (u32)  u32 territory_id (key for area_names.json: "{mapzone}_{tid}"
    constexpr uint32_t RegionId                       = 0x668;
    // [? PREDICTED] Linux 0x6F8  →  ARM64 0x73C  (u32)  obj quest UI state (Ghidra: FUN_0084CC10)
    constexpr uint32_t QuestUIState                   = 0x73C;
    // [? PREDICTED] Linux 0x72E  →  ARM64 0x772  (u16)  u16 region_id (from ExplorationData)
    constexpr uint32_t RegionIdNew                    = 0x772;
    // [? PREDICTED] Linux 0x81C  →  ARM64 0x860  (u8)  u8 is dungeon
    constexpr uint32_t DungeonFlag                    = 0x860;
    // [? PREDICTED] Linux 0x820  →  ARM64 0x864  (u32)  u32 dungeon instance ID
    constexpr uint32_t DungeonId                      = 0x864;
    // [? PREDICTED] Linux 0x1018  →  ARM64 0x105C  (u32)  obj outgoing packet queue (SkillsManager.cpp)
    constexpr uint32_t PacketSendQueue                = 0x105C;
    // [? PREDICTED] Linux 0x1038  →  ARM64 0x107C  (u32)  u32 game zone ID (not same as map file zone!)
    constexpr uint32_t ZoneId                         = 0x107C;
    // [? PREDICTED] Linux 0x10F4  →  ARM64 0x1138  (u32)  u32 0=walkable, 1=blocked (tile under mouse)
    constexpr uint32_t CursorBlocked                  = 0x1138;
    // [? PREDICTED] Linux 0x11EC  →  ARM64 0x1230  (ptr)  ptr → Camera Object
    constexpr uint32_t Camera                         = 0x1230;
    // [? PREDICTED] Linux 0x11F4  →  ARM64 0x1240  (ptr)  ptr cursor/tile selection struct
    constexpr uint32_t CursorStruct                   = 0x1240;
    // [? PREDICTED] Linux 0x1224  →  ARM64 0x1274  (u16)  u16 auth token (2 bytes, set by Handler_LoginResult on succe
    constexpr uint32_t LoginToken                     = 0x1274;
    // [? PREDICTED] Linux 0x1228  →  ARM64 0x1278  (u16)  u16 zone metadata block start
    constexpr uint32_t ZoneMeta                       = 0x1278;
    // [? PREDICTED] Linux 0x12F8  →  ARM64 0x1348  (u32)  u32 1=session active (set by Handler_CharacterList after cha
    constexpr uint32_t SessionActive                  = 0x1348;
    // [? PREDICTED] Linux 0x1560  →  ARM64 0x15B0  (ptr)  ptr → collision grid struct (GDB confirmed, Ghidra FUN_008C1
    constexpr uint32_t CollisionGrid                  = 0x15B0;
    // [? PREDICTED] Linux 0x1580  →  ARM64 0x15D8  (ptr)  ptr MapViewContainer (0x230 bytes, world map)
    constexpr uint32_t MapView                        = 0x15D8;
    // [? PREDICTED] Linux 0x15C4  →  ARM64 0x1620  (ptr)  ptr quest tracking enable/disable (Ghidra: FUN_007F8580)
    constexpr uint32_t QuestTracker                   = 0x1620;
    // [? PREDICTED] Linux 0x15D4  →  ARM64 0x1638  (ptr)  ptr quest entry array end (Ghidra: FUN_0031FDC0)
    constexpr uint32_t QuestEntryEnd                  = 0x1638;
    // [? PREDICTED] Linux 0x15DC  →  ARM64 0x1648  (ptr)  ptr quest entry array begin
    constexpr uint32_t QuestEntryBegin                = 0x1648;
    // [? PREDICTED] Linux 0x15E0  →  ARM64 0x1650  (u32)  u32 quest entry count (internal, may differ from +0x3A0)
    constexpr uint32_t QuestEntryCount2               = 0x1650;
    // [? PREDICTED] Linux 0x1610  →  ARM64 0x1680  (u32)  arr packet handler table (602 slots × 8B)
    constexpr uint32_t HandlerTable                   = 0x1680;
    // [? PREDICTED] Linux 0x28E0  →  ARM64 0x2950  (u32)  handler table end
    constexpr uint32_t HandlerTableEnd                = 0x2950;
    // [? PREDICTED] Linux 0x28E4  →  ARM64 0x2954  (u32)  arr per-opcode counters (opcode × 0xC)
    constexpr uint32_t PacketStats                    = 0x2954;
    // [? PREDICTED] Linux 0x2D80  →  ARM64 0x2DF0  (u32)  array of 12B entries: {count1:u32, count2:u32, area_id:u32}
    constexpr uint32_t AreaList                       = 0x2DF0;
} // namespace GM

namespace ConnectMgr {   // V=0 C=0 P=2
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  u32 non-zero if network connection active
    constexpr uint32_t IsConnected                    = 0x014;
    // [? PREDICTED] Linux 0x088  →  ARM64 0x088  (u32)  u32 current connection phase
    constexpr uint32_t ConnectionState                = 0x088;
} // namespace ConnectMgr

namespace Container {   // V=8 C=3 P=60
    // [✓ VERIFIED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr → .rodata vtable
    constexpr uint32_t VTable                         = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (u32)  u32 3=SELF, 1=OTHER (player/NPC/mob)
    constexpr uint32_t Type                           = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00C  (u8)  bytes between equipment slots
    constexpr uint32_t EquipStride                    = 0x00C;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00C  (u32)  u32 tile_x = bits[31:16], tile_y = bits[15:0]
    constexpr uint32_t PosPacked                      = 0x00C;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x010  (u32)  u32 changes with movement (not a direct coord)
    constexpr uint32_t PosState                       = 0x010;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x014  (u32)  u32 16.16 fixed point. Upper 16 = 24*tile_y + 12
    constexpr uint32_t WorldX                         = 0x014;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x018  (u32)  u32 16.16 fixed point. Upper 16 = 24*tile_x + 12
    constexpr uint32_t WorldY                         = 0x018;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x020  (u8)  u8 team color byte (0xFF=unset). Ghidra: FUN_008C2F30
    constexpr uint32_t TeamColor                      = 0x020;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x034  (u32)  u32 16.16 fixed (0x10000=1.0). Ghidra: BaseObject ctor
    constexpr uint32_t ScaleX                         = 0x034;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x038  (u32)  u32 16.16 fixed (0x10000=1.0). Ghidra: BaseObject ctor
    constexpr uint32_t ScaleY                         = 0x038;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x040  (ptr)  ptr points to self (validation invariant)
    constexpr uint32_t SelfPtr                        = 0x040;
    // [~ CALIBRATED] Linux 0x058  →  ARM64 0x080  (wstring)  12B std::wstring {_M_p(4), capacity(4), length(4)}
    constexpr uint32_t WStringObj                     = 0x080;
    // [✓ VERIFIED] Linux 0x058  →  ARM64 0x080  (ptr)  ptr _M_p: data pointer (SSO: points to +0x064)
    constexpr uint32_t WStringPtr                     = 0x080;
    // [~ CALIBRATED] Linux 0x05C  →  ARM64 0x088  (u32)  u32 capacity (19 = SSO)
    constexpr uint32_t WStringCap                     = 0x088;
    // [✓ VERIFIED] Linux 0x060  →  ARM64 0x08C  (u32)  u32 length in wchars
    constexpr uint32_t WStringLen                     = 0x08C;
    // [✓ VERIFIED] Linux 0x064  →  ARM64 0x090  (u8)  wchar_t[] name UTF-32LE (4 bytes/char)
    constexpr uint32_t NameData                       = 0x090;
    // [? PREDICTED] Linux 0x0B8  →  ARM64 0x0E4  (u32)  u32 1=player, 2=NPC/mob/object (NOT level, NOT model_id!)
    constexpr uint32_t SubType                        = 0x0E4;
    // [? PREDICTED] Linux 0x0EC  →  ARM64 0x118  (ptr)  ptr sub-object vtable 1
    constexpr uint32_t SubVTable1                     = 0x118;
    // [? PREDICTED] Linux 0x0F4  →  ARM64 0x128  (ptr)  ptr sub-object vtable 2 (multiple inheritance)
    constexpr uint32_t SubVTable2                     = 0x128;
    // [? PREDICTED] Linux 0x0F8  →  ARM64 0x130  (ptr)  ptr back-pointer to parent container
    constexpr uint32_t SubParent                      = 0x130;
    // [? PREDICTED] Linux 0x12C  →  ARM64 0x168  (u32)  u32 entity key (same as TreeNode::Key). Ghidra: FUN_008C1210
    constexpr uint32_t EntityKey                      = 0x168;
    // [~ CALIBRATED] Linux 0x130  →  ARM64 0x1A0  (u8)  u8 combat allegiance (0..6). Ghidra: FUN_008C1220 (BaseActor
    constexpr uint32_t Allegiance                     = 0x1A0;
    // [✓ VERIFIED] Linux 0x134  →  ARM64 0x1A4  (u32)  s32 current HP (clamped [0,HPMax]). Ghidra: setter [0x4D]
    constexpr uint32_t HP                             = 0x1A4;
    // [✓ VERIFIED] Linux 0x138  →  ARM64 0x1A8  (u32)  s32 max HP. Ghidra: setter [0x4E]
    constexpr uint32_t HPMax                          = 0x1A8;
    // [✓ VERIFIED] Linux 0x13C  →  ARM64 0x1AC  (u32)  s32 current MP/energy. Ghidra: setter [0x4F]
    constexpr uint32_t MP                             = 0x1AC;
    // [✓ VERIFIED] Linux 0x140  →  ARM64 0x1B0  (u32)  s32 max MP. Ghidra: setter [0x50]
    constexpr uint32_t MPMax                          = 0x1B0;
    // [? PREDICTED] Linux 0x144  →  ARM64 0x1B4  (u8)  u8 0=unarmed, 1=melee. Ghidra: FUN_008C7D20
    constexpr uint32_t AttackType                     = 0x1B4;
    // [? PREDICTED] Linux 0x148  →  ARM64 0x1B8  (u32)  i32 animation X pos   (= sub + 0x5C)
    constexpr uint32_t AnimPosX                       = 0x1B8;
    // [? PREDICTED] Linux 0x14C  →  ARM64 0x1BC  (u32)  i32 animation Y pos   (= sub + 0x60)
    constexpr uint32_t AnimPosY                       = 0x1BC;
    // [? PREDICTED] Linux 0x150  →  ARM64 0x1C0  (u32)  i32 movement dest X   (= sub + 0x64)
    constexpr uint32_t DestX                          = 0x1C0;
    // [? PREDICTED] Linux 0x154  →  ARM64 0x1C4  (u32)  i32 movement dest Y   (= sub + 0x68)
    constexpr uint32_t DestY                          = 0x1C4;
    // [? PREDICTED] Linux 0x158  →  ARM64 0x1C8  (u32)  u32 velocity magnitude. Ghidra: BaseActor ctor
    constexpr uint32_t MoveSpeed                      = 0x1C8;
    // [? PREDICTED] Linux 0x168  →  ARM64 0x1D8  (u32)  s32 delta X per movement tick. Ghidra: MovementTick
    constexpr uint32_t VelocityX                      = 0x1D8;
    // [? PREDICTED] Linux 0x16C  →  ARM64 0x1DC  (u32)  s32 delta Y per movement tick. Ghidra: MovementTick
    constexpr uint32_t VelocityY                      = 0x1DC;
    // [? PREDICTED] Linux 0x170  →  ARM64 0x1E0  (u32)  u32 0=idle, 1=walking, 8=dead. Ghidra: BaseActor ctor
    constexpr uint32_t WalkState                      = 0x1E0;
    // [? PREDICTED] Linux 0x2B0  →  ARM64 0x320  (u8)  u8 0=none, 1="!", 2="?".
    constexpr uint32_t QuestIndicator                 = 0x320;
    // [? PREDICTED] Linux 0x2B4  →  ARM64 0x324  (u32)  u32 combat animation priority (higher = more important)
    constexpr uint32_t CombatPriority                 = 0x324;
    // [? PREDICTED] Linux 0x2B8  →  ARM64 0x328  (ptr)  ptr pointer to targeted entity (0=none). Ghidra: entity buil
    constexpr uint32_t TargetPtr                      = 0x328;
    // [? PREDICTED] Linux 0x2BC  →  ARM64 0x330  (u32)  u32 target entity key (set by SetTargetByKey)
    constexpr uint32_t TargetKey                      = 0x330;
    // [? PREDICTED] Linux 0x2C4  →  ARM64 0x338  (u32)  obj visual effect/buff display list (BuffsList.cpp)
    constexpr uint32_t EffectList                     = 0x338;
    // [? PREDICTED] Linux 0x2DC  →  ARM64 0x350  (ptr)  ptr end = base + count * 0x14
    constexpr uint32_t Buff1EndPtr                    = 0x350;
    // [? PREDICTED] Linux 0x2E0  →  ARM64 0x358  (u32)  u32 allocated capacity (typically 0x20)
    constexpr uint32_t Buff1Capacity                  = 0x358;
    // [? PREDICTED] Linux 0x2E4  →  ARM64 0x360  (ptr)  ptr base pointer to buff entries
    constexpr uint32_t Buff1Base                      = 0x360;
    // [? PREDICTED] Linux 0x2E8  →  ARM64 0x368  (u32)  u32 number of active buffs
    constexpr uint32_t Buff1Count                     = 0x368;
    // [? PREDICTED] Linux 0x2EC  →  ARM64 0x370  (ptr)  ptr end = base + count * 4
    constexpr uint32_t Buff2EndPtr                    = 0x370;
    // [? PREDICTED] Linux 0x2F0  →  ARM64 0x378  (u32)  u32 allocated capacity (typically 0x20)
    constexpr uint32_t Buff2Capacity                  = 0x378;
    // [? PREDICTED] Linux 0x2F4  →  ARM64 0x380  (ptr)  ptr base pointer to entries
    constexpr uint32_t Buff2Base                      = 0x380;
    // [? PREDICTED] Linux 0x2F8  →  ARM64 0x388  (u32)  u32 number of entries
    constexpr uint32_t Buff2Count                     = 0x388;
    // [? PREDICTED] Linux 0x300  →  ARM64 0x390  (u32)  u32 faction (0, 1, or 2). Ghidra: FUN_008C2F30 (BaseActor.cp
    constexpr uint32_t Faction                        = 0x390;
    // [? PREDICTED] Linux 0x308  →  ARM64 0x398  (u8)  u8 entity level. Ghidra: FUN_008C3030 stores char (1 byte)
    constexpr uint32_t Level                          = 0x398;
    // [? PREDICTED] Linux 0x30C  →  ARM64 0x3A0  (ptr)  ptr pointer to DataTable entry (NPC/mob). NULL for players. 
    constexpr uint32_t DataTablePtr                   = 0x3A0;
    // [? PREDICTED] Linux 0x310  →  ARM64 0x3A8  (u32)  u32 combat-related flag (1 when NPC is in combat). NOT quest
    constexpr uint32_t CombatFlag                     = 0x3A8;
    // [? PREDICTED] Linux 0x314  →  ARM64 0x3AC  (u8)  u8 client-side hostility marker. In 5v5 arena: 0xE0=ally, 0x
    constexpr uint32_t HostilityFlag                  = 0x3AC;
    // [? PREDICTED] Linux 0x31C  →  ARM64 0x3B4  (u32)  u32 is building flag. Ghidra: FUN_008C2240
    constexpr uint32_t IsBuilding                     = 0x3B4;
    // [? PREDICTED] Linux 0x320  →  ARM64 0x3B8  (u32)  u32 entity key of owner (pets/summons). 0 = hostile mob.
    constexpr uint32_t OwnerKey                       = 0x3B8;
    // [? PREDICTED] Linux 0x326  →  ARM64 0x3BE  (u16)  s16 dialog active (!=0 blocks input). Ghidra: HandleInput
    constexpr uint32_t DialogState                    = 0x3BE;
    // [? PREDICTED] Linux 0x33A  →  ARM64 0x3D2  (u16)  u16 forces melee if !=0. Ghidra: UpdateAttackAnim
    constexpr uint32_t WeaponFlag                     = 0x3D2;
    // [? PREDICTED] Linux 0x368  →  ARM64 0x400  (u32)  u32 -1=alive, other=dead. Ghidra: entity builder
    constexpr uint32_t DeathMarker                    = 0x400;
    // [? PREDICTED] Linux 0x370  →  ARM64 0x408  (u32)  u32 HP regen rate (e.g. 19 = 19 un/5s). Isolated, validated
    constexpr uint32_t HPRegen                        = 0x408;
    // [? PREDICTED] Linux 0x3CC  →  ARM64 0x468  (ptr)  ptr equipment slot 0 (base)
    constexpr uint32_t EquipBase                      = 0x468;
    // [? PREDICTED] Linux 0x3DC  →  ARM64 0x480  (ptr)  ptr main weapon = slot 2 (validated with equip/unequip)
    constexpr uint32_t EquipWeapon                    = 0x480;
    // [? PREDICTED] Linux 0x3E4  →  ARM64 0x490  (ptr)  ptr armor slot 1 (body?)
    constexpr uint32_t EquipArmor1                    = 0x490;
    // [? PREDICTED] Linux 0x3EC  →  ARM64 0x4A0  (ptr)  ptr second weapon ref (same ptr as +0x3DC)
    constexpr uint32_t EquipWeapon2                   = 0x4A0;
    // [? PREDICTED] Linux 0x3FC  →  ARM64 0x4B8  (ptr)  ptr armor slot 2
    constexpr uint32_t EquipArmor2                    = 0x4B8;
    // [? PREDICTED] Linux 0x43D  →  ARM64 0x4FD  (u8)  u8 player class (1-20). Ghidra: FUN_008D8D10. 0 for NPCs.
    constexpr uint32_t ClassId                        = 0x4FD;
    // [? PREDICTED] Linux 0x43E  →  ARM64 0x4FE  (u16)  s16 appearance skin model. Ghidra: FUN_008D8D60
    constexpr uint32_t SkinId                         = 0x4FE;
    // [? PREDICTED] Linux 0x440  →  ARM64 0x500  (u16)  s16 head/face model. Ghidra: FUN_008D8F10
    constexpr uint32_t HeadId                         = 0x500;
    // [? PREDICTED] Linux 0x490  →  ARM64 0x550  (ptr)  ptr back-reference to GameManager. Ghidra: SelectTarget
    constexpr uint32_t GMPtr                          = 0x550;
    // [? PREDICTED] Linux 0x4A8  →  ARM64 0x56C  (u32)  u32 current experience points. GDB validated 2026-03-30
    constexpr uint32_t XP                             = 0x56C;
    // [? PREDICTED] Linux 0x4B8  →  ARM64 0x57C  (u32)  u32 equipment set ID (triggers rebuild). Ghidra: FUN_008C8FA
    constexpr uint32_t EquipSetId                     = 0x57C;
    // [? PREDICTED] Linux 0x4BC  →  ARM64 0x580  (u32)  u32 knowledge points. Ghidra: FUN_008C9240
    constexpr uint32_t Knowledge                      = 0x580;
} // namespace Container

namespace TreeNode {   // V=0 C=0 P=7
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr parent node (0 for root)
    constexpr uint32_t Parent                         = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (ptr)  ptr left child (0 if leaf)
    constexpr uint32_t Left                           = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x010  (ptr)  ptr right child (0 if leaf)
    constexpr uint32_t Right                          = 0x010;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x018  (u32)  u32 0=black, 1=red
    constexpr uint32_t Color                          = 0x018;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x01C  (u32)  u32 entity ID (tree key)
    constexpr uint32_t Key                            = 0x01C;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x020  (ptr)  ptr → Container of the entity
    constexpr uint32_t ObjPtr                         = 0x020;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x028  (u8)  total node size in bytes
    constexpr uint32_t Size                           = 0x028;
} // namespace TreeNode

namespace Camera {   // V=0 C=0 P=9
    // [? PREDICTED] Linux 0x030  →  ARM64 0x030  (f32)  float computed zoom (settings_scale * screen_ratio). READ ON
    constexpr uint32_t Zoom                           = 0x030;
    // [? PREDICTED] Linux 0x03C  →  ARM64 0x03C  (u32)  u32 zoom in 16.16 fixed point (65536 = 1.0), unrelated to ga
    constexpr uint32_t ZoomFixed1                     = 0x03C;
    // [? PREDICTED] Linux 0x040  →  ARM64 0x040  (u32)  u32 copy of zoom fixed
    constexpr uint32_t ZoomFixed2                     = 0x040;
    // [? PREDICTED] Linux 0x068  →  ARM64 0x068  (f32)  float camera X position (world pixel coords)
    constexpr uint32_t CamX                           = 0x068;
    // [? PREDICTED] Linux 0x06C  →  ARM64 0x06C  (f32)  float camera Y position
    constexpr uint32_t CamY                           = 0x06C;
    // [? PREDICTED] Linux 0x07A  →  ARM64 0x07C  (u32)  int16 viewport left bound (scroll position, not fixed)
    constexpr uint32_t VpLeft                         = 0x07C;
    // [? PREDICTED] Linux 0x07C  →  ARM64 0x080  (u32)  int16 viewport top bound
    constexpr uint32_t VpTop                          = 0x080;
    // [? PREDICTED] Linux 0x07E  →  ARM64 0x084  (u32)  int16 viewport right bound
    constexpr uint32_t VpRight                        = 0x084;
    // [? PREDICTED] Linux 0x080  →  ARM64 0x088  (u32)  int16 viewport bottom bound
    constexpr uint32_t VpBottom                       = 0x088;
} // namespace Camera

namespace InteractiveObject {   // V=0 C=0 P=11
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr RVA 0xDC8230 (fixed for all interactive objects)
    constexpr uint32_t VTable                         = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (u32)  u32 always 1
    constexpr uint32_t Type                           = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00C  (u32)  u32 tile_x<<16 | tile_y
    constexpr uint32_t PosPacked                      = 0x00C;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x020  (u32)  u32 0=open (can interact), 255=closed/locked
    constexpr uint32_t LockState                      = 0x020;
    // [? PREDICTED] Linux 0x02C  →  ARM64 0x030  (u32)  u32 0=no lock, 3=locked (quest/level required)
    constexpr uint32_t LockType                       = 0x030;
    // [? PREDICTED] Linux 0x058  →  ARM64 0x060  (wstring)  12B std::wstring of name
    constexpr uint32_t WStringObj                     = 0x060;
    // [? PREDICTED] Linux 0x064  →  ARM64 0x070  (u32)  wchar_t[] name UTF-32LE
    constexpr uint32_t NameData                       = 0x070;
    // [? PREDICTED] Linux 0x0B4  →  ARM64 0x0C0  (u32)  u32 sprite/icon ID
    constexpr uint32_t SpriteId                       = 0x0C0;
    // [? PREDICTED] Linux 0x0B8  →  ARM64 0x0C4  (u32)  u32 always 2
    constexpr uint32_t ObjType                        = 0x0C4;
    // [? PREDICTED] Linux 0x0FC  →  ARM64 0x108  (ptr)  ptr 0=passage/portal, nonzero=quest/plant (used by SDK isPas
    constexpr uint32_t QuestPtr                       = 0x108;
    // [? PREDICTED] Linux 0x11C  →  ARM64 0x12C  (u32)  u32 1=open/usable, 0=closed/consumed
    constexpr uint32_t OpenFlag                       = 0x12C;
} // namespace InteractiveObject

namespace ExplorerStats {   // V=0 C=0 P=3
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  u32 current Vigor do Explorador (0..5)
    constexpr uint32_t VigorCur                       = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u32)  u32 max vigor (=5)
    constexpr uint32_t VigorMax                       = 0x004;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  u32 current Oxigênio
    constexpr uint32_t OxygenCur                      = 0x014;
} // namespace ExplorerStats

namespace QuestEntry {   // V=0 C=0 P=11
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  u32 quest ID (e.g. 916, 1184, 1233)
    constexpr uint32_t QuestId                        = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (ptr)  wstring {ptr(4), cap(4), len(4)} + SSO buf(80B)
    constexpr uint32_t WStringName                    = 0x008;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x018  (u32)  wchar_t[20] SSO inline buffer (80B)
    constexpr uint32_t NameBuf                        = 0x018;
    // [? PREDICTED] Linux 0x060  →  ARM64 0x068  (wstring)  wstring static label of counter 1
    constexpr uint32_t Counter1Label                  = 0x068;
    // [? PREDICTED] Linux 0x0BC  →  ARM64 0x0C8  (wstring)  wstring static label of counter 2
    constexpr uint32_t Counter2Label                  = 0x0C8;
    // [? PREDICTED] Linux 0x118  →  ARM64 0x128  (wstring)  wstring static label of counter 3
    constexpr uint32_t Counter3Label                  = 0x128;
    // [? PREDICTED] Linux 0x174  →  ARM64 0x188  (wstring)  wstring stage text (definition, NOT progress)
    constexpr uint32_t StageText                      = 0x188;
    // [? PREDICTED] Linux 0x1D0  →  ARM64 0x1E8  (ptr)  container {ptr,0x20,ptr,count}
    constexpr uint32_t StagesContainer                = 0x1E8;
    // [? PREDICTED] Linux 0x1E0  →  ARM64 0x200  (ptr)  container {ptr,0x20,ptr,count}
    constexpr uint32_t DialogContainer                = 0x200;
    // [? PREDICTED] Linux 0x1F8  →  ARM64 0x220  (wstring)  wstring display/UI field
    constexpr uint32_t DisplayWStr                    = 0x220;
    // [? PREDICTED] Linux 0x218  →  ARM64 0x244  (u32)  u32 unknown (e.g. 31, 3)
    constexpr uint32_t Field218                       = 0x244;
} // namespace QuestEntry

namespace QuestNetwork {   // V=0 C=0 P=10
    // [? PREDICTED] Linux 0x002  →  ARM64 0x004  (u32)  
    constexpr uint32_t STATE_QID_OFFSET               = 0x004;
    // [? PREDICTED] Linux 0x003  →  ARM64 0x008  (u32)  quest_id starts at d904+3
    constexpr uint32_t D904_QID_OFFSET                = 0x008;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x00C  (u32)  
    constexpr uint32_t DETAIL_TYPE                    = 0x00C;
    // [? PREDICTED] Linux 0x00A  →  ARM64 0x014  (u32)  from pattern start (16 14 + 8)
    constexpr uint32_t STATE_PROGRESS_OFF             = 0x014;
    // [? PREDICTED] Linux 0x00F  →  ARM64 0x01C  (u32)  from pattern start (16 14 + 13)
    constexpr uint32_t STATE_TARGET_OFF               = 0x01C;
    // [? PREDICTED] Linux 0x043  →  ARM64 0x050  (u32)  progress u32LE at d904+67
    constexpr uint32_t D904_PROGRESS_OFF              = 0x050;
    // [? PREDICTED] Linux 0x047  →  ARM64 0x054  (u8)  target u8 at d904+71
    constexpr uint32_t D904_TARGET_OFF                = 0x054;
    // [? PREDICTED] Linux 0x048  →  ARM64 0x055  (u8)  minimum bytes needed from d904 start
    constexpr uint32_t D904_MIN_LEN                   = 0x055;
    // [? PREDICTED] Linux 0x4D9  →  ARM64 0x4E8  (u32)  
    constexpr uint32_t SUB_QUEST_STATE                = 0x4E8;
    // [? PREDICTED] Linux 0x9A02  →  ARM64 0x9A14  (u32)  
    constexpr uint32_t OP_QUEST_DETAIL_REQ            = 0x9A14;
} // namespace QuestNetwork

namespace QuestInstanceTreeNode {   // V=0 C=0 P=8
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u32)  u32 quest_id (BST key)
    constexpr uint32_t InnerQuestId                   = 0x010;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u32)  u32 world/zone grouping id
    constexpr uint32_t OuterKey                       = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x018  (ptr)  ptr → Quest heap struct
    constexpr uint32_t InnerQuestData                 = 0x018;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x018  (u32)  
    constexpr uint32_t OuterField14                   = 0x018;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x020  (ptr)  u32 status (not a ptr)
    constexpr uint32_t InnerField18                   = 0x020;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x020  (ptr)  ptr → inner BST container
    constexpr uint32_t OuterInnerContainer            = 0x020;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x028  (u32)  
    constexpr uint32_t InnerStatus                    = 0x028;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x028  (u32)  
    constexpr uint32_t OuterFlags                     = 0x028;
} // namespace QuestInstanceTreeNode

namespace QuestInstance {   // V=0 C=0 P=8
    // [? PREDICTED] Linux 0x008  →  ARM64 0x008  (u32)  u32 quest ID
    constexpr uint32_t QuestId                        = 0x008;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x018  (u8)  u8 quest finished flag
    constexpr uint32_t Done                           = 0x018;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x01C  (u8)  u8 stage (1-based)
    constexpr uint32_t CurrentStage                   = 0x01C;
    // [? PREDICTED] Linux 0x01D  →  ARM64 0x01D  (u8)  u8 stage objective completed
    constexpr uint32_t StageDone                      = 0x01D;
    // [? PREDICTED] Linux 0x024  →  ARM64 0x028  (ptr)  ptr → tree {root, count, flags} of ObjectiveNode
    constexpr uint32_t CountersTree                   = 0x028;
    // [? PREDICTED] Linux 0x028  →  ARM64 0x030  (ptr)  ptr → tree (purpose still unclear)
    constexpr uint32_t AuxTree                        = 0x030;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x040  (ptr)  ptr → tree of AreaMarker vectors (one per zone)
    constexpr uint32_t AreasTree                      = 0x040;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x050  (ptr)  ptr → tree of 10-byte marker entries
    constexpr uint32_t MarkersTree                    = 0x050;
} // namespace QuestInstance

namespace ObjectiveCounterNode {   // V=0 C=0 P=3
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u32)  u8 (low byte; full u32 slot)
    constexpr uint32_t StageIndex                     = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  u32 current count
    constexpr uint32_t Progress                       = 0x014;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x018  (u32)  u32 target count
    constexpr uint32_t Target                         = 0x018;
} // namespace ObjectiveCounterNode

namespace QuestAreaMarker {   // V=0 C=0 P=4
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  
    constexpr uint32_t AreaX                          = 0x000;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x004  (u32)  
    constexpr uint32_t AreaY                          = 0x004;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (u32)  
    constexpr uint32_t AreaZ                          = 0x008;
    // [? PREDICTED] Linux 0x006  →  ARM64 0x00C  (u32)  
    constexpr uint32_t Stride                         = 0x00C;
} // namespace QuestAreaMarker

namespace QuestMarkersNode {   // V=0 C=0 P=4
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u32)  u32 area/zone id
    constexpr uint32_t AreaKey                        = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  
    constexpr uint32_t VecEnd                         = 0x014;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x018  (u32)  
    constexpr uint32_t VecCap                         = 0x018;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x01C  (u32)  
    constexpr uint32_t VecBegin                       = 0x01C;
} // namespace QuestMarkersNode

namespace ObjectiveNode {   // V=0 C=0 P=3
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u8)  u8 objective index (key)
    constexpr uint32_t StageIndex                     = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  u32 current progress (e.g. 3 kills)
    constexpr uint32_t Progress                       = 0x014;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x018  (u32)  u32 target count (e.g. 5 kills)
    constexpr uint32_t Target                         = 0x018;
} // namespace ObjectiveNode

namespace QuestProgressMgrSub {   // V=0 C=0 P=22
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u32)  s32 -1=idle
    constexpr uint32_t ObserverLock                   = 0x004;
    // [? PREDICTED] Linux 0x02C  →  ARM64 0x030  (ptr)  ptr* → 12B container
    constexpr uint32_t AcceptedQuestTree              = 0x030;
    // [? PREDICTED] Linux 0x02C  →  ARM64 0x030  (u32)  
    constexpr uint32_t QuestTrackingTree              = 0x030;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x040  (ptr)  ptr* BST quest_id → vector of 10B marker entries
    constexpr uint32_t MarkerTree                     = 0x040;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x048  (u32)  u32 currently-displayed quest ID (0 when closed)
    constexpr uint32_t DialogQuestId                  = 0x048;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x048  (u32)  DEPRECATED alias for DialogQuestId
    constexpr uint32_t TrackedQuestId                 = 0x048;
    // [? PREDICTED] Linux 0x03D  →  ARM64 0x04D  (u8)  u8 0=first click shows reward preview, 1=next click confirms
    constexpr uint32_t DialogRewardAcked              = 0x04D;
    // [? PREDICTED] Linux 0x03E  →  ARM64 0x04E  (u8)  u8 state: 1,3,5=advance/deliver · 2=offer · 4,6=award-choice
    constexpr uint32_t DialogPhase                    = 0x04E;
    // [? PREDICTED] Linux 0x04C  →  ARM64 0x060  (ptr)  ptr vec<Stage> begin (stride 0x10)
    constexpr uint32_t DialogStagesBegin              = 0x060;
    // [? PREDICTED] Linux 0x050  →  ARM64 0x068  (ptr)  ptr vec<Stage> end
    constexpr uint32_t DialogStagesEnd                = 0x068;
    // [? PREDICTED] Linux 0x058  →  ARM64 0x078  (ptr)  ptr vec<Award> begin (stride 0x48, +0x44=type_id)
    constexpr uint32_t DialogAwardsBegin              = 0x078;
    // [? PREDICTED] Linux 0x05C  →  ARM64 0x080  (ptr)  ptr vec<Award> end
    constexpr uint32_t DialogAwardsEnd                = 0x080;
    // [? PREDICTED] Linux 0x084  →  ARM64 0x0B0  (ptr)  ptr refcounted parent (SceneMgr form handle)
    constexpr uint32_t DialogParentRef                = 0x0B0;
    // [? PREDICTED] Linux 0x088  →  ARM64 0x0B8  (u8)  u8 1=popup visible, 0=closed/no-op. Confirm function early-r
    constexpr uint32_t DialogActiveFlag               = 0x0B8;
    // [? PREDICTED] Linux 0x088  →  ARM64 0x0B8  (u32)  DEPRECATED alias for DialogActiveFlag
    constexpr uint32_t IsTracking                     = 0x0B8;
    // [? PREDICTED] Linux 0x094  →  ARM64 0x0C4  (u32)  u32 written to 2 after confirm finishes
    constexpr uint32_t DialogResultState              = 0x0C4;
    // [? PREDICTED] Linux 0x104  →  ARM64 0x134  (u32)  
    constexpr uint32_t OfferedQuestTree               = 0x134;
    // [? PREDICTED] Linux 0x104  →  ARM64 0x134  (u32)  back-compat (wrong semantically)
    constexpr uint32_t QuestInstanceTree              = 0x134;
    // [? PREDICTED] Linux 0x15C  →  ARM64 0x190  (ptr)  ptr* BST entity_key → floating combat text (NOT quest data)
    constexpr uint32_t FloatingTextCache              = 0x190;
    // [? PREDICTED] Linux 0x1F8  →  ARM64 0x230  (ptr)  ptr* BST quest_id → quest_data_obj* (509 entries, all quests
    constexpr uint32_t QuestDataTree                  = 0x230;
    // [? PREDICTED] Linux 0x200  →  ARM64 0x240  (ptr)  ptr* BST zone_id → u32[] quest_ids per zone (68 zones)
    constexpr uint32_t ZoneQuestTree                  = 0x240;
    // [? PREDICTED] Linux 0x220  →  ARM64 0x268  (ptr)  ptr* BST seq_key → {quest_id, status} for all known quests
    constexpr uint32_t QuestStatusTree                = 0x268;
} // namespace QuestProgressMgrSub

namespace MarkerEntry {   // V=0 C=0 P=6
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u16)  u16 chunk gx (global chunk coord)
    constexpr uint32_t GridX                          = 0x000;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x002  (u16)  u16 chunk gy (global chunk coord)
    constexpr uint32_t GridY                          = 0x002;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u16)  u16 flags (observed: 0x0080)
    constexpr uint32_t Flags                          = 0x004;
    // [? PREDICTED] Linux 0x006  →  ARM64 0x006  (u16)  u16 NPC type/model ID
    constexpr uint32_t NpcType                        = 0x006;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x008  (u16)  u16 icon code (0x2605 = ★ star)
    constexpr uint32_t MarkerIcon                     = 0x008;
    // [? PREDICTED] Linux 0x00A  →  ARM64 0x00A  (u8)  10 bytes per entry
    constexpr uint32_t Stride                         = 0x00A;
} // namespace MarkerEntry

namespace MarkerNode {   // V=0 C=0 P=6
    // [? PREDICTED] Linux 0x010  →  ARM64 0x010  (u32)  u32 quest ID (BST key)
    constexpr uint32_t QuestId                        = 0x010;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x014  (u32)  u32 unknown (varies)
    constexpr uint32_t Field14                        = 0x014;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x018  (ptr)  ptr end of entry buffer
    constexpr uint32_t VecEnd                         = 0x018;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x020  (u32)  u32 buffer capacity in bytes
    constexpr uint32_t VecCap                         = 0x020;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x028  (ptr)  ptr start of entry buffer
    constexpr uint32_t VecBegin                       = 0x028;
    // [? PREDICTED] Linux 0x024  →  ARM64 0x030  (u32)  u32 number of 10-byte entries
    constexpr uint32_t VecCount                       = 0x030;
} // namespace MarkerNode

namespace EntityManagerEntry {   // V=0 C=0 P=6
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr entry vtable
    constexpr uint32_t VTable                         = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (u32)  u32 1=player, 3=NPC/mob
    constexpr uint32_t Type                           = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00C  (u32)  u32 entity ID (0xFFFFFFFF = sentinel)
    constexpr uint32_t Id                             = 0x00C;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x010  (ptr)  ptr pointer 1 (heap)
    constexpr uint32_t Ptr1                           = 0x010;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x018  (ptr)  ptr pointer 2 (heap)
    constexpr uint32_t Ptr2                           = 0x018;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x020  (u8)  bytes per entry
    constexpr uint32_t Size                           = 0x020;
} // namespace EntityManagerEntry

namespace Network {   // V=0 C=0 P=44
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  
    constexpr uint32_t ACTION_CLASS_SKILL             = 0x000;
    // [? PREDICTED] Linux 0x001  →  ARM64 0x004  (u32)  
    constexpr uint32_t ACTION_CLASS_ATTACK            = 0x004;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x008  (u32)  
    constexpr uint32_t ACTION_CLASS_INTERACT          = 0x008;
    // [? PREDICTED] Linux 0x003  →  ARM64 0x00C  (u32)  
    constexpr uint32_t ACTION_CLASS_DIALOGUE          = 0x00C;
    // [? PREDICTED] Linux 0x108  →  ARM64 0x114  (u32)  
    constexpr uint32_t OP_C2S_POST_LOGIN              = 0x114;
    // [? PREDICTED] Linux 0x1FB  →  ARM64 0x208  (u32)  
    constexpr uint32_t OP_S2C_LOGIN_RESP              = 0x208;
    // [? PREDICTED] Linux 0x204  →  ARM64 0x214  (u32)  
    constexpr uint32_t OP_C2S_MOVEMENT                = 0x214;
    // [? PREDICTED] Linux 0x304  →  ARM64 0x314  (u32)  
    constexpr uint32_t OP_C2S_HEARTBEAT               = 0x314;
    // [? PREDICTED] Linux 0x404  →  ARM64 0x414  (u32)  
    constexpr uint32_t OP_S2C_MOVE_CONFIRM            = 0x414;
    // [? PREDICTED] Linux 0x504  →  ARM64 0x514  (u32)  
    constexpr uint32_t OP_S2C_HEARTBEAT_ACK           = 0x514;
    // [? PREDICTED] Linux 0x50D  →  ARM64 0x520  (u32)  
    constexpr uint32_t OP_C2S_STATE_REQ               = 0x520;
    // [? PREDICTED] Linux 0x611  →  ARM64 0x624  (u32)  server sends item data for loot window
    constexpr uint32_t OP_S2C_LOOT_ITEM_INFO          = 0x624;
    // [? PREDICTED] Linux 0x807  →  ARM64 0x81C  (u32)  
    constexpr uint32_t OP_C2S_POS_REPORT              = 0x81C;
    // [? PREDICTED] Linux 0xC11  →  ARM64 0xC28  (u32)  universal: attack/skill/interact/dialogue
    constexpr uint32_t OP_C2S_ACTION                  = 0xC28;
    // [? PREDICTED] Linux 0xD04  →  ARM64 0xD1C  (u32)  
    constexpr uint32_t OP_S2C_SERVER_NOTIFY           = 0xD1C;
    // [? PREDICTED] Linux 0x1207  →  ARM64 0x1220  (u32)  
    constexpr uint32_t OP_C2S_WALK_TO_LOOT            = 0x1220;
    // [? PREDICTED] Linux 0x1309  →  ARM64 0x1324  (u32)  
    constexpr uint32_t OP_S2C_POS_UPDATE              = 0x1324;
    // [? PREDICTED] Linux 0x1402  →  ARM64 0x1420  (u32)  
    constexpr uint32_t OP_C2S_CLOSE_DIALOG            = 0x1420;
    // [? PREDICTED] Linux 0x1402  →  ARM64 0x1420  (u32)  4B: [opcode:2B][0000] (same as CLOSE_DIALOG)
    constexpr uint32_t OP_C2S_MARKET_CLOSE            = 0x1420;
    // [? PREDICTED] Linux 0x200A  →  ARM64 0x2028  (u32)  
    constexpr uint32_t OP_C2S_PICKUP_ITEM             = 0x2028;
    // [? PREDICTED] Linux 0x2011  →  ARM64 0x2030  (u32)  item added to inventory after pickup
    constexpr uint32_t OP_S2C_PICKUP_CONFIRM          = 0x2030;
    // [? PREDICTED] Linux 0x3AFF  →  ARM64 0x3B20  (u32)  
    constexpr uint32_t GameServerPort                 = 0x3B20;
    // [? PREDICTED] Linux 0x4119  →  ARM64 0x413C  (u32)  
    constexpr uint32_t OP_S2C_ENTITY_SPAWN            = 0x413C;
    // [? PREDICTED] Linux 0x411A  →  ARM64 0x4140  (u32)  
    constexpr uint32_t OP_S2C_ENTITY_SPAWN2           = 0x4140;
    // [? PREDICTED] Linux 0x4136  →  ARM64 0x415C  (u32)  
    constexpr uint32_t OP_S2C_CHAT_MSG                = 0x415C;
    // [? PREDICTED] Linux 0x413D  →  ARM64 0x4164  (u32)  
    constexpr uint32_t OP_S2C_CHAT_MSG2               = 0x4164;
    // [? PREDICTED] Linux 0x4903  →  ARM64 0x492C  (u32)  
    constexpr uint32_t OP_S2C_SKILL_ACK               = 0x492C;
    // [? PREDICTED] Linux 0x4A00  →  ARM64 0x4A2C  (u32)  2B: confirm market open
    constexpr uint32_t OP_C2S_MARKET_OPEN             = 0x4A2C;
    // [? PREDICTED] Linux 0x4B04  →  ARM64 0x4B30  (u32)  6B: [opcode:2B][cat_id:u32 LE]
    constexpr uint32_t OP_C2S_MARKET_BROWSE           = 0x4B30;
    // [? PREDICTED] Linux 0x4C04  →  ARM64 0x4C30  (u32)  6B: [opcode:2B][item_id:u32 LE]
    constexpr uint32_t OP_C2S_MARKET_SEARCH           = 0x4C30;
    // [? PREDICTED] Linux 0x5500  →  ARM64 0x552C  (u32)  
    constexpr uint32_t OP_S2C_ENTITY_DATA             = 0x552C;
    // [? PREDICTED] Linux 0x551B  →  ARM64 0x5548  (u32)  
    constexpr uint32_t OP_C2S_LOGIN                   = 0x5548;
    // [? PREDICTED] Linux 0x9802  →  ARM64 0x9830  (u32)  7B: [type:u8][slot:u32] — use item from bag
    constexpr uint32_t OP_C2S_USE_ITEM                = 0x9830;
    // [? PREDICTED] Linux 0x9B02  →  ARM64 0x9B30  (u32)  7B: [type:u8][param:u32] — secondary item action
    constexpr uint32_t OP_C2S_ITEM_ACTION             = 0x9B30;
    // [? PREDICTED] Linux 0xAD01  →  ARM64 0xAD30  (u32)  
    constexpr uint32_t OP_C2S_OPEN_LOOT               = 0xAD30;
    // [? PREDICTED] Linux 0xB201  →  ARM64 0xB230  (u32)  
    constexpr uint32_t OP_S2C_COMBAT_START            = 0xB230;
    // [? PREDICTED] Linux 0xB501  →  ARM64 0xB530  (u32)  
    constexpr uint32_t OP_S2C_LOOT_AVAILABLE          = 0xB530;
    // [? PREDICTED] Linux 0xC901  →  ARM64 0xC930  (u32)  
    constexpr uint32_t OP_C2S_ENTITY_QUERY            = 0xC930;
    // [? PREDICTED] Linux 0xCA03  →  ARM64 0xCA34  (u32)  
    constexpr uint32_t OP_S2C_QUEST_UPDATE            = 0xCA34;
    // [? PREDICTED] Linux 0xD604  →  ARM64 0xD638  (u32)  
    constexpr uint32_t OP_S2C_QUEST_ACCEPT            = 0xD638;
    // [? PREDICTED] Linux 0xD704  →  ARM64 0xD738  (u32)  
    constexpr uint32_t OP_S2C_QUEST_COMPLETE          = 0xD738;
    // [? PREDICTED] Linux 0xD801  →  ARM64 0xD838  (u32)  7B each: [opcode:2B][04][slot:u32] (batched)
    constexpr uint32_t OP_C2S_MARKET_DETAIL           = 0xD838;
    // [? PREDICTED] Linux 0xEC03  →  ARM64 0xEC3C  (u32)  
    constexpr uint32_t OP_S2C_ENTITY_UPDATE           = 0xEC3C;
    // [? PREDICTED] Linux 0xF801  →  ARM64 0xF83C  (u32)  
    constexpr uint32_t OP_C2S_HOTBAR_SYNC             = 0xF83C;
} // namespace Network

namespace VTables {   // V=0 C=0 P=8
    // [? PREDICTED] Linux 0xDC548C  →  ARM64 0xDC548C  (u32)  GSI / Game Manager singleton
    constexpr uint32_t GameManager                    = 0xDC548C;
    // [? PREDICTED] Linux 0xDC5B68  →  ARM64 0xDC5B68  (u32)  Entity Manager singleton
    constexpr uint32_t EntityManager                  = 0xDC5B68;
    // [? PREDICTED] Linux 0xDC81CC  →  ARM64 0xDC81CC  (u32)  Unknown manager
    constexpr uint32_t OtherManager                   = 0xDC81CC;
    // [? PREDICTED] Linux 0xDC8230  →  ARM64 0xDC8230  (u32)  Interactive objects (doors, portals, altars)
    constexpr uint32_t InteractiveObj                 = 0xDC8230;
    // [? PREDICTED] Linux 0xDC8D4C  →  ARM64 0xDC8D4C  (u32)  Container of players (type 3=SELF, type 1=other player)
    constexpr uint32_t Player                         = 0xDC8D4C;
    // [? PREDICTED] Linux 0xDC9130  →  ARM64 0xDC9130  (u32)  Container of friendly NPCs, NOT attackable (type 1)
    constexpr uint32_t FriendlyNPC                    = 0xDC9130;
    // [? PREDICTED] Linux 0xDC9270  →  ARM64 0xDC9270  (u32)  Dead mob corpse (in corpseTree, has loot)
    constexpr uint32_t Corpse                         = 0xDC9270;
    // [? PREDICTED] Linux 0xDC92B0  →  ARM64 0xDC92B0  (u32)  Container of attackable mobs (type 1)
    constexpr uint32_t Enemy                          = 0xDC92B0;
} // namespace VTables

namespace ELF {   // V=0 C=0 P=12
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  
    constexpr uint32_t HeadersStart                   = 0x000;
    // [? PREDICTED] Linux 0x6D000  →  ARM64 0x6D000  (u32)  
    constexpr uint32_t HeadersEnd                     = 0x6D000;
    // [? PREDICTED] Linux 0x6D000  →  ARM64 0x6D000  (u32)  r-xp ~12.4MB executable code
    constexpr uint32_t TextStart                      = 0x6D000;
    // [? PREDICTED] Linux 0xCCD000  →  ARM64 0xCCD000  (u32)  r--p ~836KB
    constexpr uint32_t RodataStart                    = 0xCCD000;
    // [? PREDICTED] Linux 0xCCD000  →  ARM64 0xCCD000  (u32)  
    constexpr uint32_t TextEnd                        = 0xCCD000;
    // [? PREDICTED] Linux 0xD9E000  →  ARM64 0xD9E000  (u32)  r--p ~256KB (relro)
    constexpr uint32_t DataRoStart                    = 0xD9E000;
    // [? PREDICTED] Linux 0xD9E000  →  ARM64 0xD9E000  (u32)  
    constexpr uint32_t RodataEnd                      = 0xD9E000;
    // [? PREDICTED] Linux 0xDDE000  →  ARM64 0xDDE000  (u32)  
    constexpr uint32_t DataRoEnd                      = 0xDDE000;
    // [? PREDICTED] Linux 0xDDE000  →  ARM64 0xDDE000  (u32)  rw-p ~8KB
    constexpr uint32_t DataRwStart                    = 0xDDE000;
    // [? PREDICTED] Linux 0xDE0000  →  ARM64 0xDE0000  (u32)  rw-p ~1.2MB (anonymous)
    constexpr uint32_t BssStart                       = 0xDE0000;
    // [? PREDICTED] Linux 0xDE0000  →  ARM64 0xDE0000  (u32)  
    constexpr uint32_t DataRwEnd                      = 0xDE0000;
    // [? PREDICTED] Linux 0xF05000  →  ARM64 0xF05000  (u32)  
    constexpr uint32_t BssEnd                         = 0xF05000;
} // namespace ELF

namespace MapFile {   // V=0 C=0 P=8
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  
    constexpr uint32_t TileBlocked                    = 0x000;
    // [? PREDICTED] Linux 0x001  →  ARM64 0x004  (u32)  
    constexpr uint32_t TileWalkable                   = 0x004;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x008  (u32)  
    constexpr uint32_t PassageMin                     = 0x008;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x008  (u16)  bytes per tile (u16)
    constexpr uint32_t TileBytes                      = 0x008;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x00C  (u16)  u16 version + u16 param
    constexpr uint32_t HeaderSize                     = 0x00C;
    // [? PREDICTED] Linux 0x00A  →  ARM64 0x014  (u32)  
    constexpr uint32_t PassageMax                     = 0x014;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x028  (u32)  tiles per chunk side
    constexpr uint32_t ChunkSize                      = 0x028;
    // [? PREDICTED] Linux 0x624  →  ARM64 0x630  (u32)  4 + 28*28*2
    constexpr uint32_t TotalSize                      = 0x630;
} // namespace MapFile

namespace CollisionGrid {   // V=0 C=0 P=9
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr back-reference to GM (AppState+0x14)
    constexpr uint32_t GMBackRef                      = 0x000;
    // [? PREDICTED] Linux 0x002  →  ARM64 0x008  (u16)  bytes per tile (u16)
    constexpr uint32_t TileBytes                      = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00E  (u16)  s16 area_id (GDB confirmed: matches .map filename)
    constexpr uint32_t AreaId                         = 0x00E;
    // [? PREDICTED] Linux 0x00A  →  ARM64 0x010  (u16)  s16 grid_y (GDB confirmed: matches .map filename)
    constexpr uint32_t GridY                          = 0x010;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x012  (u16)  s16 layer_id (GDB confirmed: matches .map filename)
    constexpr uint32_t LayerId                        = 0x012;
    // [? PREDICTED] Linux 0x00E  →  ARM64 0x014  (u16)  s16 map zone (differs from GM+0x1038 game zone!)
    constexpr uint32_t MapZone                        = 0x014;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x018  (u32)  start of 28×28 tile array (2B/tile)
    constexpr uint32_t TileData                       = 0x018;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x024  (u32)  tiles per side
    constexpr uint32_t ChunkSize                      = 0x024;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x040  (u8)  56 bytes per row (28 tiles × 2 bytes)
    constexpr uint32_t RowStride                      = 0x040;
} // namespace CollisionGrid

namespace PropTable {   // V=0 C=0 P=2
    // [? PREDICTED] Linux 0x1144  →  ARM64 0x1148  (ptr)  BST sentinel: property_id → max_value
    constexpr uint32_t PropertyCaps                   = 0x1148;
    // [? PREDICTED] Linux 0x1250  →  ARM64 0x1258  (u32)  Start of property BST region
    constexpr uint32_t Region                         = 0x1258;
} // namespace PropTable

namespace GhidraFunctions {   // V=0 C=0 P=91
    // [? PREDICTED] Linux 0x2F3E20  →  ARM64 0x2F3E20  (u32)  FUN_00304E20 — GameManager init
    constexpr uint32_t GMConstructor                  = 0x2F3E20;
    // [? PREDICTED] Linux 0x2FB760  →  ARM64 0x2FB760  (u32)  FUN_0030C760 — main packet loop
    constexpr uint32_t PacketDispatcher               = 0x2FB760;
    // [? PREDICTED] Linux 0x2FC3F0  →  ARM64 0x2FC3F0  (ptr)  FUN_0030D3F0(gm, entity_key) → Container*
    constexpr uint32_t FindEntityByKey                = 0x2FC3F0;
    // [? PREDICTED] Linux 0x2FC600  →  ARM64 0x2FC608  (ptr)  FUN_0030D600(gm, obj_key) → IO*
    constexpr uint32_t FindObjectByKey                = 0x2FC608;
    // [? PREDICTED] Linux 0x304BE0  →  ARM64 0x304BEC  (u32)  FUN_00305BE0 — handler table setup
    constexpr uint32_t HandlerRegister                = 0x304BEC;
    // [? PREDICTED] Linux 0x321750  →  ARM64 0x32175C  (u32)  FUN_00322750
    constexpr uint32_t AppearanceUpdate               = 0x32175C;
    // [? PREDICTED] Linux 0x321A40  →  ARM64 0x321A4C  (u32)  FUN_00322A40
    constexpr uint32_t EquipmentUpdate                = 0x321A4C;
    // [? PREDICTED] Linux 0x321D30  →  ARM64 0x321D3C  (u32)  FUN_00322D30
    constexpr uint32_t EntityUpdate                   = 0x321D3C;
    // [? PREDICTED] Linux 0x322630  →  ARM64 0x32263C  (u32)  FUN_00323630
    constexpr uint32_t EntitySpawn                    = 0x32263C;
    // [? PREDICTED] Linux 0x3420A0  →  ARM64 0x3420AC  (u32)  FUN_003520A0 — AddHP(target, -dmg)
    constexpr uint32_t CombatDamage                   = 0x3420AC;
    // [? PREDICTED] Linux 0x342240  →  ARM64 0x34224C  (u32)  FUN_00352240 — crit damage
    constexpr uint32_t CombatDamageCrit               = 0x34224C;
    // [? PREDICTED] Linux 0x3423C0  →  ARM64 0x3423CC  (u32)  FUN_003523C0 — AddHP(target, +heal)
    constexpr uint32_t CombatHeal                     = 0x3423CC;
    // [? PREDICTED] Linux 0x342780  →  ARM64 0x34278C  (u32)  FUN_00352780 — apply buff to entity
    constexpr uint32_t CombatApplyBuff                = 0x34278C;
    // [? PREDICTED] Linux 0x342E90  →  ARM64 0x342E9C  (u32)  FUN_00352E90 — HP+MP regen tick
    constexpr uint32_t CombatHPMPDelta                = 0x342E9C;
    // [? PREDICTED] Linux 0x3825B0  →  ARM64 0x3825BC  (u32)  FUN_003925b0(ChatMgr, deserialized_msg) — parse+dispatch rec
    constexpr uint32_t ChatRecvParse                  = 0x3825BC;
    // [? PREDICTED] Linux 0x3828A0  →  ARM64 0x3828AC  (u32)  FUN_003928a0(ChatMgr, ChannelObj) — dispatch to UI channels
    constexpr uint32_t ChatDispatch                   = 0x3828AC;
    // [? PREDICTED] Linux 0x3864B0  →  ARM64 0x3864BC  (u32)  FUN_003964b0(ChatMgr, packet, channel) — receive handler + s
    constexpr uint32_t ChatBuildSend                  = 0x3864BC;
    // [? PREDICTED] Linux 0x38C810  →  ARM64 0x38C81C  (u32)  FUN_0039c810(out_wstr, msg_obj) — extract full text from mes
    constexpr uint32_t ChatGetText                    = 0x38C81C;
    // [? PREDICTED] Linux 0x38F630  →  ARM64 0x38F63C  (u32)  FUN_0048f630(MenuChat, text) — send area chat (via UI widget
    constexpr uint32_t ChatSendArea                   = 0x38F63C;
    // [? PREDICTED] Linux 0x40CEB0  →  ARM64 0x40CEBC  (u32)  FUN_0041CEB0(sceneMgr, callback, sceneId) — SceneSet+ViewMod
    constexpr uint32_t SceneTransition                = 0x40CEBC;
    // [? PREDICTED] Linux 0x444450  →  ARM64 0x44445C  (u32)  FUN_00454450(uiCtx, modifier) — item action dispatcher (0=dr
    constexpr uint32_t ItemActionDispatch             = 0x44445C;
    // [? PREDICTED] Linux 0x4942C0  →  ARM64 0x4942CC  (u32)  FUN_004A42C0(viewCtrl) — caching + ViewMode + connect(sessio
    constexpr uint32_t ConnectToGameWorld             = 0x4942CC;
    // [? PREDICTED] Linux 0x495390  →  ARM64 0x49539C  (u32)  FUN_004A5390(viewCtrl) — AVANÇAR/NEXT button handler (cleanu
    constexpr uint32_t ViewCtrlAdvance                = 0x49539C;
    // [? PREDICTED] Linux 0x4BE830  →  ARM64 0x4BE83C  (u32)  FUN_004CE830(widget, email, password) — hash creds, store, c
    constexpr uint32_t LoginWithCredentials           = 0x4BE83C;
    // [? PREDICTED] Linux 0x4EB500  →  ARM64 0x4EB50C  (u32)  FUN_004FB500(viewCtrl, sceneId, callback, ctx) — write scene
    constexpr uint32_t ViewTransition                 = 0x4EB50C;
    // [? PREDICTED] Linux 0x4EC690  →  ARM64 0x4EC69C  (u32)  FUN_004FC690(viewCtrl) — close popup + reset scene state
    constexpr uint32_t ViewCtrlReset                  = 0x4EC69C;
    // [? PREDICTED] Linux 0x54B8C0  →  ARM64 0x54B8CC  (u32)  FUN_0055B8C0(widget) — login button handler
    constexpr uint32_t JogarHandler                   = 0x54B8CC;
    // [? PREDICTED] Linux 0x5CCA50  →  ARM64 0x5CCA5C  (u32)  FUN_005DCA50(viewCtrl) — read charData from viewCtrl+0x497C/
    constexpr uint32_t SelectCharConnect              = 0x5CCA5C;
    // [? PREDICTED] Linux 0x722F40  →  ARM64 0x722F4C  (u32)  FUN_00732f40(qpm)
    constexpr uint32_t QuestDialogOnButton            = 0x722F4C;
    // [? PREDICTED] Linux 0x7D4BD0  →  ARM64 0x7D4BDC  (u32)  FUN_007E5BD0 — process exit by type
    constexpr uint32_t ExitTableProcess               = 0x7D4BDC;
    // [? PREDICTED] Linux 0x7D7C70  →  ARM64 0x7D7C7C  (u32)  FUN_007E8C70 — full zone change
    constexpr uint32_t ZoneTransition                 = 0x7D7C7C;
    // [? PREDICTED] Linux 0x7D8B10  →  ARM64 0x7D8B1C  (u32)  FUN_007E9B10 — load collision from file
    constexpr uint32_t LoadZoneCollision              = 0x7D8B1C;
    // [? PREDICTED] Linux 0x7D9D60  →  ARM64 0x7D9D70  (ptr)  FUN_007EAD60(grid, pos) → bool
    constexpr uint32_t IsWalkable                     = 0x7D9D70;
    // [? PREDICTED] Linux 0x7DA3A0  →  ARM64 0x7DA3B4  (u32)  FUN_007EB3A0(grid, start, end, result, flag)
    constexpr uint32_t PathfindEntry                  = 0x7DA3B4;
    // [? PREDICTED] Linux 0x7DAF10  →  ARM64 0x7DAF24  (u32)  FUN_007EBF10(grid, start, end, cost_grid)
    constexpr uint32_t PathfindAStar                  = 0x7DAF24;
    // [? PREDICTED] Linux 0x845CA0  →  ARM64 0x845CB8  (ptr)  FUN_00856CA0(mgr, 0x47) → bool
    constexpr uint32_t HasResItem                     = 0x845CB8;
    // [? PREDICTED] Linux 0x846B40  →  ARM64 0x846B5C  (u32)  FUN_00857B40(mgr, 0x47)
    constexpr uint32_t UseResItem                     = 0x846B5C;
    // [? PREDICTED] Linux 0x862790  →  ARM64 0x8627AC  (u32)  FUN_00872790(itemsMgr, item_id, slot_index) — full use-item 
    constexpr uint32_t ItemsMgrUseItem                = 0x8627AC;
    // [? PREDICTED] Linux 0x863FF0  →  ARM64 0x86400C  (u32)  FUN_00873ff0(itemsMgr, type, subtype) — find slot by item ty
    constexpr uint32_t FindSlotByType                 = 0x86400C;
    // [? PREDICTED] Linux 0x8667E0  →  ARM64 0x8667FC  (u32)  FUN_008767E0(itemsMgr, flag) — open loot confirmation panel
    constexpr uint32_t LootPanelOpen                  = 0x8667FC;
    // [? PREDICTED] Linux 0x880E70  →  ARM64 0x880E8C  (u32)  FUN_00891E70
    constexpr uint32_t MarketSearch                   = 0x880E8C;
    // [? PREDICTED] Linux 0x880FE0  →  ARM64 0x880FFC  (u32)  FUN_00891FE0
    constexpr uint32_t MarketBuy                      = 0x880FFC;
    // [? PREDICTED] Linux 0x8810A0  →  ARM64 0x8810BC  (u32)  FUN_008920A0
    constexpr uint32_t MarketSell                     = 0x8810BC;
    // [? PREDICTED] Linux 0x8811A0  →  ARM64 0x8811BC  (u32)  FUN_008921A0
    constexpr uint32_t MarketCancel                   = 0x8811BC;
    // [? PREDICTED] Linux 0x886DE0  →  ARM64 0x886DFC  (u32)  build+send attack packet (type=1 attack, type=2 walk_to_loot
    constexpr uint32_t AttackPacketBuild              = 0x886DFC;
    // [? PREDICTED] Linux 0x886ED0  →  ARM64 0x886EEC  (u32)  build+send skill packet
    constexpr uint32_t ActionPacketBuild              = 0x886EEC;
    // [? PREDICTED] Linux 0x886FF0  →  ARM64 0x88700C  (u32)  build+send interact packet (alive entities, type<4)
    constexpr uint32_t InteractPacketBuild            = 0x88700C;
    // [? PREDICTED] Linux 0x8870C0  →  ARM64 0x8870DC  (u32)  build+send corpse/object interact packet (type>=4, hardcoded
    constexpr uint32_t CorpseInteractBuild            = 0x8870DC;
    // [? PREDICTED] Linux 0x887190  →  ARM64 0x8871AC  (u32)  FUN_00897190(packetQueue, item_id, from_slot, to_slot) — mov
    constexpr uint32_t BuildMoveItemPkt               = 0x8871AC;
    // [? PREDICTED] Linux 0x887200  →  ARM64 0x88721C  (u32)  FUN_00897200(packetQueue, item_id, slot_idx, targetKey) — bu
    constexpr uint32_t BuildUseItemPacket             = 0x88721C;
    // [? PREDICTED] Linux 0x887320  →  ARM64 0x88733C  (u32)  FUN_00897320(packetQueue, slot_idx, item_id) — delete/discar
    constexpr uint32_t BuildDeleteItemPkt             = 0x88733C;
    // [? PREDICTED] Linux 0x887C30  →  ARM64 0x887C4C  (u16)  FUN_00897c30: case 0x2716 loot/object packet (action=0x2E, o
    constexpr uint32_t ObjectInteractBuild            = 0x887C4C;
    // [? PREDICTED] Linux 0x887D10  →  ARM64 0x887D2C  (u32)  FUN_00897D10(packetMgr, slotIdx, count) — pickup item from l
    constexpr uint32_t LootPickupSlot                 = 0x887D2C;
    // [? PREDICTED] Linux 0x887D70  →  ARM64 0x887D8C  (u16)  FUN_00897D70(packetMgr, u16 cmd) — simple quest/dialog packe
    constexpr uint32_t QuestPacketSimple              = 0x887D8C;
    // [? PREDICTED] Linux 0x895DB0  →  ARM64 0x895DCC  (u32)  FUN_008A5DB0(sessionMgr, mode) — start auth connection
    constexpr uint32_t ConnectWrapperConnect          = 0x895DCC;
    // [? PREDICTED] Linux 0x8B0220  →  ARM64 0x8B023C  (u32)  FUN_008C1220 — Container+0x130
    constexpr uint32_t SetAllegiance                  = 0x8B023C;
    // [? PREDICTED] Linux 0x8B02A0  →  ARM64 0x8B02BC  (u32)  FUN_008C12A0 — Container+0x134
    constexpr uint32_t SetHP                          = 0x8B02BC;
    // [? PREDICTED] Linux 0x8B02D0  →  ARM64 0x8B02EC  (u32)  FUN_008C12D0 — Container+0x138
    constexpr uint32_t SetHPMax                       = 0x8B02EC;
    // [? PREDICTED] Linux 0x8B0450  →  ARM64 0x8B046C  (u32)  FUN_008C1450 — Container+0x13C
    constexpr uint32_t SetMP                          = 0x8B046C;
    // [? PREDICTED] Linux 0x8B0480  →  ARM64 0x8B049C  (u32)  FUN_008C1480 — Container+0x140
    constexpr uint32_t SetMPMax                       = 0x8B049C;
    // [? PREDICTED] Linux 0x8B1C60  →  ARM64 0x8B1C7C  (u32)  FUN_008C1C60(container, dest, flag) — client-only
    constexpr uint32_t LocalMoveTo                    = 0x8B1C7C;
    // [? PREDICTED] Linux 0x8B1F30  →  ARM64 0x8B1F4C  (u32)  FUN_008C2F30 — Container+0x300
    constexpr uint32_t SetFaction                     = 0x8B1F4C;
    // [? PREDICTED] Linux 0x8B2300  →  ARM64 0x8B231C  (u32)  FUN_008C3300 — Container+0x310 (0=none, 1="!", 2="?")
    constexpr uint32_t SetQuestIndicator              = 0x8B231C;
    // [? PREDICTED] Linux 0x8B2D40  →  ARM64 0x8B2D5C  (u32)  set Container+0x2B8
    constexpr uint32_t SetTarget                      = 0x8B2D5C;
    // [? PREDICTED] Linux 0x8B2E30  →  ARM64 0x8B2E4C  (u32)  clear Container+0x2B8/+0x2BC
    constexpr uint32_t ClearTarget                    = 0x8B2E4C;
    // [? PREDICTED] Linux 0x8B5BC0  →  ARM64 0x8B5BDC  (u32)  returns entity+8, updates tile hash
    constexpr uint32_t GetEntityTile                  = 0x8B5BDC;
    // [? PREDICTED] Linux 0x8B70D0  →  ARM64 0x8B70EC  (u32)  click handler dispatcher
    constexpr uint32_t ClickDispatcher                = 0x8B70EC;
    // [? PREDICTED] Linux 0x8B72F0  →  ARM64 0x8B730C  (u32)  FUN_008C72F0(container) — intra-area move
    constexpr uint32_t GroundClick                    = 0x8B730C;
    // [? PREDICTED] Linux 0x8B73D0  →  ARM64 0x8B73EC  (u32)  FUN_008C73D0(container) — border transition
    constexpr uint32_t BorderClick                    = 0x8B73EC;
    // [? PREDICTED] Linux 0x8B7590  →  ARM64 0x8B75AC  (u32)  FUN_008C7590(container) — go down one layer
    constexpr uint32_t LayerDown                      = 0x8B75AC;
    // [? PREDICTED] Linux 0x8B7700  →  ARM64 0x8B771C  (u32)  FUN_008C7700(container) — go up one layer
    constexpr uint32_t LayerUp                        = 0x8B771C;
    // [? PREDICTED] Linux 0x8B7870  →  ARM64 0x8B788C  (u32)  click-to-attack (walk+attack+state)
    constexpr uint32_t ClickAttack                    = 0x8B788C;
    // [? PREDICTED] Linux 0x8B7950  →  ARM64 0x8B796C  (u32)  use current skill on entity
    constexpr uint32_t SelectTarget                   = 0x8B796C;
    // [? PREDICTED] Linux 0x8B7BE0  →  ARM64 0x8B7BFC  (u32)  interact/loot (walk+packet)
    constexpr uint32_t InteractTarget                 = 0x8B7BFC;
    // [? PREDICTED] Linux 0x8B7D20  →  ARM64 0x8B7D3C  (u32)  set +0x144 based on state
    constexpr uint32_t SetActionState                 = 0x8B7D3C;
    // [? PREDICTED] Linux 0x8BCBF0  →  ARM64 0x8BCC10  (ptr)  FUN_008CCBF0(timer*, seconds) → timer[0]=sec*1000
    constexpr uint32_t CDStart                        = 0x8BCC10;
    // [? PREDICTED] Linux 0x8C7D10  →  ARM64 0x8C7D34  (u32)  FUN_008D8D10 — Container+0x43D
    constexpr uint32_t SetClassId                     = 0x8C7D34;
    // [? PREDICTED] Linux 0x8DB980  →  ARM64 0x8DB9A4  (u32)  FUN_008EC980
    constexpr uint32_t OnLootReceived                 = 0x8DB9A4;
    // [? PREDICTED] Linux 0x8DC000  →  ARM64 0x8DC024  (u32)  FUN_008ED000
    constexpr uint32_t OnEntityDead                   = 0x8DC024;
    // [? PREDICTED] Linux 0x8E7FD0  →  ARM64 0x8E7FF4  (u32)  FUN_008F8FD0
    constexpr uint32_t QuestStageUpdate               = 0x8E7FF4;
    // [? PREDICTED] Linux 0x8EB910  →  ARM64 0x8EB934  (u32)  FUN_008fb910(qpm, cmd)
    constexpr uint32_t QuestDialogConfirm             = 0x8EB934;
    // [? PREDICTED] Linux 0x8FBFC0  →  ARM64 0x8FBFE8  (ptr)  FUN_0090BFC0(skillsMgr, skillId) → u16 deciseconds
    constexpr uint32_t GetSkillCooldown               = 0x8FBFE8;
    // [? PREDICTED] Linux 0x8FC060  →  ARM64 0x8FC090  (ptr)  FUN_0090C060(SM, skillId) → node+0x14 ptr (BST lookup)
    constexpr uint32_t SkillTreeLookup                = 0x8FC090;
    // [? PREDICTED] Linux 0x8FC190  →  ARM64 0x8FC1C8  (ptr)  FUN_0090C190(SM, skillId) → range (multiplier*raw/100)
    constexpr uint32_t GetSkillRange                  = 0x8FC1C8;
    // [? PREDICTED] Linux 0x8FCD80  →  ARM64 0x8FCDBC  (u32)  set skill + auto-repeat
    constexpr uint32_t SetSkillAndAttack              = 0x8FCDBC;
    // [? PREDICTED] Linux 0x8FCE70  →  ARM64 0x8FCEAC  (u32)  auto-repeat dispatch
    constexpr uint32_t SkillAutoRepeat                = 0x8FCEAC;
    // [? PREDICTED] Linux 0x8FCFD0  →  ARM64 0x8FD010  (ptr)  FUN_0090CFD0(SM) → u16 skill_type from DataTable
    constexpr uint32_t GetActiveSkillType             = 0x8FD010;
    // [? PREDICTED] Linux 0x8FD630  →  ARM64 0x8FD674  (u32)  skill on entity (internal)
    constexpr uint32_t UseSkillOnEntity               = 0x8FD674;
    // [? PREDICTED] Linux 0x8FD890  →  ARM64 0x8FD8D8  (ptr)  FUN_0090D890(SM) → self-cast active skill (__regparm1)
    constexpr uint32_t UseSkillOnSelf                 = 0x8FD8D8;
    // [? PREDICTED] Linux 0x8FD980  →  ARM64 0x8FD9CC  (u32)  skill on object (internal)
    constexpr uint32_t UseSkillOnObject               = 0x8FD9CC;
    // [? PREDICTED] Linux 0xACE520  →  ARM64 0xACE570  (ptr)  FUN_00ADE520(pakMgr, filename, outBuf) → char (0=fail). Hook
    constexpr uint32_t PakFileRead                    = 0xACE570;
} // namespace GhidraFunctions

namespace PropertyTree {   // V=0 C=0 P=6
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x00C  (u32)  offset of marker within node
    constexpr uint32_t MarkerOff                      = 0x00C;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x020  (u8)  bytes per tree node
    constexpr uint32_t NodeSize                       = 0x020;
    // [? PREDICTED] Linux 0x021  →  ARM64 0x024  (u32)  common marker value (also 0x31, 0x111)
    constexpr uint32_t Marker                         = 0x024;
    // [? PREDICTED] Linux 0xDA2574  →  ARM64 0xDA2578  (u32)  atk_magic, xp_max, percentages
    constexpr uint32_t VT_Combat                      = 0xDA2578;
    // [? PREDICTED] Linux 0xDA258C  →  ARM64 0xDA2590  (u32)  oxygen, def_phys, dodge, etc.
    constexpr uint32_t VT_Display                     = 0xDA2590;
    // [? PREDICTED] Linux 0xDA25D4  →  ARM64 0xDA25D8  (u32)  hp_regen, mp_regen, small stats
    constexpr uint32_t VT_Regen                       = 0xDA25D8;
} // namespace PropertyTree

namespace StatCache {   // V=0 C=0 P=11
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (u32)  u32 flat
    constexpr uint32_t AtkPhysical                    = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u32)  u32 flat
    constexpr uint32_t AtkMagical                     = 0x004;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x008  (u32)  u32 flat
    constexpr uint32_t DefMagicalFlat                 = 0x008;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x00C  (u32)  u32 flat
    constexpr uint32_t DefPhysicalFlat                = 0x00C;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x01C  (u32)  u32 ×100 (616 = 6.16%)
    constexpr uint32_t CritChance                     = 0x01C;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x020  (u32)  u32 ×100
    constexpr uint32_t Dodge                          = 0x020;
    // [? PREDICTED] Linux 0x028  →  ARM64 0x028  (u32)  u32 ×100
    constexpr uint32_t Resilience                     = 0x028;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x030  (u32)  u32 ×100
    constexpr uint32_t Penetration                    = 0x030;
    // [? PREDICTED] Linux 0x040  →  ARM64 0x040  (u32)  u32 ×100
    constexpr uint32_t Solidity                       = 0x040;
    // [? PREDICTED] Linux 0x0A0  →  ARM64 0x0A0  (u32)  u32 flat per 5s
    constexpr uint32_t HpRegen                        = 0x0A0;
    // [? PREDICTED] Linux 0x0A4  →  ARM64 0x0A4  (u32)  u32 flat per 5s
    constexpr uint32_t MpRegen                        = 0x0A4;
} // namespace StatCache

namespace SkillDef {   // V=0 C=0 P=16
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr DataTable container pointer
    constexpr uint32_t ParentPtr                      = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (u32)  u32 skill ID (confirmed: matches tree key)
    constexpr uint32_t SkillId                        = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x00C  (u32)  u32 string table ID for name
    constexpr uint32_t NameStringId                   = 0x00C;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x010  (u32)  u32 string table ID for description
    constexpr uint32_t DescStringId                   = 0x010;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x014  (u16)  u16 icon/anim data + u16 flags
    constexpr uint32_t IconData                       = 0x014;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x018  (u16)  u16 1=self, 2=conditional, 3=target, 6=conditional, 7=target
    constexpr uint32_t SkillType                      = 0x018;
    // [? PREDICTED] Linux 0x016  →  ARM64 0x01A  (u16)  u16 max_targets or sub-type
    constexpr uint32_t SubField                       = 0x01A;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x020  (ptr)  ptr → f32[] primary effect values (damage/heal per level)
    constexpr uint32_t EffectParams1                  = 0x020;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x028  (u32)  u32 byte count of effect_params1 array
    constexpr uint32_t EffectParams1Sz                = 0x028;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x030  (ptr)  ptr → u16[] cooldowns per level (in seconds)
    constexpr uint32_t CooldownArray                  = 0x030;
    // [? PREDICTED] Linux 0x024  →  ARM64 0x038  (u32)  u32 byte count (÷2 = num levels)
    constexpr uint32_t CooldownArraySz                = 0x038;
    // [? PREDICTED] Linux 0x028  →  ARM64 0x040  (ptr)  ptr → f32[] secondary effect values
    constexpr uint32_t EffectParams2                  = 0x040;
    // [? PREDICTED] Linux 0x02C  →  ARM64 0x048  (u32)  u32 byte count
    constexpr uint32_t EffectParams2Sz                = 0x048;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x050  (ptr)  ptr → u16[] ranges per level (0 = self/melee)
    constexpr uint32_t RangeArray                     = 0x050;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x058  (u32)  u32 byte count (÷2 = num levels)
    constexpr uint32_t RangeArraySz                   = 0x058;
    // [? PREDICTED] Linux 0x038  →  ARM64 0x05C  (u32)  u32 base/default cooldown value
    constexpr uint32_t BaseCooldown                   = 0x05C;
} // namespace SkillDef

namespace SkillNode {   // V=0 C=0 P=13
    // [? PREDICTED] Linux 0x000  →  ARM64 0x000  (ptr)  ptr
    constexpr uint32_t Parent                         = 0x000;
    // [? PREDICTED] Linux 0x004  →  ARM64 0x008  (ptr)  ptr
    constexpr uint32_t Left                           = 0x008;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x010  (ptr)  ptr
    constexpr uint32_t Right                          = 0x010;
    // [? PREDICTED] Linux 0x00C  →  ARM64 0x018  (u32)  u32 0=black, 1=red
    constexpr uint32_t Color                          = 0x018;
    // [? PREDICTED] Linux 0x010  →  ARM64 0x01C  (u32)  u32 skill ID (tree key)
    constexpr uint32_t SkillId                        = 0x01C;
    // [? PREDICTED] Linux 0x014  →  ARM64 0x020  (ptr)  ptr → SkillDef in DataTable
    constexpr uint32_t SkillDefPtr                    = 0x020;
    // [? PREDICTED] Linux 0x018  →  ARM64 0x028  (u8)  u8 current learned level
    constexpr uint32_t SkillLevel                     = 0x028;
    // [? PREDICTED] Linux 0x01C  →  ARM64 0x02C  (u32)  u32 remaining cooldown (ms, 0=ready, decrements per frame)
    constexpr uint32_t RemainingCdMs                  = 0x02C;
    // [? PREDICTED] Linux 0x020  →  ARM64 0x030  (u32)  u32 base cooldown duration (ms, static from DataTable)
    constexpr uint32_t BaseCdMs                       = 0x030;
    // [? PREDICTED] Linux 0x02E  →  ARM64 0x03E  (u8)  u8 if set, use override cooldown
    constexpr uint32_t OverrideFlag                   = 0x03E;
    // [? PREDICTED] Linux 0x030  →  ARM64 0x040  (u16)  u16 cooldown override (ms)
    constexpr uint32_t CdOverride                     = 0x040;
    // [? PREDICTED] Linux 0x032  →  ARM64 0x042  (u8)  u8 if set, use override range
    constexpr uint32_t RangeOverrideF                 = 0x042;
    // [? PREDICTED] Linux 0x034  →  ARM64 0x044  (u16)  u16 range override
    constexpr uint32_t RangeOverride                  = 0x044;
} // namespace SkillNode

namespace ItemDef {   // V=0 C=0 P=4
    // [? PREDICTED] Linux 0x004  →  ARM64 0x004  (u32)  u32 item ID (confirmed: HotkeysPanel reads *(def+4))
    constexpr uint32_t ItemId                         = 0x004;
    // [? PREDICTED] Linux 0x008  →  ARM64 0x008  (u8)  u8 item type (0xC=HP potion, 0xD=MP potion, etc.)
    constexpr uint32_t ItemType                       = 0x008;
    // [? PREDICTED] Linux 0x009  →  ARM64 0x009  (u8)  u8 sub-type (for type 0x26 branching)
    constexpr uint32_t ItemSubType                    = 0x009;
    // [? PREDICTED] Linux 0x028  →  ARM64 0x028  (u32)  u32 weapon attack speed (raw units, /578 ≈ seconds)
    constexpr uint32_t WeaponSpeedRaw                 = 0x028;
} // namespace ItemDef

namespace WorldManager {   // V=0 C=0 P=10
    // [? PREDICTED] Linux 0x024  →  ARM64 0x024  (u16)  s16 current zone_id
    constexpr uint32_t ZoneId                         = 0x024;
    // [? PREDICTED] Linux 0x026  →  ARM64 0x026  (u16)  s16 current layer
    constexpr uint32_t LayerId                        = 0x026;
    // [? PREDICTED] Linux 0x74C  →  ARM64 0x750  (ptr)  arr primary exit table (8B entries: ptr+callback)
    constexpr uint32_t ExitTableMain                  = 0x750;
    // [? PREDICTED] Linux 0x12AC  →  ARM64 0x12B4  (u32)  arr attack exit table
    constexpr uint32_t ExitTableAttack                = 0x12B4;
    // [? PREDICTED] Linux 0x2374  →  ARM64 0x237C  (u32)  arr select exit table
    constexpr uint32_t ExitTableSelect                = 0x237C;
    // [? PREDICTED] Linux 0x26DC  →  ARM64 0x26E4  (u32)  arr border exit table
    constexpr uint32_t ExitTableBorder                = 0x26E4;
    // [? PREDICTED] Linux 0x2A44  →  ARM64 0x2A4C  (u32)  arr layer transition table
    constexpr uint32_t ExitTableLayer                 = 0x2A4C;
    // [? PREDICTED] Linux 0x48D8  →  ARM64 0x48E0  (u8)  u8 exit is pending
    constexpr uint32_t ExitPending                    = 0x48E0;
    // [? PREDICTED] Linux 0x48DC  →  ARM64 0x48E4  (u32)  u32 action code (10000-10005)
    constexpr uint32_t ExitType                       = 0x48E4;
    // [? PREDICTED] Linux 0x48E0  →  ARM64 0x48E8  (u32)  u32 exit parameter
    constexpr uint32_t ExitParam                      = 0x48E8;
} // namespace WorldManager

} // namespace OffsetsAndroid
