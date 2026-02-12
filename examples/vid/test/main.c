/*
 * RayMap + RayMapVid Integration - Projection Mapping Video
 * 
 * Demonstrates video projection mapping with interactive calibration
 */

#include <stdio.h>
#include <stdlib.h>

// RayMap + RayMapVid (auto-implements both)
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#include "raymapvid.h"

// ============================================================================
// CONFIGURATION
// ============================================================================
#define VIDEO_PATH "./a.mp4"
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main(int argc, char **argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║     RayMap + RayMapVid - Video Projection Mapping         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    const char *videoPath = (argc > 1) ? argv[1] : VIDEO_PATH;
    
    // Init window FIRST (required for textures)
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayMap Video Mapping");
    SetTargetFPS(60);
    
    // Load video
    printf("Loading video: %s\n", videoPath);
    RMV_Video *video = RMV_LoadVideo(videoPath);
    
    if (!video) {
        printf("ERROR: Failed to load video\n");
        printf("Check VIDEO_PATH in source code\n\n");
        CloseWindow();
        return 1;
    }
    
    RMV_VideoInfo info = RMV_GetVideoInfo(video);
    printf("Video loaded:\n");
    printf("  Resolution: %dx%d\n", info.width, info.height);
    printf("  Duration: %.2f s\n", info.duration);
    printf("  FPS: %.2f\n", info.fps);
    printf("  Codec: %s\n\n", info.codec);
    
    // Create RayMap surface (matches video resolution)
    RM_Surface *surface = RM_CreateSurface(info.width, info.height, RM_MAP_BILINEAR);
    if (!surface) {
        printf("ERROR: Failed to create RayMap surface\n");
        RMV_UnloadVideo(video);
        CloseWindow();
        return 1;
    }
    
    // Reset quad to centered position
    RM_ResetQuad(surface, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Setup calibration
    RM_Calibration calibration = RM_CalibrationDefault(surface);
    calibration.enabled = true; // Start in calibration mode
    
    // Start video playback
    RMV_SetVideoLoop(video, true);
    RMV_PlayVideo(video);
    
    printf("Controls:\n");
    printf("  C         - Toggle calibration mode\n");
    printf("  SPACE     - Play/Pause video\n");
    printf("  R         - Restart video\n");
    printf("  L         - Toggle loop\n");
    printf("  M         - Cycle mapping mode (Bilinear/Homography)\n");
    printf("  S         - Save calibration\n");
    printf("  O         - Load calibration\n");
    printf("  BACKSPACE - Reset quad\n");
    printf("  ESC       - Exit\n\n");
    
    bool loopEnabled = true;
    
    // Main loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // === INPUT ===
        
        // Calibration controls
        if (IsKeyPressed(KEY_C)) {
            RM_ToggleCalibration(&calibration);
            printf("Calibration: %s\n", calibration.enabled ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            RM_ResetQuad(surface, SCREEN_WIDTH, SCREEN_HEIGHT);
            printf("Quad reset\n");
        }
        
        // Mapping mode
        if (IsKeyPressed(KEY_M)) {
            RM_MapMode mode = RM_GetMapMode(surface);
            mode = (mode == RM_MAP_BILINEAR) ? RM_MAP_HOMOGRAPHY : RM_MAP_BILINEAR;
            RM_SetMapMode(surface, mode);
            printf("Mapping mode: %s\n", mode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY");
        }
        
        // Video controls
        if (IsKeyPressed(KEY_SPACE)) {
            RMV_ToggleVideoPause(video);
        }
        
        if (IsKeyPressed(KEY_R)) {
            RMV_StopVideo(video);
            RMV_PlayVideo(video);
            printf("Video restarted\n");
        }
        
        if (IsKeyPressed(KEY_L)) {
            loopEnabled = !loopEnabled;
            RMV_SetVideoLoop(video, loopEnabled);
            printf("Loop: %s\n", loopEnabled ? "ON" : "OFF");
        }
        
        // Save/Load config
        if (IsKeyPressed(KEY_S)) {
            if (RM_SaveConfig(surface, "video_mapping.cfg")) {
                printf("Configuration saved\n");
            }
        }
        
        if (IsKeyPressed(KEY_O)) {
            if (RM_LoadConfig(surface, "video_mapping.cfg")) {
                printf("Configuration loaded\n");
            }
        }
        
        // Update calibration
        RM_UpdateCalibration(&calibration);
        
        // Update video
        RMV_UpdateVideo(video, dt);
        
        // === RENDERING ===
        
        // 1. Draw video TO the RayMap surface
        RM_BeginSurface(surface);
            ClearBackground(BLACK);
            
            // Get video texture and draw it
            Texture2D videoTex = RMV_GetVideoTexture(video);
            DrawTexture(videoTex, 0, 0, WHITE);
            
        RM_EndSurface(surface);
        
        // 2. Draw the warped surface to screen
        BeginDrawing();
            ClearBackground(DARKGRAY);
            
            // Draw the mapped video
            RM_DrawSurface(surface);
            
            // Draw calibration overlay
            if (calibration.enabled) {
                RM_DrawCalibration(calibration);
            }
            
            // === UI ===
            DrawRectangle(10, 10, 400, 160, Fade(BLACK, 0.7f));
            
            DrawText("RayMap Video Mapping", 20, 20, 20, YELLOW);
            
            DrawText(TextFormat("Video: %s", 
                     RMV_IsVideoPlaying(video) ? "PLAYING" : "PAUSED"),
                     20, 45, 16, GREEN);
            
            DrawText(TextFormat("Mode: %s",
                     RM_GetMapMode(surface) == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY"),
                     20, 65, 16, WHITE);
            
            DrawText(TextFormat("Calibration: %s",
                     calibration.enabled ? "ON" : "OFF"),
                     20, 85, 16, calibration.enabled ? GREEN : GRAY);
            
            DrawText(TextFormat("Loop: %s", loopEnabled ? "ON" : "OFF"),
                     20, 105, 16, WHITE);
            
            DrawText(TextFormat("FPS: %.0f", GetFPS()),
                     20, 125, 16, GetFPS() >= 30 ? GREEN : RED);
            
            if (calibration.enabled) {
                DrawText("Drag corners to adjust mapping", 20, 145, 14, YELLOW);
            }
            
            // Help text
            DrawText("C=Calib M=Mode SPACE=Pause R=Restart S=Save O=Load", 
                     10, SCREEN_HEIGHT - 30, 16, WHITE);
            
        EndDrawing();
    }
    
    // Cleanup
    RM_DestroySurface(surface);
    RMV_UnloadVideo(video);
    CloseWindow();
    
    printf("\n✓ RayMap Video Mapping test completed!\n");
    return 0;
}
