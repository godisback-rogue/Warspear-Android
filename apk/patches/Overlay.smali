.class public Lcom/wsAndroid/Overlay;
.super Ljava/lang/Object;
.source "Overlay.java"


# direct methods
.method public constructor <init>()V
    .locals 0

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method

# Native per-frame draw — implemented in libinjector as
# Java_com_wsAndroid_Overlay_draw. Called by MDRenderer.onDrawFrame after
# the game has finished rendering into the EGL surface.
.method public static native draw()V
.end method

# Native touch handler — implemented in libinjector as
# Java_com_wsAndroid_Overlay_onTouch. Called by MDSurfaceView.onTouchEvent
# BEFORE the game processes the touch. Returns true if ImGui consumed the
# event (touch fell on our window), false to let the game receive it.
.method public static native onTouch(Landroid/view/MotionEvent;)Z
.end method
