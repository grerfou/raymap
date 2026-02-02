/**********************************************************************************************
*
*   RayMap Example 03 - Texture Mapping
*
*   DESCRIPTION:
*       Learn how to map textures (images) onto warped surfaces.
*       Load PNG/JPG images, apply rotation, scaling, and animation.
*
*   FEATURES:
*       - Load and display textures
*       - Animated texture rotation
*       - Multiple textures on same surface
*       - Calibration support
*
*   CONTROLS:
*       TAB     - Toggle calibration mode
*       SPACE   - Toggle rotation animation
*       1/2/3   - Switch between different textures
*       ESC     - Exit
*
*   COMPILATION:
*       make
*
*   NOTE:
*       This example expects an image file "texture.png" in the same directory.
*       If not found, it will generate a procedural texture instead.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

//--------------------------------------------------------------------------------------------
// Generate a procedural texture if no image file available
//--------------------------------------------------------------------------------------------
Texture2D GenerateCheckerTexture(int width, int height, int checks)
{
    Image img = GenImageChecked(width, height, checks, checks, BLUE, SKYBLUE);
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

Texture2D GenerateGradientTexture(int width, int height)
{
    Image img = GenImageGradientLinear(width, height, 45, PURPLE, PINK);
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

Texture2D GeneratePerlinTexture(int width, int height)
{
    Image img = GenImagePerlinNoise(width, height, 50, 50, 4.0f);
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

//--------------------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------------------
int main(void)
{
    // Initialize window
    InitWindow(1280, 720, "RayMap Example 03 - Texture Mapping");
    SetTargetFPS(60);
    
    // Create surface
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    if (!surface) {
        TraceLog(LOG_ERROR, "Failed to create surface");
        CloseWindow();
        return -1;
    }
    
    // Try to load texture, fallback to procedural
    Texture2D userTexture = {0};
    bool hasUserTexture = false;
    
    if (FileExists("texture.png")) {
        userTexture = LoadTexture("texture.png");
        hasUserTexture = true;
        printf("✓ Loaded texture.png\n");
    } else {
        printf("ℹ texture.png not found, using procedural textures\n");
    }
    
    // Generate procedural textures
    Texture2D checkerTex = GenerateCheckerTexture(512, 512, 8);
    Texture2D gradientTex = GenerateGradientTexture(512, 512);
    Texture2D perlinTex = GeneratePerlinTexture(512, 512);
    
    // Current texture selection
    int currentTexture = hasUserTexture ? 0 : 1;
    
    // Initialize calibration
    RM_Calibration calib = RM_CalibrationDefault(surface);
    calib.enabled = false;
    
    // Animation
    float rotation = 0.0f;
    bool animateRotation = true;
    float time = 0.0f;
    
    // Main loop
    while (!WindowShouldClose())
    {
        time = GetTime();
        
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        
        // Calibration
        RM_UpdateCalibrationInput(&calib, KEY_TAB);
        
        // Toggle animation
        if (IsKeyPressed(KEY_SPACE)) {
            animateRotation = !animateRotation;
        }
        
        // Switch textures
        if (IsKeyPressed(KEY_ONE) && hasUserTexture) currentTexture = 0;
        if (IsKeyPressed(KEY_TWO)) currentTexture = 1;
        if (IsKeyPressed(KEY_THREE)) currentTexture = 2;
        if (IsKeyPressed(KEY_FOUR)) currentTexture = 3;
        
        // Update rotation
        if (animateRotation) {
            rotation += 30.0f * GetFrameTime();
        }
        
        //----------------------------------------------------------------------------------
        // Draw content to surface
        //----------------------------------------------------------------------------------
        RM_BeginSurface(surface);
            
            ClearBackground(DARKGRAY);
            
            // Select texture
            Texture2D activeTex;
            const char *texName;
            switch (currentTexture) {
                case 0:
                    activeTex = userTexture;
                    texName = "User Texture";
                    break;
                case 1:
                    activeTex = checkerTex;
                    texName = "Checker Pattern";
                    break;
                case 2:
                    activeTex = gradientTex;
                    texName = "Gradient";
                    break;
                case 3:
                    activeTex = perlinTex;
                    texName = "Perlin Noise";
                    break;
                default:
                    activeTex = checkerTex;
                    texName = "Checker Pattern";
                    break;
            }
            
            // Draw rotating texture in center
            Rectangle source = {0, 0, (float)activeTex.width, (float)activeTex.height};
            Rectangle dest = {400, 300, 400, 400};  // Centered, 400x400
            Vector2 origin = {200, 200};  // Center of rotation
            
            DrawTexturePro(activeTex, source, dest, origin, rotation, WHITE);
            
            // Background decoration
            DrawRectangleLines(50, 50, 700, 500, LIGHTGRAY);
            
            // Info text
            DrawText("TEXTURE MAPPING", 220, 30, 40, WHITE);
            DrawText(texName, 300, 550, 25, YELLOW);
            
            // Rotation indicator
            if (animateRotation) {
                DrawText("ROTATING", 330, 520, 20, GREEN);
            } else {
                DrawText("STATIC", 345, 520, 20, RED);
            }
            
        RM_EndSurface(surface);
        
        //----------------------------------------------------------------------------------
        // Draw to screen
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(BLACK);
            
            // Draw warped surface
            RM_DrawSurface(surface);
            
            // Draw calibration overlay
            if (calib.enabled) {
                RM_DrawCalibration(calib);
            }
            
            // UI Panel
            DrawRectangle(0, 0, 400, 220, Fade(BLACK, 0.7f));
            DrawText("Example 03 - Texture Mapping", 10, 10, 20, WHITE);
            
            DrawText("Controls:", 10, 45, 18, YELLOW);
            DrawText("  TAB   - Toggle calibration", 10, 65, 16, LIGHTGRAY);
            DrawText("  SPACE - Toggle rotation", 10, 85, 16, LIGHTGRAY);
            if (hasUserTexture) {
                DrawText("  1     - User texture (texture.png)", 10, 105, 16, LIGHTGRAY);
            }
            DrawText("  2     - Checker pattern", 10, 125, 16, LIGHTGRAY);
            DrawText("  3     - Gradient", 10, 145, 16, LIGHTGRAY);
            DrawText("  4     - Perlin noise", 10, 165, 16, LIGHTGRAY);
            DrawText("  ESC   - Exit", 10, 185, 16, LIGHTGRAY);
            
            // Status
            DrawText(TextFormat("Rotation: %.1f°", rotation), 10, 690, 18, GREEN);
            DrawText(TextFormat("FPS: %d", GetFPS()), 1200, 10, 20, LIME);
            
        EndDrawing();
    }
    
    // Cleanup
    if (hasUserTexture) UnloadTexture(userTexture);
    UnloadTexture(checkerTex);
    UnloadTexture(gradientTex);
    UnloadTexture(perlinTex);
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    return 0;
}
