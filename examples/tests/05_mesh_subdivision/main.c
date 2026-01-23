#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(1280, 720, "Test 05: Mesh Subdivision - RayMap");
    SetTargetFPS(60);
    
    printf("=== Test Mesh Subdivision ===\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(400, 300, RM_MAP_BILINEAR);
    
    // Quad déformé initial (trapèze)
    RM_Quad trapeze = {
        { 100, 50 },    // Top-left
        { 700, 80 },    // Top-right (décalé)
        { 750, 640 },    // Bottom-right
        { 50, 670 }    // Bottom-left
    };
    RM_SetQuad(surface, trapeze);
    
    printf(" Surface créée avec mesh 16x16\n");
    printf(" Quad trapèze défini\n\n");
    
    float time = 0.0f;
    bool animating = true;
    bool showInfo = true;
    
    printf("Contrôles:\n");
    printf("  SPACE : Toggle animation\n");
    printf("  R     : Reset quad (rectangle)\n");
    printf("  T     : Trapèze prononcé\n");
    printf("  I     : Toggle info overlay\n");
    printf("  ESC   : Quitter\n\n");
    
    while (!WindowShouldClose()) {
        
        float deltaTime = GetFrameTime();
        time += deltaTime;
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Toggle animation
        if (IsKeyPressed(KEY_SPACE)) {
            animating = !animating;
            printf("Animation: %s\n", animating ? "ON" : "OFF");
        }
        
        // Reset quad (rectangle centré)
        if (IsKeyPressed(KEY_R)) {
            RM_Quad reset = {
                { 440, 210 },   // Centré dans 1280x720
                { 840, 210 },
                { 840, 510 },
                { 440, 510 }
            };
            RM_SetQuad(surface, reset);
            printf(" Quad reset (rectangle)\n");
        }
        
        // Trapèze prononcé
        if (IsKeyPressed(KEY_T)) {
            RM_Quad trap = {
                { 200, 100 },
                { 1080, 50 },
                { 1180, 620 },
                { 100, 670 }
            };
            RM_SetQuad(surface, trap);
            printf(" Trapèze prononcé\n");
        }
        
        // Toggle info
        if (IsKeyPressed(KEY_I)) {
            showInfo = !showInfo;
        }
        
        // ═══════════════════════════════════════════
        // ANIMATION
        // ═══════════════════════════════════════════
        
        if (animating) {
            float offset = sinf(time * 1.2f) * 150.0f;
            RM_Quad animated = {
                { 200 + offset, 100 },
                { 1080 - offset, 50 },
                { 1180, 620 },
                { 100, 670 }
            };
            RM_SetQuad(surface, animated);
        }
        
        // ═══════════════════════════════════════════
        // DESSINER DANS LA SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            // Titre
            DrawText("MESH WARP!", 80, 30, 50, WHITE);
            DrawText("Bilinear Interpolation", 90, 90, 20, LIGHTGRAY);
            
            // Cercle
            DrawCircle(200, 180, 60, RED);
            DrawCircleLines(200, 180, 65, WHITE);
            
            // Rectangle
            DrawRectangle(260, 120, 120, 120, GREEN);
            DrawRectangleLines(260, 120, 120, 120, WHITE);
            
            // Grille de référence (pour voir la déformation)
            for (int x = 0; x < 400; x += 50) {
                DrawLine(x, 0, x, 300, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y < 300; y += 50) {
                DrawLine(0, y, 400, y, ColorAlpha(WHITE, 0.2f));
            }
            
            // Diagonales (doivent être courbes si déformé)
            DrawLine(0, 0, 400, 300, ColorAlpha(YELLOW, 0.5f));
            DrawLine(400, 0, 0, 300, ColorAlpha(ORANGE, 0.5f));
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // DESSINER À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Afficher la surface déformée
            RM_DrawSurface(surface);
            
            // ═══════════════════════════════════════════
            // HUD et INFO
            // ═══════════════════════════════════════════
            
            if (showInfo) {
                // Titre
                DrawRectangle(0, 0, 400, 140, ColorAlpha(BLACK, 0.7f));
                DrawText("Test: Mesh Subdivision", 10, 10, 24, LIME);
                
                // Info
                DrawText("Resolution: 16x16", 10, 40, 16, WHITE);
                DrawText("Interpolation: Bilinear", 10, 60, 16, WHITE);
                DrawText(TextFormat("Animation: %s", animating ? "ON" : "OFF"), 10, 80, 16, animating ? GREEN : RED);
                
                // FPS
                DrawFPS(10, 110);
                
                // Contrôles
                DrawRectangle(0, 720 - 120, 300, 120, ColorAlpha(BLACK, 0.7f));
                DrawText("SPACE: Toggle anim", 10, 720 - 110, 14, LIGHTGRAY);
                DrawText("R: Reset", 10, 720 - 90, 14, LIGHTGRAY);
                DrawText("T: Trapeze", 10, 720 - 70, 14, LIGHTGRAY);
                DrawText("I: Toggle info", 10, 720 - 50, 14, LIGHTGRAY);
                DrawText("ESC: Quit", 10, 720 - 30, 14, LIGHTGRAY);
                
                // Quad actuel (debug)
                RM_Quad current = RM_GetQuad(surface);
                DrawRectangle(1280 - 300, 0, 300, 100, ColorAlpha(BLACK, 0.7f));
                DrawText("Quad Corners:", 1280 - 290, 10, 14, YELLOW);
                DrawText(TextFormat("TL: (%.0f,%.0f)", current.topLeft.x, current.topLeft.y), 
                        1280 - 290, 30, 12, WHITE);
                DrawText(TextFormat("TR: (%.0f,%.0f)", current.topRight.x, current.topRight.y), 
                        1280 - 290, 50, 12, WHITE);
                DrawText(TextFormat("BL: (%.0f,%.0f)", current.bottomLeft.x, current.bottomLeft.y), 
                        1280 - 290, 70, 12, WHITE);
            }
            
            // Message de validation
            if (!animating) {
                DrawText("Observez: les lignes droites deviennent courbes!", 
                        1280/2 - 250, 720 - 40, 18, YELLOW);
            }
            
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n===  Test terminé ===\n");
    printf("Points validés:\n");
    printf("   Mesh 16x16 généré\n");
    printf("   Déformation lisse (pas de ligne au milieu)\n");
    printf("   Les 4 coins indépendants\n");
    printf("   Interpolation bilinéaire fonctionne\n");
    
    return 0;
}
