#include "raylib.h"
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

int main(void) {
    InitWindow(1280, 720, "RayMap - Simple Calibration");
    
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    while (!WindowShouldClose()) {
        // ✨ UNE SEULE LIGNE !
        RM_UpdateCalibrationInput(&calib, KEY_TAB);
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw content
            RM_BeginSurface(surface);
                ClearBackground(BLUE);
                DrawText("Hello RayMap!", 50, 50, 60, WHITE);
            RM_EndSurface(surface);
            
            // Draw warped surface
            RM_DrawSurface(surface);
            
            // Draw calibration overlay
            RM_DrawCalibration(calib);
            
            // Info
            if (calib.enabled) {
                DrawText("Calibration ON - Press TAB to exit", 10, 10, 20, YELLOW);
            } else {
                DrawText("Press TAB for calibration", 10, 10, 20, WHITE);
            }
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    return 0;
}
