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
    
    // Créer surface et calibration
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_MESH);
    
    RM_Quad initialQuad = {
        { 200, 150 },
        { 1080, 150 },
        { 200, 570 },
        { 1080, 570 }
    };
    RM_SetQuad(surface, initialQuad);
    
    RM_Calibration *calibration = RM_CreateCalibration(surface);
    
    // Personnaliser les couleurs
    RM_CalibrationConfig *config = RM_GetCalibrationConfig(calibration);
    config->cornerRadius = 20.0f;
    config->gridResolutionX = 10;
    config->gridResolutionY = 8;
    
    printf(" Surface et calibration créées\n\n");
    
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
    printf("  1-5        : Modes d'affichage\n");
    printf("  R          : Reset quad\n");
    printf("  C/B/G      : Toggle éléments (mode CUSTOM)\n");
    printf("  +/-        : Taille coins\n");
    printf("  UP/DOWN    : Résolution grille\n");
    printf("  ESC        : Quitter\n\n");
    
    while (!WindowShouldClose()) {
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Modes d'affichage
        if (IsKeyPressed(KEY_ONE)) {
            currentMode = MODE_ALL;
            config->showCorners = true;
            config->showBorder = true;
            config->showGrid = true;
            printf(" Mode: %s\n", modeNames[currentMode]);
        }
        if (IsKeyPressed(KEY_TWO)) {
            currentMode = MODE_CORNERS_ONLY;
            config->showCorners = true;
            config->showBorder = false;
            config->showGrid = false;
            printf(" Mode: %s\n", modeNames[currentMode]);
        }
        if (IsKeyPressed(KEY_THREE)) {
            currentMode = MODE_BORDER_ONLY;
            config->showCorners = false;
            config->showBorder = true;
            config->showGrid = false;
            printf(" Mode: %s\n", modeNames[currentMode]);
        }
        if (IsKeyPressed(KEY_FOUR)) {
            currentMode = MODE_GRID_ONLY;
            config->showCorners = false;
            config->showBorder = false;
            config->showGrid = true;
            printf(" Mode: %s\n", modeNames[currentMode]);
        }
        if (IsKeyPressed(KEY_FIVE)) {
            currentMode = MODE_CUSTOM;
            printf(" Mode: %s\n", modeNames[currentMode]);
        }
        
        // Toggles en mode CUSTOM
        if (currentMode == MODE_CUSTOM) {
            if (IsKeyPressed(KEY_C)) {
                config->showCorners = !config->showCorners;
                printf("   Coins: %s\n", config->showCorners ? "ON" : "OFF");
            }
            if (IsKeyPressed(KEY_B)) {
                config->showBorder = !config->showBorder;
                printf("   Bordure: %s\n", config->showBorder ? "ON" : "OFF");
            }
            if (IsKeyPressed(KEY_G)) {
                config->showGrid = !config->showGrid;
                printf("   Grille: %s\n", config->showGrid ? "ON" : "OFF");
            }
        }
        
        // Taille des coins
        if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) {
            config->cornerRadius += 2.0f;
            if (config->cornerRadius > 30.0f) config->cornerRadius = 30.0f;
            printf(" Taille coins: %.0f\n", config->cornerRadius);
        }
        if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) {
            config->cornerRadius -= 2.0f;
            if (config->cornerRadius < 5.0f) config->cornerRadius = 5.0f;
            printf(" Taille coins: %.0f\n", config->cornerRadius);
        }
        
        // Résolution de la grille
        if (IsKeyPressed(KEY_UP)) {
            config->gridResolutionX++;
            config->gridResolutionY++;
            if (config->gridResolutionX > 20) config->gridResolutionX = 20;
            if (config->gridResolutionY > 20) config->gridResolutionY = 20;
            printf(" Grille: %dx%d\n", config->gridResolutionX, config->gridResolutionY);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            config->gridResolutionX--;
            config->gridResolutionY--;
            if (config->gridResolutionX < 2) config->gridResolutionX = 2;
            if (config->gridResolutionY < 2) config->gridResolutionY = 2;
            printf(" Grille: %dx%d\n", config->gridResolutionX, config->gridResolutionY);
        }
        
        // Reset
        if (IsKeyPressed(KEY_R)) {
            RM_SetQuad(surface, initialQuad);
            printf(" Quad reset\n");
        }
        
        // ═══════════════════════════════════════════
        // UPDATE CALIBRATION
        // ═══════════════════════════════════════════
        
        RM_BeginCalibration(calibration);
        RM_UpdateCalibration(calibration);
        RM_EndCalibration(calibration);
        
        // ═══════════════════════════════════════════
        // DESSINER SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("UI TEST", 150, 80, 60, WHITE);
            DrawText(modeNames[currentMode], 100, 160, 24, YELLOW);
            
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
            
            // UI de calibration (modulaire)
            if (currentMode == MODE_ALL) {
                RM_DrawCalibration(calibration);  // Tout en une fois
            } else {
                // Appeler individuellement
                if (config->showBorder) RM_DrawCalibrationBorder(calibration);
                if (config->showGrid) RM_DrawCalibrationGrid(calibration);
                if (config->showCorners) RM_DrawCalibrationCorners(calibration);
            }
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            DrawRectangle(0, 0, 1280, 80, ColorAlpha(BLACK, 0.85f));
            DrawText("Test 09: Calibration UI Modulaire", 20, 15, 24, LIME);
            DrawText(TextFormat("Mode: %s", modeNames[currentMode]), 20, 45, 18, YELLOW);
            
            // Info configuration
            DrawRectangle(1280 - 400, 0, 400, 200, ColorAlpha(BLACK, 0.85f));
            DrawText("CONFIGURATION:", 1280 - 390, 15, 18, WHITE);
            DrawText(TextFormat("Coins: %s", config->showCorners ? "ON" : "OFF"), 
                    1280 - 390, 45, 16, config->showCorners ? GREEN : RED);
            DrawText(TextFormat("Bordure: %s", config->showBorder ? "ON" : "OFF"), 
                    1280 - 390, 70, 16, config->showBorder ? GREEN : RED);
            DrawText(TextFormat("Grille: %s", config->showGrid ? "ON" : "OFF"), 
                    1280 - 390, 95, 16, config->showGrid ? GREEN : RED);
            
            DrawText(TextFormat("Taille coins: %.0f", config->cornerRadius), 
                    1280 - 390, 125, 14, LIGHTGRAY);
            DrawText(TextFormat("Grille: %dx%d", config->gridResolutionX, config->gridResolutionY), 
                    1280 - 390, 145, 14, LIGHTGRAY);
            
            if (calibration->activeCorner >= 0) {
                DrawText(TextFormat("Coin actif: %d", calibration->activeCorner), 
                        1280 - 390, 170, 14, RED);
            }
            
            // Contrôles
            DrawRectangle(0, 720 - 180, 400, 180, ColorAlpha(BLACK, 0.85f));
            DrawText("Contrôles:", 10, 720 - 170, 18, YELLOW);
            DrawText("1-5: Modes d'affichage", 10, 720 - 145, 14, LIGHTGRAY);
            DrawText("C/B/G: Toggle (mode 5)", 10, 720 - 125, 14, LIGHTGRAY);
            DrawText("+/-: Taille coins", 10, 720 - 105, 14, LIGHTGRAY);
            DrawText("UP/DOWN: Résolution grille", 10, 720 - 85, 14, LIGHTGRAY);
            DrawText("R: Reset", 10, 720 - 65, 14, LIGHTGRAY);
            DrawText("CLIC: Drag corners", 10, 720 - 45, 14, LIGHTGRAY);
            
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
