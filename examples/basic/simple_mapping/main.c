/**********************************************************************************************
*
*   RayMap Example 01 - Simple Mapping
*
*   DESCRIPTION:
*       The simplest possible RayMap example. Creates a single warped surface with
*       static content. Perfect starting point for beginners.
*
*   FEATURES:
*       - Single surface with bilinear mapping
*       - Static quad (no calibration)
*       - Simple visual content
*
*   CONTROLS:
*       ESC - Exit
*
*   COMPILATION:
*       make
*
**********************************************************************************************/

#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

int main(void)
{
    // Initialize window
    InitWindow(1280, 720, "RayMap Example 01 - Simple Mapping");
    SetTargetFPS(60);
    
    // Create a warped surface (800x600 with bilinear mapping)
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface");
        CloseWindow();
        return -1;
    }
    
    // Define a simple trapezoid quad for warping
    RM_Quad quad = {
        {150, 100},      // Top-left
        {1000, 80},      // Top-right
        {1100, 620},     // Bottom-right
        {50, 640}        // Bottom-left
    };
    RM_SetQuad(surface, quad);
    
    // Main loop
    while (!WindowShouldClose())
    {
        //----------------------------------------------------------------------------------
        // Draw content to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            // Background
            ClearBackground(RAYWHITE);
            
            // Simple shapes
            DrawRectangle(50, 50, 700, 500, SKYBLUE);
            DrawCircle(400, 300, 150, RED);
            DrawRectangle(300, 200, 200, 200, GOLD);
            
            // Text
            DrawText("RAYMAP", 250, 250, 80, WHITE);
            DrawText("Simple Mapping Example", 180, 350, 40, DARKGRAY);
            
        RM_EndSurface(surface);
        
        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // Instructions
            DrawText("Example 01 - Simple Mapping", 10, 10, 20, WHITE);
            DrawText("ESC to exit", 10, 690, 20, LIGHTGRAY);
            
        EndDrawing();
    }
    
    // Cleanup
    RM_DestroySurface(surface);
    CloseWindow();
    
    return 0;
}
