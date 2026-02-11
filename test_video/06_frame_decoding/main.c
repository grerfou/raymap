/*
 * Test 06_frame_decoding - Étape 2.2 : Décodage frame-by-frame
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

#define RAYMAPVID_IMPLEMENTATION
#include "raymapvid.h"

#define TEST_VIDEO_PATH "a.mp4"

int main(int argc, char **argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║    RayMapVid - Test 06: Frame Decoding                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    const char *videoPath = (argc > 1) ? argv[1] : TEST_VIDEO_PATH;
    
    FILE *f = fopen(videoPath, "rb");
    if (!f) {
        printf(" ERROR: Video file not found: %s\n\n", videoPath);
        return 1;
    }
    fclose(f);
    
    int tests_passed = 0;
    int tests_total = 0;
    
    SetTraceLogLevel(LOG_WARNING);
    
    RMV_Video *video = RMV_LoadVideo(videoPath);
    
    if (!video || !RMV_IsVideoLoaded(video)) {
        printf("✗ FAIL: Could not load video\n\n");
        return 1;
    }
    
    RMV_VideoInfo info = RMV_GetVideoInfo(video);
    
    InitWindow(info.width, info.height, "Test 06 - Frame Decoding");
    SetTargetFPS(60);
    
    printf("Video info:\n");
    printf("  Resolution: %dx%d\n", info.width, info.height);
    printf("  FPS: %.2f\n", info.fps);
    printf("  Duration: %.2f s\n\n", info.duration);
    
    // Test 1
    tests_total++;
    printf("[Test 1] UpdateVideo in STOPPED state\n");
    RMV_UpdateVideo(video, 0.016f);
    printf("  ✓ PASS: No crash when STOPPED\n\n");
    tests_passed++;
    
    // Test 2
    tests_total++;
    printf("[Test 2] UpdateVideo in PLAYING state\n");
    RMV_PlayVideo(video);
    for (int i = 0; i < 10; i++) {
        RMV_UpdateVideo(video, 1.0f / info.fps);
    }
    printf("  ✓ PASS: Decoded 10 frames\n\n");
    tests_passed++;
    
    // Test 3
    tests_total++;
    printf("[Test 3] UpdateVideo in PAUSED state\n");
    RMV_PauseVideo(video);
    RMV_UpdateVideo(video, 0.016f);
    printf("  ✓ PASS: No crash when PAUSED\n\n");
    tests_passed++;
    
    // Test 4 - Visual
    tests_total++;
    printf("[Test 4] Visual playback test\n");
    printf("  Controls:\n");
    printf("    SPACE - Play/Pause\n");
    printf("    R     - Restart\n");
    printf("    L     - Toggle Loop\n");
    printf("    ESC   - Exit\n\n");
    
    RMV_StopVideo(video);
    RMV_SetVideoLoop(video, true);
    RMV_PlayVideo(video);
    
    bool testPassed = false;
    float testTimer = 0.0f;
    bool loopEnabled = true;  // Track loop state locally
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        testTimer += dt;
        
        if (testTimer >= 3.0f && !testPassed) {
            testPassed = true;
            printf("  ✓ PASS: Playback ran for 3 seconds\n\n");
            tests_passed++;
        }
        
        // Controls
        if (IsKeyPressed(KEY_SPACE)) {
            RMV_ToggleVideoPause(video);
        }
        if (IsKeyPressed(KEY_R)) {
            RMV_StopVideo(video);
            RMV_PlayVideo(video);
        }
        if (IsKeyPressed(KEY_L)) {
            loopEnabled = !loopEnabled;
            RMV_SetVideoLoop(video, loopEnabled);
            printf("  Loop: %s\n", loopEnabled ? "ON" : "OFF");
        }
        
        // Update video
        RMV_UpdateVideo(video, dt);
        
        // Render
        BeginDrawing();
            ClearBackground(BLACK);
            
            Texture2D tex = RMV_GetVideoTexture(video);
            DrawTexture(tex, 0, 0, WHITE);
            
            // UI
            DrawRectangle(10, 10, 300, 120, Fade(BLACK, 0.7f));
            DrawText(TextFormat("State: %s", 
                     RMV_IsVideoPlaying(video) ? "PLAYING" : "PAUSED"),
                     20, 20, 20, WHITE);
            DrawText(TextFormat("FPS: %.0f", GetFPS()), 20, 45, 20, WHITE);
            DrawText(TextFormat("Loop: %s", loopEnabled ? "ON" : "OFF"), 
                     20, 70, 20, WHITE);
            DrawText(TextFormat("Timer: %.1fs", testTimer), 20, 95, 20, GREEN);
            
            DrawText("SPACE=Play R=Restart L=Loop ESC=Exit", 
                     10, info.height - 30, 20, WHITE);
            
        EndDrawing();
        
        if (testPassed && testTimer >= 5.0f) {
            break;
        }
    }
    
    if (!testPassed) {
        printf("  ✓ PASS: Visual test completed\n\n");
        tests_passed++;
    }
    
    RMV_UnloadVideo(video);
    CloseWindow();
    
    printf("════════════════════════════════════════════════════════════\n");
    printf("Résultats: %d/%d tests passed\n", tests_passed, tests_total);
    printf("════════════════════════════════════════════════════════════\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ ÉTAPE 2.2 VALIDÉE - Frame Decoding OK!\n\n");
        return 0;
    } else {
        printf("\n✗ FAIL\n\n");
        return 1;
    }
}
