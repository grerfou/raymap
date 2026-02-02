/**********************************************************************************************
*
*   RayMap Example 02 - Interactive Calibration
*
*   DESCRIPTION:
*       Learn how to use RayMap's interactive calibration system.
*       Drag corners with mouse, save/load configurations, reset quad.
*
*   FEATURES:
*       - Interactive corner dragging
*       - Save/load calibration to file
*       - Reset to default position
*       - Visual calibration overlay
*
*   CONTROLS:
*       TAB     - Toggle calibration mode on/off
*       S       - Save calibration to file
*       L       - Load calibration from file
*       R       - Reset quad to centered rectangle
*       MOUSE   - Drag corners in calibration mode
*       ESC     - Exit
*
*   COMPILATION:
*       make
*
**********************************************************************************************/

#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void)
{
    // Initialize window
    InitWindow(1280, 720, "RayMap Example 02 - Interactive Calibration");
    SetTargetFPS(60);
    
    // Create surface
    RM_Surface *surface = RM_CreateSurface(640, 480, RM_MAP_BILINEAR);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface");
        CloseWindow();
        return -1;
    }
    
    // Initialize calibration with default settings
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    // Start with calibration disabled
    calib.enabled = false;
    
    // Animation time
    float time = 0.0f;
    
    // Main loop
    while (!WindowShouldClose())
    {
        time = GetTime();
        
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        
        // Toggle calibration mode with TAB
        RM_UpdateCalibrationInput(&calib, KEY_TAB);
        
        // Save configuration
        if (IsKeyPressed(KEY_S)) {
            if (RM_SaveConfig(surface, "calibration.txt")) {
                printf("✓ Configuration saved to calibration.txt\n");
            } else {
                printf("✗ Failed to save configuration\n");
            }
        }
        
        // Load configuration
        if (IsKeyPressed(KEY_L)) {
            if (RM_LoadConfig(surface, "calibration.txt")) {
                printf("✓ Configuration loaded from calibration.txt\n");
            } else {
                printf("✗ Failed to load configuration (file may not exist)\n");
            }
        }
        
        // Reset quad
        if (IsKeyPressed(KEY_R)) {
            RM_ResetCalibrationQuad(&calib, 1280, 720);
            printf("✓ Quad reset to centered rectangle\n");
        }
        
        //----------------------------------------------------------------------------------
        // Draw content to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            
            // Animated gradient background
            for (int y = 0; y < 480; y++) {
                float t = (float)y / 480.0f;
                Color top = SKYBLUE;
                Color bottom = DARKBLUE;
                Color c = ColorLerp(top, bottom, t);
                DrawRectangle(0, y, 640, 1, c);
            }
            
            // Animated circle
            float radius = 80.0f + sinf(time * 2.0f) * 20.0f;
            DrawCircle(320, 240, radius, YELLOW);
            DrawCircleLines(320, 240, radius + 10, GOLD);
            
            // Title
            DrawText("CALIBRATION", 150, 180, 60, WHITE);
            DrawText("DEMO", 230, 250, 60, WHITE);
            
            // Instructions at bottom
            const char *hint = calib.enabled ? "Drag corners!" : "Press TAB";
            DrawText(hint, 240, 420, 25, LIGHTGRAY);
            
        RM_EndSurface(surface);
        
        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(BLACK);
            
            // Draw warped surface
            RM_DrawSurface(surface);
            
            // Draw calibration overlay (if enabled)
            if (calib.enabled) {
                RM_DrawCalibration(calib);
            }
            
            // UI Panel
            DrawRectangle(0, 0, 350, 200, Fade(BLACK, 0.7f));
            DrawText("Example 02 - Calibration", 10, 10, 20, WHITE);
            DrawText(TextFormat("Mode: %s", calib.enabled ? "CALIBRATION" : "NORMAL"), 
                     10, 40, 20, calib.enabled ? GREEN : LIGHTGRAY);
            
            DrawText("Controls:", 10, 75, 18, YELLOW);
            DrawText("  TAB - Toggle calibration", 10, 95, 16, LIGHTGRAY);
            DrawText("  S   - Save config", 10, 115, 16, LIGHTGRAY);
            DrawText("  L   - Load config", 10, 135, 16, LIGHTGRAY);
            DrawText("  R   - Reset quad", 10, 155, 16, LIGHTGRAY);
            DrawText("  ESC - Exit", 10, 175, 16, LIGHTGRAY);
            
            // FPS
            DrawText(TextFormat("FPS: %d", GetFPS()), 1200, 10, 20, LIME);
            
        EndDrawing();
    }
    
    // Cleanup
    RM_DestroySurface(surface);
    CloseWindow();
    
    return 0;
}
