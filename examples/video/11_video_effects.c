/*******************************************************************************************
*
*   raymapvid - 12_video_effects
*
*   DESCRIPTION:
*       Color tint effects applied to a projected video.
*       Uses raylib's DrawTexture color modulation - no shader required.
*
*   CONTROLS:
*       0-8   - Select effect
*       UP    - Increase brightness
*       DOWN  - Decrease brightness
*       SPACE - Play / Pause
*       ESC   - Exit
*
*   LICENSE: zlib/libpng
*   Copyright (c) 2025 grerfou
*
********************************************************************************************/

#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#include "raymapvid.h"

typedef struct { const char *name; Color tint; } Effect;

static const Effect EFFECTS[] = {
    { "Normal",    { 255, 255, 255, 255 } },
    { "Red",       { 255, 100, 100, 255 } },
    { "Green",     { 100, 255, 100, 255 } },
    { "Blue",      { 100, 100, 255, 255 } },
    { "Grayscale", { 180, 180, 180, 255 } },
    { "Sepia",     { 240, 200, 140, 255 } },
    { "Dark",      {  80,  80,  80, 255 } },
    { "Warm",      { 255, 220, 160, 255 } },
    { "Cold",      { 160, 210, 255, 255 } },
};
#define NUM_EFFECTS 9

int main(void)
{
    const int screenWidth  = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "RayMapVid - 12 Video Effects");
    SetTargetFPS(60);

    // A simple path works too: "video.mp4" or "/absolute/path/video.mp4"
    // GetApplicationDirectory() is used here so the example works regardless
    // of the working directory when the executable is launched.
    // Build path to resources folder relative to the executable
    // Executable is in build/examples/video/, resources are in examples/video/resources/
    const char *videoPath = TextFormat("%s../../../examples/video/resources/video.mp4", GetApplicationDirectory());

    RMV_Video    *video   = RMV_LoadVideo(videoPath);
    RMV_VideoInfo info    = RMV_GetVideoInfo(video);
    RM_Surface   *surface = RM_CreateSurface(info.width, info.height, RM_MAP_BILINEAR);

    RM_ResetQuad(surface, screenWidth, screenHeight);
    RMV_SetVideoLoop(video, true);
    RMV_PlayVideo(video);

    int   activeEffect = 0;
    float brightness   = 1.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) { RMV_ToggleVideoPause(video); }

        // Effect selection
        if (IsKeyPressed(KEY_ZERO))  { activeEffect = 0; }
        if (IsKeyPressed(KEY_ONE))   { activeEffect = 1; }
        if (IsKeyPressed(KEY_TWO))   { activeEffect = 2; }
        if (IsKeyPressed(KEY_THREE)) { activeEffect = 3; }
        if (IsKeyPressed(KEY_FOUR))  { activeEffect = 4; }
        if (IsKeyPressed(KEY_FIVE))  { activeEffect = 5; }
        if (IsKeyPressed(KEY_SIX))   { activeEffect = 6; }
        if (IsKeyPressed(KEY_SEVEN)) { activeEffect = 7; }
        if (IsKeyPressed(KEY_EIGHT)) { activeEffect = 8; }

        // Brightness adjustment
        if (IsKeyDown(KEY_UP))   { brightness = Clamp(brightness + dt * 0.5f, 0.1f, 2.0f); }
        if (IsKeyDown(KEY_DOWN)) { brightness = Clamp(brightness - dt * 0.5f, 0.1f, 2.0f); }

        // Decode the next frame
        RMV_UpdateVideo(video, dt);

        // Multiply the effect tint by the brightness factor
        Color base = EFFECTS[activeEffect].tint;
        Color tint = {
            (unsigned char)Clamp(base.r * brightness, 0, 255),
            (unsigned char)Clamp(base.g * brightness, 0, 255),
            (unsigned char)Clamp(base.b * brightness, 0, 255),
            255
        };

        // Draw the tinted frame onto the surface
        RM_BeginSurface(surface);
            ClearBackground(BLACK);
            DrawTexture(RMV_GetVideoTexture(video), 0, 0, tint);
        RM_EndSurface(surface);

        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(surface);

            // Effect list - highlight the active one
            for (int i = 0; i < NUM_EFFECTS; i++)
            {
                Color labelColor = LIGHTGRAY;
                if (i == activeEffect) { labelColor = ORANGE; }
                DrawText(TextFormat("[%d] %s", i, EFFECTS[i].name), 10, 10 + i * 22, 18, labelColor);
            }

            DrawText(TextFormat("Brightness: %.2f  [UP/DOWN]", brightness), 10, 220, 18, LIGHTGRAY);
            DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    RMV_UnloadVideo(video);
    RM_DestroySurface(surface);
    CloseWindow();
    return 0;
}
