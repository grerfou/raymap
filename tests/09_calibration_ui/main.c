#include "raylib.h"
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#include <stdio.h>

int main(void) {
    InitWindow(1280, 720, "Test 09: Calibration UI - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════\n");
    printf("  Test: Calibration UI Modulaire\n");
    printf("═══════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════
    // SETUP
    // ═══════════════════════════════════════════
    
    // Créer surface
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_BILINEAR);
    
    RM_Quad initialQuad = {
        { 200, 150 },
        { 1080, 150 },
        { 1080, 570 },
        { 200, 570 }
    };
    RM_SetQuad(surface, initialQuad);
    
    //  NOUVEAU : Calibration sur stack
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    //  NOUVEAU : Accès direct à la config
    calib.config.cornerRadius = 20.0f;
    calib.config.gridResolutionX = 10;
    calib.config.gridResolutionY = 8;
    
    printf("✓ Surface et calibration créées\n\n");
    
    // Modes d'affichage
    typedef enum {
        MODE_ALL = 0,
        MODE_CORNERS_ONLY,
        MODE_BORDER_ONLY,
        MODE_GRID_ONLY,
        MODE_CUSTOM
    } DrawMode;
    
    DrawMode currentMode = MODE_ALL;
    const char *modeNames[] = {
        "TOUT (complet)",
        "COINS seulement",
        "BORDURE seulement",
        "GRILLE seulement",
        "CUSTOM (personnalisé)"
    };
    
    printf("Contrôles:\n");
    printf("  CLIC       : Drag corners\n");
    printf("  TAB        : Toggle calibration mode\n");
    printf("  1-5        : Modes d'affichage\n");
    printf("  R          : Reset quad\n");
    printf("  C/B/G      : Toggle éléments (mode CUSTOM)\n");
    printf("  +/-        : Taille coins\n");
    printf("  UP/DOWN    : Résolution grille\n");
    printf("  ESC        : Quitter\n\n");
    
    // ═══════════════════════════════════════════
    // MAIN LOOP
    // ═══════════════════════════════════════════
    
    while (!WindowShouldClose()) {
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        //  NOUVEAU : Toggle calibration
        if (IsKeyPressed(KEY_TAB)) {
            RM_ToggleCalibration(&calib);
            printf("✓ Calibration: %s\n", calib.enabled ? "ON" : "OFF");
        }
        
        // Modes d'affichage (seulement si calibration active)
        if (calib.enabled) {
            if (IsKeyPressed(KEY_ONE)) {
                currentMode = MODE_ALL;
                calib.config.showCorners = true;
                calib.config.showBorder = true;
                calib.config.showGrid = true;
                printf("✓ Mode: %s\n", modeNames[currentMode]);
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentMode = MODE_CORNERS_ONLY;
                calib.config.showCorners = true;
                calib.config.showBorder = false;
                calib.config.showGrid = false;
                printf("✓ Mode: %s\n", modeNames[currentMode]);
            }
            if (IsKeyPressed(KEY_THREE)) {
                currentMode = MODE_BORDER_ONLY;
                calib.config.showCorners = false;
                calib.config.showBorder = true;
                calib.config.showGrid = false;
                printf("✓ Mode: %s\n", modeNames[currentMode]);
            }
            if (IsKeyPressed(KEY_FOUR)) {
                currentMode = MODE_GRID_ONLY;
                calib.config.showCorners = false;
                calib.config.showBorder = false;
                calib.config.showGrid = true;
                printf("✓ Mode: %s\n", modeNames[currentMode]);
            }
            if (IsKeyPressed(KEY_FIVE)) {
                currentMode = MODE_CUSTOM;
                printf("✓ Mode: %s\n", modeNames[currentMode]);
            }
            
            // Toggles en mode CUSTOM
            if (currentMode == MODE_CUSTOM) {
                if (IsKeyPressed(KEY_C)) {
                    calib.config.showCorners = !calib.config.showCorners;
                    printf("  Coins: %s\n", calib.config.showCorners ? "ON" : "OFF");
                }
                if (IsKeyPressed(KEY_B)) {
                    calib.config.showBorder = !calib.config.showBorder;
                    printf("  Bordure: %s\n", calib.config.showBorder ? "ON" : "OFF");
                }
                if (IsKeyPressed(KEY_G)) {
                    calib.config.showGrid = !calib.config.showGrid;
                    printf("  Grille: %s\n", calib.config.showGrid ? "ON" : "OFF");
                }
            }
            
            // Taille des coins
            if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) {
                calib.config.cornerRadius += 2.0f;
                if (calib.config.cornerRadius > 30.0f) calib.config.cornerRadius = 30.0f;
                printf("✓ Taille coins: %.0f\n", calib.config.cornerRadius);
            }
            if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) {
                calib.config.cornerRadius -= 2.0f;
                if (calib.config.cornerRadius < 5.0f) calib.config.cornerRadius = 5.0f;
                printf("✓ Taille coins: %.0f\n", calib.config.cornerRadius);
            }
            
            // Résolution de la grille
            if (IsKeyPressed(KEY_UP)) {
                calib.config.gridResolutionX++;
                calib.config.gridResolutionY++;
                if (calib.config.gridResolutionX > 20) calib.config.gridResolutionX = 20;
                if (calib.config.gridResolutionY > 20) calib.config.gridResolutionY = 20;
                printf("✓ Grille: %dx%d\n", calib.config.gridResolutionX, calib.config.gridResolutionY);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                calib.config.gridResolutionX--;
                calib.config.gridResolutionY--;
                if (calib.config.gridResolutionX < 2) calib.config.gridResolutionX = 2;
                if (calib.config.gridResolutionY < 2) calib.config.gridResolutionY = 2;
                printf("✓ Grille: %dx%d\n", calib.config.gridResolutionX, calib.config.gridResolutionY);
            }
        }
        
        // Reset (toujours actif)
        if (IsKeyPressed(KEY_R)) {
            RM_SetQuad(surface, initialQuad);
            printf("✓ Quad reset\n");
        }
        
        // ═══════════════════════════════════════════
        // UPDATE CALIBRATION
        // ═══════════════════════════════════════════
        
        //  NOUVEAU : Une seule ligne
        RM_UpdateCalibration(&calib);
        
        // ═══════════════════════════════════════════
        // DESSINER SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("UI TEST", 150, 80, 60, WHITE);
            if (calib.enabled) {
                DrawText(modeNames[currentMode], 100, 160, 24, YELLOW);
            } else {
                DrawText("Press TAB", 160, 160, 24, GRAY);
            }
            
            // Contenu de test
            for (int x = 0; x < 500; x += 50) {
                DrawLine(x, 0, x, 400, ColorAlpha(WHITE, 0.15f));
            }
            for (int y = 0; y < 400; y += 50) {
                DrawLine(0, y, 500, y, ColorAlpha(WHITE, 0.15f));
            }
            
            DrawCircle(250, 250, 60, RED);
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // DESSINER À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Surface
            RM_DrawSurface(surface);
            
            //  UI de calibration (modulaire - ne dessine rien si disabled)
            if (currentMode == MODE_ALL) {
                RM_DrawCalibration(calib);  // Tout en une fois
            } else {
                // Appeler individuellement
                if (calib.config.showBorder) RM_DrawCalibrationBorder(calib);
                if (calib.config.showGrid) RM_DrawCalibrationGrid(calib);
                if (calib.config.showCorners) RM_DrawCalibrationCorners(calib);
            }
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            DrawRectangle(0, 0, 1280, 100, ColorAlpha(BLACK, 0.85f));
            DrawText("Test 09: Calibration UI Modulaire", 20, 15, 24, LIME);
            
            if (calib.enabled) {
                DrawText(TextFormat("Mode: %s", modeNames[currentMode]), 20, 45, 18, YELLOW);
                DrawText("Press TAB to exit calibration mode", 20, 70, 16, GRAY);
            } else {
                DrawText("Press TAB to enter calibration mode", 20, 45, 18, GRAY);
            }
            
            //  Info configuration (seulement si calibration active)
            if (calib.enabled) {
                DrawRectangle(1280 - 400, 0, 400, 200, ColorAlpha(BLACK, 0.85f));
                DrawText("CONFIGURATION:", 1280 - 390, 15, 18, WHITE);
                DrawText(TextFormat("Coins: %s", calib.config.showCorners ? "ON" : "OFF"), 
                        1280 - 390, 45, 16, calib.config.showCorners ? GREEN : RED);
                DrawText(TextFormat("Bordure: %s", calib.config.showBorder ? "ON" : "OFF"), 
                        1280 - 390, 70, 16, calib.config.showBorder ? GREEN : RED);
                DrawText(TextFormat("Grille: %s", calib.config.showGrid ? "ON" : "OFF"), 
                        1280 - 390, 95, 16, calib.config.showGrid ? GREEN : RED);
                
                DrawText(TextFormat("Taille coins: %.0f", calib.config.cornerRadius), 
                        1280 - 390, 125, 14, LIGHTGRAY);
                DrawText(TextFormat("Grille: %dx%d", calib.config.gridResolutionX, calib.config.gridResolutionY), 
                        1280 - 390, 145, 14, LIGHTGRAY);
                
                //  Accès direct au membre
                if (calib.activeCorner >= 0) {
                    DrawText(TextFormat("Coin actif: %d", calib.activeCorner), 
                            1280 - 390, 170, 14, RED);
                }
            }
            
            // Contrôles
            DrawRectangle(0, 720 - 200, 400, 200, ColorAlpha(BLACK, 0.85f));
            DrawText("Contrôles:", 10, 720 - 190, 18, YELLOW);
            DrawText("TAB: Toggle calibration", 10, 720 - 165, 14, LIGHTGRAY);
            
            if (calib.enabled) {
                DrawText("1-5: Modes d'affichage", 10, 720 - 145, 14, LIGHTGRAY);
                DrawText("C/B/G: Toggle (mode 5)", 10, 720 - 125, 14, LIGHTGRAY);
                DrawText("+/-: Taille coins", 10, 720 - 105, 14, LIGHTGRAY);
                DrawText("UP/DOWN: Résolution grille", 10, 720 - 85, 14, LIGHTGRAY);
                DrawText("CLIC: Drag corners", 10, 720 - 65, 14, LIGHTGRAY);
            }
            
            DrawText("R: Reset", 10, 720 - 45, 14, LIGHTGRAY);
            
            DrawFPS(1280 - 100, 720 - 30);
            
        EndDrawing();
    }
    
    //  NOUVEAU : Pas de destroy pour calibration
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════\n");
    printf("   Test terminé\n");
    printf("═══════════════════════════════════════\n");
    
    return 0;
}
