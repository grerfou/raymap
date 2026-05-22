/*******************************************************************************************
*
*   raymapvid - 12_multi_video_zones
*
*   DESCRIPTION:
*       Two independent video zones arranged side by side.
*       Each zone has its own surface, video stream and calibration config.
*       Select a zone with 1-2, then use the usual controls on it.
*
*   CONTROLS:
*       1-2   - Select active zone
*       SPACE - Play/Pause selected zone
*       A     - Play/Pause all zones simultaneously
*       C     - Toggle calibration for selected zone
*       R     - Reset selected zone to default quad
*       S     - Save selected zone calibration
*       L     - Load selected zone calibration
*       MOUSE - Drag corners (when calibration is active)
*       ESC   - Exit
*
*   LICENSE: zlib/libpng
*   Copyright (c) 2025 grerfou
*
********************************************************************************************/

#include <stdio.h>
#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#include "raymapvid.h"

#define ZONE_COUNT 2

int main(void)
{
    const int screenWidth  = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "RayMapVid - 12 Multi Video Zones (2-zone)");
    SetTargetFPS(60);

    // Build paths to resources folder relative to the executable
    // Executable is in build/examples/video/, resources are in examples/video/resources/
    char videoPaths[ZONE_COUNT][512];
    for (int i = 0; i < ZONE_COUNT; i++)
    {
        snprintf(videoPaths[i], sizeof(videoPaths[i]),
                 "%s../../../examples/video/resources/zone%d.mp4", GetApplicationDirectory(), i);
    }

    const char *configFiles[ZONE_COUNT] = { "zone0.cfg", "zone1.cfg" };

    // Default quads: 2 zones side by side with 10px padding
    RM_Quad defaultQuads[ZONE_COUNT] = {
        { { 10,  10 }, { 950,  10 }, { 950,  1070 }, { 10,  1070 } },   // left
        { { 970, 10 }, { 1910, 10 }, { 1910, 1070 }, { 970, 1070 } }    // right
    };

    RM_Surface    *surfaces[ZONE_COUNT];
    RMV_Video     *videos[ZONE_COUNT];
    RM_Calibration calibrations[ZONE_COUNT];

    // Initialize each zone
    for (int i = 0; i < ZONE_COUNT; i++)
    {
        videos[i] = RMV_LoadVideo(videoPaths[i]);

        RMV_VideoInfo info = RMV_GetVideoInfo(videos[i]);
        surfaces[i] = RM_CreateSurface(info.width, info.height, RM_MAP_HOMOGRAPHY);

        // Load saved calibration or fall back to the default grid position
        if (!RM_LoadConfig(surfaces[i], configFiles[i]))
        {
            RM_SetQuad(surfaces[i], defaultQuads[i]);
        }

        calibrations[i]         = RM_CalibrationDefault(surfaces[i]);
        calibrations[i].enabled = false;

        RMV_SetVideoLoop(videos[i], true);
        RMV_PlayVideo(videos[i]);
    }

    int selected = 0;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Zone selection
        if (IsKeyPressed(KEY_ONE)) { selected = 0; }
        if (IsKeyPressed(KEY_TWO)) { selected = 1; }

        // Controls for the selected zone
        if (IsKeyPressed(KEY_SPACE)) { RMV_ToggleVideoPause(videos[selected]); }
        if (IsKeyPressed(KEY_C))     { RM_ToggleCalibration(&calibrations[selected]); }
        if (IsKeyPressed(KEY_R))     { RM_SetQuad(surfaces[selected], defaultQuads[selected]); }
        if (IsKeyPressed(KEY_S))     { RM_SaveConfig(surfaces[selected], configFiles[selected]); }
        if (IsKeyPressed(KEY_L))     { RM_LoadConfig(surfaces[selected], configFiles[selected]); }

        // Play/Pause all zones simultaneously, using zone 0 as the reference state
        if (IsKeyPressed(KEY_A))
        {
            if (RMV_IsVideoPlaying(videos[0]))
            {
                for (int i = 0; i < ZONE_COUNT; i++) { RMV_PauseVideo(videos[i]); }
            }
            else
            {
                for (int i = 0; i < ZONE_COUNT; i++) { RMV_PlayVideo(videos[i]); }
            }
        }

        // Handle corner dragging for the selected zone
        RM_UpdateCalibration(&calibrations[selected]);

        // Decode and render each zone to its surface
        for (int i = 0; i < ZONE_COUNT; i++)
        {
            RMV_UpdateVideo(videos[i], dt);

            RM_BeginSurface(surfaces[i]);
                ClearBackground(BLACK);
                DrawTexture(RMV_GetVideoTexture(videos[i]), 0, 0, WHITE);
            RM_EndSurface(surfaces[i]);
        }

        BeginDrawing();
            ClearBackground(BLACK);

            // Draw all surfaces to screen
            for (int i = 0; i < ZONE_COUNT; i++)
            {
                RM_DrawSurface(surfaces[i]);
            }

            // Draw calibration overlay for the selected zone only
            RM_DrawCalibration(calibrations[selected]);

            // Highlight the selected zone with an orange border
            RM_Quad q = RM_GetQuad(surfaces[selected]);
            DrawLineEx(q.topLeft,     q.topRight,    2, ORANGE);
            DrawLineEx(q.topRight,    q.bottomRight, 2, ORANGE);
            DrawLineEx(q.bottomRight, q.bottomLeft,  2, ORANGE);
            DrawLineEx(q.bottomLeft,  q.topLeft,     2, ORANGE);

            DrawText(TextFormat("[1-2] Zone (active: %d)  [A] All  [SPACE] Play/Pause  [C] Calib  [S] Save  [L] Load", selected + 1),
                     10, 10, 18, LIGHTGRAY);
            DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < ZONE_COUNT; i++)
    {
        RMV_UnloadVideo(videos[i]);
        RM_DestroySurface(surfaces[i]);
    }

    CloseWindow();
    return 0;
}
