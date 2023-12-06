// Simple citro2d untextured shape example
#include <citro2d.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define BOTTOM_WIDTH 320
#define BOTTOM_HEIGHT 240

C2D_TextBuf g_staticBuf;
C2D_Text g_staticText;
C2D_Font font;

bool buttonPressed = false;

//---------------------------------------------------------------------------------
void drawShapes()
{
  //---------------------------------------------------------------------------------
  // Init libs
  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();
  consoleInit(GFX_TOP, NULL);

  // Create screens
  C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

  g_staticBuf = C2D_TextBufNew(4096);
  font = C2D_FontLoadSystem(CFG_REGION_USA);
  C2D_TextFontParse(&g_staticText, font, g_staticBuf, "Toggle lights");
  C2D_TextOptimize(&g_staticText);

  // Create colors
  // #862971 #c01c9a
  u32 clrRec1 = C2D_Color32(0x86, 0x29, 0x71, 0xFF);
  u32 clrRec2 = C2D_Color32(0xc0, 0x1c, 0x9a, 0xFF);
  u32 white = C2D_Color32(0xff, 0xff, 0xff, 0xFF);

  u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);

  // Main loop
  while (aptMainLoop())
  {
    hidScanInput();

    // Respond to user input
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
      break; // break in order to return to hbmenu
    printf("\x1b[1;1HSimple citro2d shapes example");
    printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
    printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
    printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);

    touchPosition touch;
    // Read the touch screen coordinates
    hidTouchRead(&touch);
    // Print the touch screen coordinates
    printf("\x1b[2;0H%03d; %03d", touch.px, touch.py);

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, clrClear);
    C2D_SceneBegin(top);

    u32 color = clrRec1;
    float fontScale = 0.8f;
    int fontSize = 25 * fontScale;
    if (
        touch.px > BOTTOM_WIDTH / 2 - 70 &&
        touch.px < BOTTOM_WIDTH / 2 + 70 &&
        touch.py > BOTTOM_HEIGHT / 2 - 20 &&
        touch.py < BOTTOM_HEIGHT / 2 + 20)
    {
      color = clrRec2;
      buttonPressed = true;
    }

    if (touch.px == 0 && touch.py == 0 && buttonPressed)
    {
      buttonPressed = false;
      toggleLights();
    }

    C2D_DrawRectangle(
        BOTTOM_WIDTH / 2 - 70,
        BOTTOM_HEIGHT / 2 - 20,
        0,
        140,
        40,
        color,
        color,
        color,
        color);
    C2D_DrawText(
        &g_staticText,
        C2D_AlignCenter | C2D_WithColor,
        BOTTOM_WIDTH / 2,
        BOTTOM_HEIGHT / 2 - fontSize / 2,
        0.0f,
        fontScale,
        fontScale,
        white);

    C3D_FrameEnd(0);
  }

  // Deinit libs
  C2D_Fini();
  C3D_Fini();
  gfxExit();
}