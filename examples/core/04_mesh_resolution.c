/*******************************************************************************************
*
*   raymap - 04_mesh_resolution
*
*   DESCRIPTION:
*       Demonstrates the impact of mesh resolution on warping quality and performance.
*       Shows a complex curved pattern that requires high mesh resolution to render
*       correctly. Users can adjust resolution in real-time to see the trade-off
*       between quality and performance.
*
*   DEPENDENCIES:
*       raylib 5.0+
*       raymap 1.1.0+
*
*   COMPILATION (Linux):
*       gcc 04_mesh_resolution.c -o 04_mesh_resolution -lraylib -lm
*
*   COMPILATION (macOS):
*       clang 04_mesh_resolution.c -o 04_mesh_resolution -lraylib -framework CoreVideo \
*             -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
*
*   COMPILATION (Windows - MinGW):
*       gcc 04_mesh_resolution.c -o 04_mesh_resolution.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*
*   CONTROLS:
*       UP      - Increase mesh resolution
*       DOWN    - Decrease mesh resolution
*       SPACE   - Reset to default resolution
*       ESC     - Exit
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2026 grerfou
*
********************************************************************************************/

#include "raylib.h"
#include <math.h>

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

    InitWindow(screenWidth, screenHeight, "RayMap - 04 Mesh Resolution");
    SetTargetFPS(60);

    // Create surface with homography
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface!");
        CloseWindow();
        return -1;
    }

    // Start with medium resolution
    int meshColumns = 16;
    int meshRows = 16;
    RM_SetMeshResolution(surface, meshColumns, meshRows);

    // Apply a warped quad
    RM_Quad warpedQuad = {
        .topLeft = { 50, 50 },
        .topRight = { 1870, 100 },
        .bottomRight = { 1820, 1030 },
        .bottomLeft = { 100, 980 }
    };
    RM_SetQuad(surface, warpedQuad);

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        
        // Increase resolution
        if (IsKeyPressed(KEY_UP)) {
            meshColumns = (meshColumns < 64) ? meshColumns + 4 : 64;
            meshRows = (meshRows < 64) ? meshRows + 4 : 64;
            RM_SetMeshResolution(surface, meshColumns, meshRows);
            TraceLog(LOG_INFO, "Mesh resolution: %dx%d", meshColumns, meshRows);
        }
        
        // Decrease resolution
        if (IsKeyPressed(KEY_DOWN)) {
            meshColumns = (meshColumns > 4) ? meshColumns - 4 : 4;
            meshRows = (meshRows > 4) ? meshRows - 4 : 4;
            RM_SetMeshResolution(surface, meshColumns, meshRows);
            TraceLog(LOG_INFO, "Mesh resolution: %dx%d", meshColumns, meshRows);
        }
        
        // Reset to default
        if (IsKeyPressed(KEY_SPACE)) {
            meshColumns = 16;
            meshRows = 16;
            RM_SetMeshResolution(surface, meshColumns, meshRows);
            TraceLog(LOG_INFO, "Mesh resolution reset to default: %dx%d", meshColumns, meshRows);
        }

        //----------------------------------------------------------------------------------
        // Draw to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            ClearBackground((Color){ 15, 15, 25, 255 });
            
            // Draw complex curved patterns that stress-test mesh resolution
            
            // Concentric circles (need good mesh for smooth curves)
            for (int i = 0; i < 25; i++) {
                float radius = 30 + i * 15;
                Color circleColor = ColorFromHSV(i * 14.4f, 0.8f, 0.9f);
                DrawCircleLines(400, 300, radius, ColorAlpha(circleColor, 0.6f));
            }
            
            // Spiral pattern
            int numPoints = 200;
            for (int i = 0; i < numPoints - 1; i++) {
                float angle1 = (i / (float)numPoints) * PI * 6;
                float angle2 = ((i + 1) / (float)numPoints) * PI * 6;
                float radius1 = (i / (float)numPoints) * 200;
                float radius2 = ((i + 1) / (float)numPoints) * 200;
                
                Vector2 p1 = { 400 + cosf(angle1) * radius1, 300 + sinf(angle1) * radius1 };
                Vector2 p2 = { 400 + cosf(angle2) * radius2, 300 + sinf(angle2) * radius2 };
                
                DrawLineEx(p1, p2, 2.0f, ColorAlpha(SKYBLUE, 0.8f));
            }
            
            // Sine wave grid
            for (int x = 0; x < 800; x += 20) {
                for (int y = 0; y < 600; y += 20) {
                    float wave = sinf(x * 0.02f) * cosf(y * 0.02f) * 5.0f;
                    DrawCircle(x, y, 2 + wave, ColorAlpha(YELLOW, 0.3f));
                }
            }
            
            // Title and info
            DrawText("MESH RESOLUTION TEST", 220, 30, 40, WHITE);
            DrawText("Notice how curves smooth out with higher resolution", 
                     120, 550, 20, LIGHTGRAY);
            
            // Corner markers
            DrawCircle(50, 50, 10, RED);
            DrawCircle(750, 50, 10, GREEN);
            DrawCircle(750, 550, 10, BLUE);
            DrawCircle(50, 550, 10, YELLOW);
            
        RM_EndSurface(surface);

        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // HUD
            DrawText("RAYMAP - MESH RESOLUTION", 10, 10, 20, GREEN);
            DrawFPS(screenWidth - 100, 10);
            
            // Controls panel
            DrawRectangle(10, 50, 350, 150, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, 50, 350, 150, GREEN);
            
            DrawText("CONTROLS:", 20, 60, 18, YELLOW);
            DrawText("[UP]    Increase Resolution (+4)", 20, 90, 16, WHITE);
            DrawText("[DOWN]  Decrease Resolution (-4)", 20, 115, 16, WHITE);
            DrawText("[SPACE] Reset to Default (16x16)", 20, 140, 16, WHITE);
            DrawText("[ESC]   Exit", 20, 165, 16, GRAY);
            
            // Info panel
            DrawRectangle(10, 220, 350, 180, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, 220, 350, 180, BLUE);
            
            DrawText("MESH INFO:", 20, 230, 18, YELLOW);
            
            // Get current mesh resolution
            int currentCols, currentRows;
            RM_GetMeshResolution(surface, &currentCols, &currentRows);
            
            DrawText(TextFormat("Resolution: %dx%d", currentCols, currentRows), 
                     20, 260, 18, WHITE);
            
            int vertexCount = (currentCols + 1) * (currentRows + 1);
            int triangleCount = currentCols * currentRows * 2;
            
            DrawText(TextFormat("Vertices: %d", vertexCount), 20, 285, 16, LIGHTGRAY);
            DrawText(TextFormat("Triangles: %d", triangleCount), 20, 310, 16, LIGHTGRAY);
            
            // Quality indicator
            const char *qualityText;
            Color qualityColor;
            
            if (currentCols >= 32) {
                qualityText = "Quality: HIGH";
                qualityColor = GREEN;
            } else if (currentCols >= 16) {
                qualityText = "Quality: MEDIUM";
                qualityColor = YELLOW;
            } else {
                qualityText = "Quality: LOW";
                qualityColor = RED;
            }
            
            DrawText(qualityText, 20, 340, 16, qualityColor);
            DrawText("(for this curved content)", 20, 360, 14, GRAY);
            
            // Tip
            DrawRectangle(10, screenHeight - 100, 600, 90, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, screenHeight - 100, 600, 90, ORANGE);
            DrawText("TIP:", 20, screenHeight - 90, 16, ORANGE);
            DrawText("Higher mesh resolution = smoother curves but more GPU load", 
                     20, screenHeight - 65, 14, WHITE);
            DrawText("Use lower resolution for simple content or flat surfaces", 
                     20, screenHeight - 45, 14, WHITE);
            DrawText("Use higher resolution for complex curves and strong warps", 
                     20, screenHeight - 25, 14, WHITE);
            
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
