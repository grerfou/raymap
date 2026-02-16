/*******************************************************************************************
*
*   raymap - 05_point_mapping
*
*   DESCRIPTION:
*       Demonstrates point mapping between texture space and screen space.
*       Shows how to detect mouse clicks on a warped surface and map them
*       to texture coordinates. Useful for interactive applications with
*       warped surfaces (touch screens, projection mapping, etc.)
*
*   DEPENDENCIES:
*       raylib 5.0+
*       raymap 1.1.0+
*
*   COMPILATION (Linux):
*       gcc 05_point_mapping.c -o 05_point_mapping -lraylib -lm
*
*   COMPILATION (macOS):
*       clang 05_point_mapping.c -o 05_point_mapping -lraylib -framework CoreVideo \
*             -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
*
*   COMPILATION (Windows - MinGW):
*       gcc 05_point_mapping.c -o 05_point_mapping.exe -lraylib -lopengl32 -lgdi32 -lwinmm
*
*   CONTROLS:
*       LEFT CLICK  - Add marker at click position
*       RIGHT CLICK - Clear all markers
*       C           - Toggle calibration mode
*       R           - Reset quad
*       ESC         - Exit
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2026 grerfou
*
********************************************************************************************/

#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#define MAX_MARKERS 50

// Marker structure
typedef struct {
    Vector2 texturePos;   // Position in texture space [0,1]
    Color color;
    bool active;
} Marker;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "RayMap - 05 Point Mapping");
    SetTargetFPS(60);

    // Create surface
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface!");
        CloseWindow();
        return -1;
    }

    // Apply a warped quad
    RM_Quad warpedQuad = {
        .topLeft = { 250, 100 },
        .topRight = { 1670, 120 },
        .bottomRight = { 1600, 950 },
        .bottomLeft = { 320, 980 }
    };
    RM_SetQuad(surface, warpedQuad);

    // Calibration
    RM_Calibration calibration = RM_CalibrationDefault(surface);
    calibration.enabled = false;  // Start disabled

    // Markers array
    Marker markers[MAX_MARKERS] = { 0 };
    int markerCount = 0;

    // Hover info
    Vector2 hoverTexturePos = { -1, -1 };
    bool hoverValid = false;

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        
        // Toggle calibration
        if (IsKeyPressed(KEY_C)) {
            RM_ToggleCalibration(&calibration);
        }
        
        // Reset quad
        if (IsKeyPressed(KEY_R)) {
            RM_ResetCalibrationQuad(&calibration, screenWidth, screenHeight);
        }
        
        // Update calibration
        RM_UpdateCalibration(&calibration);
        
        // Mouse interaction (only when not calibrating)
        if (!RM_IsCalibrating(calibration)) {
            Vector2 mousePos = GetMousePosition();
            
            // Unmap mouse position to texture coordinates
            hoverTexturePos = RM_UnmapPoint(surface, mousePos);
            hoverValid = (hoverTexturePos.x >= 0.0f && hoverTexturePos.x <= 1.0f &&
                         hoverTexturePos.y >= 0.0f && hoverTexturePos.y <= 1.0f);
            
            // Add marker on left click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverValid) {
                if (markerCount < MAX_MARKERS) {
                    markers[markerCount].texturePos = hoverTexturePos;
                    
                    // Random color for each marker
                    markers[markerCount].color = ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f);
                    markers[markerCount].active = true;
                    
                    markerCount++;
                    TraceLog(LOG_INFO, "Marker added at texture coords: (%.3f, %.3f)", 
                             hoverTexturePos.x, hoverTexturePos.y);
                }
            }
            
            // Clear all markers on right click
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                markerCount = 0;
                for (int i = 0; i < MAX_MARKERS; i++) {
                    markers[i].active = false;
                }
                TraceLog(LOG_INFO, "All markers cleared");
            }
        }

        //----------------------------------------------------------------------------------
        // Draw to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            ClearBackground((Color){ 25, 35, 45, 255 });
            
            // Draw coordinate grid
            for (int x = 0; x <= 8; x++) {
                int xPos = x * 100;
                DrawLine(xPos, 0, xPos, 600, ColorAlpha(DARKGRAY, 0.5f));
                DrawText(TextFormat("%.1f", x / 8.0f), xPos + 5, 5, 12, GRAY);
            }
            for (int y = 0; y <= 6; y++) {
                int yPos = y * 100;
                DrawLine(0, yPos, 800, yPos, ColorAlpha(DARKGRAY, 0.5f));
                DrawText(TextFormat("%.1f", y / 6.0f), 5, yPos + 5, 12, GRAY);
            }
            
            // Draw origin marker
            DrawCircle(0, 0, 15, RED);
            DrawText("(0,0)", 20, 10, 15, RED);
            
            // Draw (1,1) marker
            DrawCircle(800, 600, 15, GREEN);
            DrawText("(1,1)", 750, 570, 15, GREEN);
            
            // Title
            DrawText("POINT MAPPING DEMO", 220, 50, 40, WHITE);
            DrawText("Click anywhere to add markers", 220, 100, 20, LIGHTGRAY);
            
            // Draw all markers in texture space
            for (int i = 0; i < markerCount; i++) {
                if (markers[i].active) {
                    // Convert normalized coords [0,1] to texture pixels
                    int x = (int)(markers[i].texturePos.x * 800);
                    int y = (int)(markers[i].texturePos.y * 600);
                    
                    // Draw marker
                    DrawCircle(x, y, 12, markers[i].color);
                    DrawCircleLines(x, y, 12, WHITE);
                    DrawText(TextFormat("%d", i + 1), x - 5, y - 7, 14, BLACK);
                    
                    // Draw crosshair
                    DrawLine(x - 20, y, x - 13, y, WHITE);
                    DrawLine(x + 13, y, x + 20, y, WHITE);
                    DrawLine(x, y - 20, x, y - 13, WHITE);
                    DrawLine(x, y + 13, x, y + 20, WHITE);
                }
            }
            
            DrawText(TextFormat("Markers: %d/%d", markerCount, MAX_MARKERS), 
                     280, 520, 25, YELLOW);
            
        RM_EndSurface(surface);

        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // Draw calibration overlay if active
            if (calibration.enabled) {
                RM_DrawCalibration(calibration);
            }
            
            // Draw hover cursor and info
            if (hoverValid && !RM_IsCalibrating(calibration)) {
                Vector2 mousePos = GetMousePosition();
                
                // Draw cursor crosshair
                DrawLineEx((Vector2){mousePos.x - 20, mousePos.y}, 
                          (Vector2){mousePos.x + 20, mousePos.y}, 2, LIME);
                DrawLineEx((Vector2){mousePos.x, mousePos.y - 20}, 
                          (Vector2){mousePos.x, mousePos.y + 20}, 2, LIME);
                
                // Show texture coordinates
                DrawText(TextFormat("UV: (%.3f, %.3f)", hoverTexturePos.x, hoverTexturePos.y),
                        (int)mousePos.x + 25, (int)mousePos.y - 10, 16, LIME);
            }
            
            // HUD
            DrawText("RAYMAP - POINT MAPPING", 10, 10, 20, GREEN);
            DrawFPS(screenWidth - 100, 10);
            
            // Controls panel
            DrawRectangle(10, 50, 400, 180, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, 50, 400, 180, GREEN);
            
            DrawText("CONTROLS:", 20, 60, 18, YELLOW);
            DrawText("[LEFT CLICK]  Add Marker", 20, 90, 16, WHITE);
            DrawText("[RIGHT CLICK] Clear All Markers", 20, 115, 16, WHITE);
            DrawText("[C] Toggle Calibration", 20, 140, 16, calibration.enabled ? GREEN : GRAY);
            DrawText("[R] Reset Quad", 20, 165, 16, WHITE);
            DrawText("[ESC] Exit", 20, 190, 16, GRAY);
            
            // Marker list
            if (markerCount > 0) {
                int panelHeight = 60 + markerCount * 22;
                DrawRectangle(screenWidth - 310, 50, 300, panelHeight, Fade(BLACK, 0.7f));
                DrawRectangleLines(screenWidth - 310, 50, 300, panelHeight, BLUE);
                
                DrawText("MARKERS:", screenWidth - 300, 60, 18, YELLOW);
                
                for (int i = 0; i < markerCount && i < 10; i++) {
                    if (markers[i].active) {
                        int y = 90 + i * 22;
                        DrawCircle(screenWidth - 290, y + 6, 6, markers[i].color);
                        DrawText(TextFormat("#%d: (%.3f, %.3f)", 
                                          i + 1, 
                                          markers[i].texturePos.x, 
                                          markers[i].texturePos.y),
                                screenWidth - 275, y, 14, WHITE);
                    }
                }
                
                if (markerCount > 10) {
                    DrawText(TextFormat("... and %d more", markerCount - 10),
                            screenWidth - 275, 90 + 10 * 22, 14, GRAY);
                }
            }
            
            // Info box
            DrawRectangle(10, screenHeight - 150, 650, 140, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, screenHeight - 150, 650, 140, ORANGE);
            
            DrawText("HOW IT WORKS:", 20, screenHeight - 140, 16, ORANGE);
            DrawText("• RM_UnmapPoint() converts screen coordinates to texture space [0,1]", 
                     20, screenHeight - 115, 14, WHITE);
            DrawText("• RM_MapPoint() converts texture coordinates to screen space", 
                     20, screenHeight - 95, 14, WHITE);
            DrawText("• Useful for: Click detection, touch input, interactive surfaces", 
                     20, screenHeight - 75, 14, WHITE);
            DrawText("• Works even with heavily warped/perspective-distorted surfaces!", 
                     20, screenHeight - 55, 14, LIGHTGRAY);
            DrawText("• Try calibrating the surface and see how markers stay in place", 
                     20, screenHeight - 35, 14, LIGHTGRAY);
            
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
