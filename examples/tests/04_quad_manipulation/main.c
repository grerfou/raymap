#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(800, 600, "Test Quad Manipulation");
    SetTargetFPS(60);
    
    printf("=== Test Quad Manipulation ===\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(400, 300, RM_MAP_MESH);
    
    // Test 1 : GetQuad sur quad par défaut
    printf("\nTest 1: Get quad par défaut\n");
    RM_Quad defaultQuad = RM_GetQuad(surface);
    printf("  TL: (%.0f, %.0f)\n", defaultQuad.topLeft.x, defaultQuad.topLeft.y);
    printf("  TR: (%.0f, %.0f)\n", defaultQuad.topRight.x, defaultQuad.topRight.y);
    printf("  BL: (%.0f, %.0f)\n", defaultQuad.bottomLeft.x, defaultQuad.bottomLeft.y);
    printf("  BR: (%.0f, %.0f)\n", defaultQuad.bottomRight.x, defaultQuad.bottomRight.y);
    
    // Test 2 : GetSurfaceSize
    printf("\nTest 2: Get surface size\n");
    int w, h;
    RM_GetSurfaceSize(surface, &w, &h);
    printf("  Size: %dx%d\n", w, h);
    
    // Test 3 : Quad déformé (trapèze)
    printf("\nTest 3: Set quad déformé (trapèze)\n");
    RM_Quad trapeze = {
        { 150, 100 },   // Top-left
        { 650, 80 },    // Top-right (décalé)
        { 100, 500 },   // Bottom-left
        { 700, 520 }    // Bottom-right
    };
    RM_SetQuad(surface, trapeze);
    
    // Vérifier que le quad a bien changé
    RM_Quad newQuad = RM_GetQuad(surface);
    printf("  Nouveau quad défini\n");
    printf("  TL: (%.0f, %.0f)\n", newQuad.topLeft.x, newQuad.topLeft.y);
    
    // Variables pour animation
    float time = 0.0f;
    bool animating = true;
    
    // Boucle principale
    while (!WindowShouldClose()) {
        
        float deltaTime = GetFrameTime();
        time += deltaTime;
        
        // Animation du quad (oscillation)
        if (animating) {
            float offset = sinf(time * 2.0f) * 50.0f;
            
            RM_Quad animated = {
                { 150 + offset, 100 },
                { 650 - offset, 80 },
                { 100, 500 },
                { 700, 520 }
            };
            RM_SetQuad(surface, animated);
        }
        
        // Toggle animation
        if (IsKeyPressed(KEY_SPACE)) {
            animating = !animating;
            printf("Animation: %s\n", animating ? "ON" : "OFF");
        }
        
        // Reset quad
        if (IsKeyPressed(KEY_R)) {
            RM_Quad reset = {
                { 200, 150 },
                { 600, 150 },
                { 200, 450 },
                { 600, 450 }
            };
            RM_SetQuad(surface, reset);
            printf("Quad reset\n");
        }
        
        // Dessiner DANS la surface
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("HELLO RAYMAP!", 50, 50, 30, WHITE);
            DrawCircle(200, 150, 40, RED);
            DrawRectangle(250, 100, 100, 50, GREEN);
            
            // Grille pour voir la déformation
            for (int x = 0; x < 400; x += 50) {
                DrawLine(x, 0, x, 300, ColorAlpha(WHITE, 0.3f));
            }
            for (int y = 0; y < 300; y += 50) {
                DrawLine(0, y, 400, y, ColorAlpha(WHITE, 0.3f));
            }
            
        RM_EndSurface(surface);
        
        // Dessiner à l'écran
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Afficher la surface déformée
            RM_DrawSurface(surface);
            
            // HUD
            DrawText("Test: Quad Manipulation", 10, 10, 20, LIME);
            DrawText("SPACE: Toggle animation", 10, 35, 16, WHITE);
            DrawText("R: Reset quad", 10, 55, 16, WHITE);
            DrawFPS(10, 75);
            
            // Afficher le quad actuel
            RM_Quad current = RM_GetQuad(surface);
            DrawText(TextFormat("TL: (%.0f,%.0f)", current.topLeft.x, current.topLeft.y), 
                    10, 100, 14, YELLOW);
            
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n✅ Test terminé\n");
    return 0;
}
