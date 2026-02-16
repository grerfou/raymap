/*******************************************************************************************
*
*   raymap - 01_minimal_surface
*
*   DESCRIPTION:
*       The absolute minimum working example for RayMap.
*       Creates a simple surface, draws some content on it, and displays it.
*       Perfect starting point to understand the basic workflow.
*
*   DEPENDENCIES:
*       raylib 5.0+
*       raymap 1.1.0+
*
*   COMPILATION (Linux):
*       gcc 01_minimal_surface.c -o 01_minimal_surface -lraylib -lm
*
*   COMPILATION (macOS):
*       clang 01_minimal_surface.c -o 01_minimal_surface -lraylib -framework CoreVideo \
*             -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
*
*   COMPILATION (Windows - MinGW):
*       gcc 01_minimal_surface.c -o 01_minimal_surface.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*
*   CONTROLS:
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

    InitWindow(screenWidth, screenHeight, "RayMap - 01 Minimal Surface");
    SetTargetFPS(60);

    // Create a simple 800x600 surface using bilinear interpolation
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface!");
        CloseWindow();
        return -1;
    }

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        // Nothing to update in this minimal example

        //----------------------------------------------------------------------------------
        // Draw to surface (render texture)
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            // Draw some content on the surface
            DrawText("Hello RayMap!", 250, 250, 60, WHITE);
            DrawCircle(400, 300, 100, YELLOW);
            DrawRectangle(300, 450, 200, 80, RED);
            
            // Draw some info
            DrawText("This is a 800x600 surface", 200, 50, 20, LIGHTGRAY);
            DrawText("Rendered to screen at any resolution", 150, 520, 20, LIGHTGRAY);
        RM_EndSurface(surface);

        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // HUD - overlay info
            DrawText("RAYMAP - MINIMAL EXAMPLE", 10, 10, 20, GREEN);
            DrawFPS(screenWidth - 100, 10);
            
            // Instructions
            DrawText("This surface is currently not warped (default quad)", 
                     10, screenHeight - 30, 16, GRAY);
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
