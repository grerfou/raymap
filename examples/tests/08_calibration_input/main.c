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
    
    // ═══════════════════════════════════════════
    // SETUP
    // ═══════════════════════════════════════════
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_BILINEAR);
    
    // Quad initial (centré)
    RM_Quad initialQuad = {
        { 200, 150 },
        { 1080, 150 },
        { 200, 570 },
        { 1080, 570 }
    };
    RM_SetQuad(surface, initialQuad);
    
    //  NOUVEAU : Créer calibration (struct sur la stack, pas de malloc)
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    printf("✓ Surface créée\n");
    printf("✓ Calibration créée\n\n");
    
    printf("Contrôles:\n");
    printf("  CLIC GAUCHE : Sélectionner et déplacer un coin\n");
    printf("  TAB         : Toggle mode calibration\n");
    printf("  R           : Reset quad\n");
    printf("  G           : Toggle grille\n");
    printf("  B           : Toggle bordure\n");
    printf("  C           : Toggle coins\n");
    printf("  ESC         : Quitter\n\n");
    
    // ═══════════════════════════════════════════
    // MAIN LOOP
    // ═══════════════════════════════════════════
    
    while (!WindowShouldClose()) {
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        //  NOUVEAU : Toggle calibration avec TAB
        if (IsKeyPressed(KEY_TAB)) {
            RM_ToggleCalibration(&calib);
            printf("✓ Calibration: %s\n", calib.enabled ? "ON" : "OFF");
        }
        
        // Reset quad
        if (IsKeyPressed(KEY_R)) {
            RM_SetQuad(surface, initialQuad);
            printf("✓ Quad reset\n");
        }
        
        // ✨ NOUVEAU : Accès direct à la config (struct publique)
        if (IsKeyPressed(KEY_G)) {
            calib.config.showGrid = !calib.config.showGrid;
            printf("✓ Grille: %s\n", calib.config.showGrid ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_B)) {
            calib.config.showBorder = !calib.config.showBorder;
            printf("✓ Bordure: %s\n", calib.config.showBorder ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_C)) {
            calib.config.showCorners = !calib.config.showCorners;
            printf("✓ Coins: %s\n", calib.config.showCorners ? "ON" : "OFF");
        }
        
        // ═══════════════════════════════════════════
        // CALIBRATION UPDATE
        // ═══════════════════════════════════════════
        
        //  NOUVEAU : Une seule ligne suffit !
        RM_UpdateCalibration(&calib);
        
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
            
            // 2.  NOUVEAU : Afficher calibration (pass by value)
            RM_DrawCalibration(calib);
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            DrawRectangle(0, 0, 1280, 80, ColorAlpha(BLACK, 0.8f));
            DrawText("Test 08: Calibration Interactive", 20, 15, 24, LIME);
            
            //  NOUVEAU : Afficher état calibration
            if (calib.enabled) {
                DrawText("🖱️  MODE CALIBRATION - Cliquez et glissez les coins", 20, 45, 16, YELLOW);
            } else {
                DrawText("Press TAB to enter calibration mode", 20, 45, 16, GRAY);
            }
            
            //  NOUVEAU : Accès direct au membre activeCorner
            if (calib.enabled && calib.activeCorner >= 0) {
                const char *cornerNames[] = {"TOP-LEFT", "TOP-RIGHT", "BOTTOM-RIGHT", "BOTTOM-LEFT"};
                DrawText(TextFormat("Coin actif: %s", cornerNames[calib.activeCorner]), 
                        1280 - 300, 15, 18, RED);
            }
            
            // Contrôles
            DrawRectangle(0, 720 - 160, 320, 160, ColorAlpha(BLACK, 0.8f));
            DrawText("Contrôles:", 10, 720 - 150, 18, YELLOW);
            DrawText("TAB: Toggle calibration", 10, 720 - 125, 14, LIGHTGRAY);
            DrawText("CLIC: Déplacer coin", 10, 720 - 105, 14, LIGHTGRAY);
            DrawText("R: Reset", 10, 720 - 85, 14, LIGHTGRAY);
            DrawText("G: Toggle grille", 10, 720 - 65, 14, LIGHTGRAY);
            DrawText("B: Toggle bordure", 10, 720 - 45, 14, LIGHTGRAY);
            DrawText("C: Toggle coins", 10, 720 - 25, 14, LIGHTGRAY);
            
            DrawFPS(1280 - 100, 720 - 30);
            
        EndDrawing();
    }
    
    //  NOUVEAU : Pas de RM_DestroyCalibration() (struct sur stack)
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════\n");
    printf("   Test terminé\n");
    printf("═══════════════════════════════════════\n");
    
    return 0;
}
