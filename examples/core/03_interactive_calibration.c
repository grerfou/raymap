/*******************************************************************************************
*
*   raymap - 03_interactive_calibration
*
*   DESCRIPTION:
*       Complete interactive calibration example with all features:
*       - Drag corners with mouse
*       - Toggle calibration mode on/off
*       - Reset quad to default
*       - Save/Load configuration to file
*       - Visual overlay (corners, grid, border)
*
*   DEPENDENCIES:
*       raylib 5.0+
*       raymap 1.1.0+
*
*   COMPILATION (Linux):
*       gcc 03_interactive_calibration.c -o 03_interactive_calibration -lraylib -lm
*
*   COMPILATION (macOS):
*       clang 03_interactive_calibration.c -o 03_interactive_calibration -lraylib \
*             -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT \
*             -framework OpenGL
*
*   COMPILATION (Windows - MinGW):
*       gcc 03_interactive_calibration.c -o 03_interactive_calibration.exe -lraylib \
*           -lopengl32 -lgdi32 -lwinmm
*
*   CONTROLS:
*       C       - Toggle calibration mode
*       R       - Reset quad to centered default
*       S       - Save configuration to file
*       L       - Load configuration from file
*       MOUSE   - Drag corners when calibration is active
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

    InitWindow(screenWidth, screenHeight, "RayMap - 03 Interactive Calibration");
    SetTargetFPS(60);

    // Create surface with homography (better for calibration)
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface!");
        CloseWindow();
        return -1;
    }

    // Initialize calibration with default settings
    RM_Calibration calibration = RM_CalibrationDefault(surface);
    
    // Customize calibration visual config
    calibration.config.showCorners = true;
    calibration.config.showGrid = true;
    calibration.config.showBorder = true;
    calibration.config.cornerColor = YELLOW;
    calibration.config.selectedCornerColor = GREEN;
    calibration.config.gridColor = ColorAlpha(WHITE, 0.3f);
    calibration.config.borderColor = RED;
    calibration.config.cornerRadius = 15.0f;
    calibration.config.gridResolutionX = 8;
    calibration.config.gridResolutionY = 8;
    
    // Start with calibration enabled
    calibration.enabled = true;

    // Config file path
    const char *configFile = "raymap_calibration.cfg";
    
    // Messages
    char statusMessage[256] = "Calibration ready. Drag corners to adjust.";
    float messageTimer = 0.0f;
    const float messageDuration = 3.0f;

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        
        // Update message timer
        if (messageTimer > 0.0f) {
            messageTimer -= deltaTime;
        }
        
        // Toggle calibration mode with C
        if (IsKeyPressed(KEY_C)) {
            RM_ToggleCalibration(&calibration);
            
            if (calibration.enabled) {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Calibration ENABLED - Drag corners");
            } else {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Calibration DISABLED - Press C to enable");
            }
            messageTimer = messageDuration;
        }
        
        // Reset quad with R
        if (IsKeyPressed(KEY_R)) {
            RM_ResetCalibrationQuad(&calibration, screenWidth, screenHeight);
            snprintf(statusMessage, sizeof(statusMessage), 
                     "Quad reset to default centered position");
            messageTimer = messageDuration;
        }
        
        // Save configuration with S
        if (IsKeyPressed(KEY_S)) {
            if (RM_SaveConfig(surface, configFile)) {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Configuration saved to '%s'", configFile);
            } else {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Failed to save configuration!");
            }
            messageTimer = messageDuration;
        }
        
        // Load configuration with L
        if (IsKeyPressed(KEY_L)) {
            if (RM_LoadConfig(surface, configFile)) {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Configuration loaded from '%s'", configFile);
            } else {
                snprintf(statusMessage, sizeof(statusMessage), 
                         "Failed to load configuration (file not found?)");
            }
            messageTimer = messageDuration;
        }
        
        // Update calibration (handles corner dragging)
        RM_UpdateCalibration(&calibration);

        //----------------------------------------------------------------------------------
        // Draw to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            ClearBackground((Color){ 20, 30, 50, 255 });
            
            // Draw reference grid
            for (int x = 0; x <= 8; x++) {
                int xPos = x * 100;
                DrawLine(xPos, 0, xPos, 600, ColorAlpha(SKYBLUE, 0.3f));
                if (x < 8) {
                    DrawText(TextFormat("%d", x * 100), xPos + 5, 5, 15, LIGHTGRAY);
                }
            }
            for (int y = 0; y <= 6; y++) {
                int yPos = y * 100;
                DrawLine(0, yPos, 800, yPos, ColorAlpha(SKYBLUE, 0.3f));
                if (y < 6) {
                    DrawText(TextFormat("%d", y * 100), 5, yPos + 5, 15, LIGHTGRAY);
                }
            }
            
            // Draw content
            DrawText("CALIBRATION TEST", 200, 50, 50, WHITE);
            
            // Draw geometric shapes to visualize distortion
            DrawCircle(200, 200, 60, RED);
            DrawCircle(600, 200, 60, GREEN);
            DrawCircle(400, 300, 60, BLUE);
            DrawCircle(200, 400, 60, YELLOW);
            DrawCircle(600, 400, 60, MAGENTA);
            
            // Draw rectangles
            DrawRectangle(320, 250, 160, 100, ORANGE);
            
            // Center crosshair
            DrawLine(400, 0, 400, 600, ColorAlpha(RED, 0.5f));
            DrawLine(0, 300, 800, 300, ColorAlpha(RED, 0.5f));
            
            DrawText("Drag corners to warp this surface", 150, 520, 25, LIGHTGRAY);
        RM_EndSurface(surface);

        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the warped surface
            RM_DrawSurface(surface);
            
            // Draw calibration overlay (corners, grid, border)
            RM_DrawCalibration(calibration);
            
            // HUD - Title
            DrawText("RAYMAP - INTERACTIVE CALIBRATION", 10, 10, 20, GREEN);
            DrawFPS(screenWidth - 100, 10);
            
            // Controls panel
            DrawRectangle(10, 50, 320, 200, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, 50, 320, 200, GREEN);
            
            DrawText("CONTROLS:", 20, 60, 18, YELLOW);
            DrawText("[C] Toggle Calibration", 20, 90, 16, calibration.enabled ? WHITE : GRAY);
            DrawText("[R] Reset Quad", 20, 115, 16, WHITE);
            DrawText("[S] Save Config", 20, 140, 16, WHITE);
            DrawText("[L] Load Config", 20, 165, 16, WHITE);
            DrawText("[MOUSE] Drag Corners", 20, 190, 16, WHITE);
            DrawText("[ESC] Exit", 20, 215, 16, GRAY);
            
            // Status panel
            DrawRectangle(10, 270, 320, 100, Fade(BLACK, 0.7f));
            DrawRectangleLines(10, 270, 320, 100, BLUE);
            
            DrawText("STATUS:", 20, 280, 18, YELLOW);
            
            const char *modeText = calibration.enabled ? "ACTIVE" : "INACTIVE";
            Color modeColor = calibration.enabled ? GREEN : RED;
            DrawText(TextFormat("Calibration: %s", modeText), 20, 305, 16, modeColor);
            
            int activeCorner = RM_GetActiveCorner(calibration);
            if (activeCorner >= 0) {
                DrawText(TextFormat("Dragging corner: %d", activeCorner), 20, 330, 16, YELLOW);
            } else {
                DrawText("No corner selected", 20, 330, 16, GRAY);
            }
            
            // Status message with timer
            if (messageTimer > 0.0f) {
                int msgY = screenHeight - 60;
                int msgWidth = MeasureText(statusMessage, 20);
                DrawRectangle(screenWidth/2 - msgWidth/2 - 20, msgY - 10, 
                             msgWidth + 40, 50, Fade(BLACK, 0.8f));
                DrawRectangleLines(screenWidth/2 - msgWidth/2 - 20, msgY - 10, 
                                  msgWidth + 40, 50, GREEN);
                DrawText(statusMessage, screenWidth/2 - msgWidth/2, msgY, 20, GREEN);
            }
            
            // Info
            if (!calibration.enabled) {
                DrawText("Press [C] to enable calibration mode", 
                        screenWidth/2 - 180, screenHeight - 30, 20, YELLOW);
            }
            
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
