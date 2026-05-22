/*******************************************************************************************
*
*   raymapvid - 09_basic_video_playback
*
*   DESCRIPTION:
*       Load a video file and display it on a RayMap surface.
*       This is the simplest possible RayMapVid example.
*
*   CONTROLS:
*       SPACE - Play / Pause
*       S     - Stop
*       L     - Toggle loop
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

    InitWindow(screenWidth, screenHeight, "RayMapVid - 09 Basic Video Playback");
    SetTargetFPS(60);

    // A simple path works too: "video.mp4" or "/absolute/path/video.mp4"
    // GetApplicationDirectory() is used here so the example works regardless
    // of the working directory when the executable is launched.
    // Build path to resources folder relative to the executable
    // Executable is in build/examples/video/, resources are in examples/video/resources/
    const char *videoPath = TextFormat("%s../../../examples/video/resources/video.mp4", GetApplicationDirectory());

    // Load the video and create a surface matching its dimensions
    RMV_Video    *video   = RMV_LoadVideo(videoPath);
    RMV_VideoInfo info    = RMV_GetVideoInfo(video);
    RM_Surface   *surface = RM_CreateSurface(info.width, info.height, RM_MAP_BILINEAR);
    bool          loop    = true;

    // Center the surface on screen and start playback
    RM_ResetQuad(surface, screenWidth, screenHeight);
    RMV_SetVideoLoop(video, loop);
    RMV_PlayVideo(video);

    while (!WindowShouldClose())
    {
        // Playback controls
        if (IsKeyPressed(KEY_SPACE)) { RMV_ToggleVideoPause(video); }
        if (IsKeyPressed(KEY_S))     { RMV_StopVideo(video); }

        if (IsKeyPressed(KEY_L))
        {
            loop = !loop;
            RMV_SetVideoLoop(video, loop);
        }

        // Decode the next frame
        RMV_UpdateVideo(video, GetFrameTime());

        // Draw the current video frame onto the surface
        RM_BeginSurface(surface);
            ClearBackground(BLACK);
            DrawTexture(RMV_GetVideoTexture(video), 0, 0, WHITE);
        RM_EndSurface(surface);

        // Draw the warped surface to screen
        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(surface);
            DrawText("[SPACE] Play/Pause  [S] Stop  [L] Loop", 10, 10, 20, LIGHTGRAY);
            DrawFPS(screenWidth - 100, 10);
        EndDrawing();
    }

    RMV_UnloadVideo(video);
    RM_DestroySurface(surface);
    CloseWindow();
    return 0;
}
