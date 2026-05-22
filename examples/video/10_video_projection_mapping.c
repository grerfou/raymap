/*******************************************************************************************
*
*   raymapvid - 10_video_projection_mapping
*
*   DESCRIPTION:
*       Video mapped onto a warped surface with live calibration.
*       This is the main use case for RayMapVid in a real installation.
*       Calibration is saved to a file and reloaded automatically on startup.
*
*   CONTROLS:
*       SPACE - Play / Pause
*       C     - Toggle calibration overlay
*       R     - Reset quad to default position
*       S     - Save calibration to file
*       L     - Load calibration from file
*       MOUSE - Drag corners (when calibration is active)
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

int main(void)
{
    const int screenWidth  = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "RayMapVid - 10 Video Projection Mapping");
    SetTargetFPS(60);

    // A simple path works too: "video.mp4" or "/absolute/path/video.mp4"
    // GetApplicationDirectory() is used here so the example works regardless
    // of the working directory when the executable is launched.
    // Build path to resources folder relative to the executable
    // Executable is in build/examples/video/, resources are in examples/video/resources/
    const char *videoPath = TextFormat("%s../../../examples/video/resources/video.mp4", GetApplicationDirectory());

    const char *configFile = "video_mapping.cfg";

    // Load video and create a matching surface
    RMV_Video    *video        = RMV_LoadVideo(videoPath);
    RMV_VideoInfo info         = RMV_GetVideoInfo(video);
    RM_Surface   *surface      = RM_CreateSurface(info.width, info.height, RM_MAP_HOMOGRAPHY);
    RM_Calibration calibration = RM_CalibrationDefault(surface);
    calibration.enabled        = false;

    // Load saved calibration if it exists, otherwise center the surface
    if (!RM_LoadConfig(surface, configFile))
    {
        RM_ResetQuad(surface, screenWidth, screenHeight);
    }

    RMV_SetVideoLoop(video, true);
    RMV_PlayVideo(video);

    while (!WindowShouldClose())
    {
        // Video and calibration controls
        if (IsKeyPressed(KEY_SPACE)) { RMV_ToggleVideoPause(video); }
        if (IsKeyPressed(KEY_C))     { RM_ToggleCalibration(&calibration); }
        if (IsKeyPressed(KEY_R))     { RM_ResetCalibrationQuad(&calibration, screenWidth, screenHeight); }
        if (IsKeyPressed(KEY_S))     { RM_SaveConfig(surface, configFile); }
        if (IsKeyPressed(KEY_L))     { RM_LoadConfig(surface, configFile); }

        // Handle corner dragging and decode the next frame
        RM_UpdateCalibration(&calibration);
        RMV_UpdateVideo(video, GetFrameTime());

        // Draw the current video frame onto the surface
        RM_BeginSurface(surface);
            ClearBackground(BLACK);
            DrawTexture(RMV_GetVideoTexture(video), 0, 0, WHITE);
        RM_EndSurface(surface);

        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(surface);
            // Draw corner handles when calibration is active
            RM_DrawCalibration(calibration);
            DrawText("[SPACE] Play/Pause  [C] Calibrate  [R] Reset  [S] Save  [L] Load", 10, 10, 18, Fade(WHITE, 0.5f));
            DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    RMV_UnloadVideo(video);
    RM_DestroySurface(surface);
    CloseWindow();
    return 0;
}
