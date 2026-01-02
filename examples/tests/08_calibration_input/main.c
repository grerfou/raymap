#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(1280, 720, "Test 08: Calibration Input - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════\n");
    printf("  Test: Calibration Interactive\n");
    printf("═══════════════════════════════════════\n\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_MESH);
    
    // Quad initial (centré)
    RM_Quad initialQuad = {
        { 200, 150 },
        { 1080, 150 },
        { 200, 570 },
        { 1080, 570 }
    };
    RM_SetQuad(surface, initialQuad);
    
    // Créer la calibration
    RM_Calibration *calibration = RM_CreateCalibration(surface);
    
    printf(" Surface créée\n");
    printf(" Calibration créée\n\n");
    
    printf("Contrôles:\n");
    printf("  CLIC GAUCHE : Sélectionner et déplacer un coin\n");
    printf("  R          : Reset quad\n");
    printf("  G          : Toggle grille\n");
    printf("  B          : Toggle bordure\n");
    printf("  C          : Toggle coins\n");
    printf("  ESC        : Quitter\n\n");
    
    while (!WindowShouldClose()) {
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Reset quad
        if (IsKeyPressed(KEY_R)) {
            RM_SetQuad(surface, initialQuad);
            printf(" Quad reset\n");
        }
        
        // Toggle options
        RM_CalibrationConfig *config = RM_GetCalibrationConfig(calibration);
        
        if (IsKeyPressed(KEY_G)) {
            config->showGrid = !config->showGrid;
            printf(" Grille: %s\n", config->showGrid ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_B)) {
            config->showBorder = !config->showBorder;
            printf(" Bordure: %s\n", config->showBorder ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_C)) {
            config->showCorners = !config->showCorners;
            printf(" Coins: %s\n", config->showCorners ? "ON" : "OFF");
        }
        
        // ═══════════════════════════════════════════
        // CALIBRATION UPDATE
        // ═══════════════════════════════════════════
        
        RM_BeginCalibration(calibration);
        RM_UpdateCalibration(calibration);  // ← Gestion du drag ici
        RM_EndCalibration(calibration);
        
        // ═══════════════════════════════════════════
        // DESSINER DANS LA SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("CALIBRATION", 100, 50, 50, WHITE);
            DrawText("Drag corners!", 120, 120, 30, YELLOW);
            
            // Grille de référence
            for (int x = 0; x < 500; x += 50) {
                DrawLine(x, 0, x, 400, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y < 400; y += 50) {
                DrawLine(0, y, 500, y, ColorAlpha(WHITE, 0.2f));
            }
            
            // Cercle
            DrawCircle(250, 200, 60, RED);
            DrawCircleLines(250, 200, 65, WHITE);
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // DESSINER À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // 1. Afficher la surface
            RM_DrawSurface(surface);
            
            // 2. Afficher l'UI de calibration PAR-DESSUS
            RM_DrawCalibration(calibration);
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            DrawRectangle(0, 0, 1280, 60, ColorAlpha(BLACK, 0.8f));
            DrawText("Test 08: Calibration Interactive", 20, 15, 24, LIME);
            DrawText("🖱️  Cliquez et glissez les coins jaunes", 20, 40, 16, YELLOW);
            
            // Info coin actif
            if (calibration->activeCorner >= 0) {
                const char *cornerNames[] = {"TOP-LEFT", "TOP-RIGHT", "BOTTOM-RIGHT", "BOTTOM-LEFT"};
                DrawText(TextFormat("Coin actif: %s", cornerNames[calibration->activeCorner]), 
                        1280 - 300, 15, 18, RED);
            }
            
            // Contrôles
            DrawRectangle(0, 720 - 140, 320, 140, ColorAlpha(BLACK, 0.8f));
            DrawText("Contrôles:", 10, 720 - 130, 18, YELLOW);
            DrawText("CLIC: Déplacer coin", 10, 720 - 105, 14, LIGHTGRAY);
            DrawText("R: Reset", 10, 720 - 85, 14, LIGHTGRAY);
            DrawText("G: Toggle grille", 10, 720 - 65, 14, LIGHTGRAY);
            DrawText("B: Toggle bordure", 10, 720 - 45, 14, LIGHTGRAY);
            DrawText("C: Toggle coins", 10, 720 - 25, 14, LIGHTGRAY);
            
            DrawFPS(1280 - 100, 720 - 30);
            
        EndDrawing();
    }
    
    RM_DestroyCalibration(calibration);
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════\n");
    printf("   Test terminé\n");
    printf("═══════════════════════════════════════\n");
    
    return 0;
}
