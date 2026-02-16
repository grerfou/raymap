/*******************************************************************************************
*
*   raymap - 02_basic_warping
*
*   DESCRIPTION:
*       Demonstrates basic surface warping with a predefined quad.
*       Shows the difference between BILINEAR and HOMOGRAPHY mapping modes.
*       The surface is warped into a trapezoid shape to simulate perspective.
*
*   DEPENDENCIES:
*       raylib 5.0+
*       raymap 1.1.0+
*
*   COMPILATION (Linux):
*       gcc 02_basic_warping.c -o 02_basic_warping -lraylib -lm
*
*   COMPILATION (macOS):
*       clang 02_basic_warping.c -o 02_basic_warping -lraylib -framework CoreVideo \
*             -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
*
*   COMPILATION (Windows - MinGW):
*       gcc 02_basic_warping.c -o 02_basic_warping.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*
*   CONTROLS:
*       SPACE   - Toggle between BILINEAR and HOMOGRAPHY modes
*       ESC     - Exit
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2026 grerfou
*
********************************************************************************************/

#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "RayMap - 02 Basic Warping");
    SetTargetFPS(60);

    // Create surface
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface!");
        CloseWindow();
        return -1;
    }

    // Define a trapezoid quad to simulate perspective projection
    // This creates a "looking at wall from angle" effect
    RM_Quad perspectiveQuad = {
        .topLeft = { 200, 100 },        // Top-left corner
        .topRight = { 1720, 100 },      // Top-right corner
        .bottomRight = { 1600, 980 },   // Bottom-right (pulled in)
        .bottomLeft = { 320, 980 }      // Bottom-left (pulled in)
    };
    
    // Apply the warped quad
    RM_SetQuad(surface, perspectiveQuad);

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        
        // Toggle mapping mode with SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            RM_MapMode currentMode = RM_GetMapMode(surface);
            RM_MapMode newMode = (currentMode == RM_MAP_BILINEAR) 
                                 ? RM_MAP_HOMOGRAPHY 
                                 : RM_MAP_BILINEAR;
            RM_SetMapMode(surface, newMode);
            
            TraceLog(LOG_INFO, "Switched to %s mode", 
                     newMode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY");
        }

        //----------------------------------------------------------------------------------
        // Draw to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            // Draw a reference grid to visualize warping
            for (int x = 0; x <= 8; x++) {
                DrawLine(x * 100, 0, x * 100, 600, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y <= 6; y++) {
                DrawLine(0, y * 100, 800, y * 100, ColorAlpha(WHITE, 0.2f));
            }
            
            // Draw content
            DrawText("WARPED SURFACE", 200, 50, 50, WHITE);
            
            // Draw circles to show distortion
            DrawCircle(200, 200, 80, RED);
            DrawCircle(600, 200, 80, GREEN);
            DrawCircle(200, 400, 80, BLUE);
            DrawCircle(600, 400, 80, YELLOW);
            
            // Center cross
            DrawLine(400, 0, 400, 600, RED);
            DrawLine(0, 300, 800, 300, RED);
            
            DrawText("Notice how the grid warps!", 180, 520, 25, LIGHTGRAY);
        RM_EndSurface(surface);

        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // HUD
            RM_MapMode currentMode = RM_GetMapMode(surface);
            const char *modeName = (currentMode == RM_MAP_BILINEAR) 
                                   ? "BILINEAR" : "HOMOGRAPHY";
            
            DrawText("RAYMAP - BASIC WARPING", 10, 10, 20, GREEN);
            DrawText(TextFormat("Mode: %s", modeName), 10, 40, 20, YELLOW);
            DrawText("[SPACE] Toggle Mode", 10, 70, 20, GRAY);
            
            DrawFPS(screenWidth - 100, 10);
            
            // Info box
            DrawRectangle(10, screenHeight - 180, 500, 170, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, screenHeight - 180, 500, 170, GREEN);
            
            DrawText("BILINEAR: Simple linear interpolation", 20, screenHeight - 170, 16, WHITE);
            DrawText("  - Fast, good for small warps", 20, screenHeight - 150, 14, LIGHTGRAY);
            DrawText("  - Lines stay straight", 20, screenHeight - 130, 14, LIGHTGRAY);
            
            DrawText("HOMOGRAPHY: Perspective-correct", 20, screenHeight - 100, 16, WHITE);
            DrawText("  - Accurate perspective transform", 20, screenHeight - 80, 14, LIGHTGRAY);
            DrawText("  - Better for large perspective warps", 20, screenHeight - 60, 14, LIGHTGRAY);
            DrawText("  - Slightly more expensive", 20, screenHeight - 40, 14, LIGHTGRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RM_DestroySurface(surface);
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
