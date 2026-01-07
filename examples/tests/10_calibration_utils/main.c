#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Test 10: Calibration Utilities - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════\n");
    printf("  Test: Calibration Utilities\n");
    printf("═══════════════════════════════════════\n\n");
    
    // Créer surface et calibration
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_MESH);
    RM_Calibration *calibration = RM_CreateCalibration(surface);
    
    // Test 1 : Reset initial (centré)
    printf("Test 1: Reset quad (centré)\n");
    RM_ResetQuad(surface, screenWidth, screenHeight);
    RM_Quad quad = RM_GetQuad(surface);
    printf("   TL: (%.0f, %.0f)\n", quad.topLeft.x, quad.topLeft.y);
    printf("   TR: (%.0f, %.0f)\n", quad.topRight.x, quad.topRight.y);
    printf("   BL: (%.0f, %.0f)\n", quad.bottomLeft.x, quad.bottomLeft.y);
    printf("   BR: (%.0f, %.0f)\n\n", quad.bottomRight.x, quad.bottomRight.y);
    
    // Test 2 : GetActiveCorner (devrait être -1)
    printf("Test 2: GetActiveCorner (initial)\n");
    int activeCorner = RM_GetActiveCorner(calibration);
    printf("   Coin actif: %d (attendu: -1)\n\n", activeCorner);
    
    // Test 3 : IsCalibrating (devrait être false)
    printf("Test 3: IsCalibrating (initial)\n");
    bool isCalibrating = RM_IsCalibrate(calibration);
    printf("   En calibration: %s (attendu: false)\n\n", isCalibrating ? "true" : "false");
    
    printf("Contrôles:\n");
    printf("  CLIC       : Drag corners\n");
    printf("  R          : Reset quad (centré)\n");
    printf("  T          : Reset quad (plein écran)\n");
    printf("  C          : Afficher état calibration\n");
    printf("  ESC        : Quitter\n\n");
    
    // Variables pour tracking
    int lastActiveCorner = -1;
    bool wasCalibrating = false;
    int calibrationCount = 0;
    
    while (!WindowShouldClose()) {
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Reset centré
        if (IsKeyPressed(KEY_R)) {
            RM_ResetQuad(surface, screenWidth, screenHeight);
            printf(" Reset: Quad centré\n");
            
            RM_Quad q = RM_GetQuad(surface);
            printf("   TL: (%.0f, %.0f)\n", q.topLeft.x, q.topLeft.y);
        }
        
        // Reset plein écran
        if (IsKeyPressed(KEY_T)) {
            RM_Quad fullscreen = {
                { 0, 0 },
                { (float)screenWidth, 0 },
                { 0, (float)screenHeight },
                { (float)screenWidth, (float)screenHeight }
            };
            RM_SetQuad(surface, fullscreen);
            printf(" Reset: Plein écran\n");
        }
        
        // Afficher état calibration
        if (IsKeyPressed(KEY_C)) {
            int corner = RM_GetActiveCorner(calibration);
            bool calibrating = RM_IsCalibrate(calibration);
            
            printf("\n État calibration:\n");
            printf("   Coin actif: %d\n", corner);
            printf("   En calibration: %s\n", calibrating ? "OUI" : "NON");
            
            if (corner >= 0) {
                const char *cornerNames[] = {"TOP-LEFT", "TOP-RIGHT", "BOTTOM-RIGHT", "BOTTOM-LEFT"};
                printf("   Nom coin: %s\n\n", cornerNames[corner]);
            }
        }
        
        // ═══════════════════════════════════════════
        // UPDATE CALIBRATION
        // ═══════════════════════════════════════════
        
        RM_BeginCalibration(calibration);
        RM_UpdateCalibration(calibration);
        RM_EndCalibration(calibration);
        
        // ═══════════════════════════════════════════
        // TRACKING (pour tests automatiques)
        // ═══════════════════════════════════════════
        
        int currentActiveCorner = RM_GetActiveCorner(calibration);
        bool currentlyCalibrating = RM_IsCalibrate(calibration);
        
        // Test 4 : Détecter sélection de coin
        if (currentActiveCorner != lastActiveCorner) {
            if (currentActiveCorner >= 0) {
                const char *cornerNames[] = {"TOP-LEFT", "TOP-RIGHT", "BOTTOM-RIGHT", "BOTTOM-LEFT"};
                printf(" Test 4: Coin sélectionné → %s (index: %d)\n", 
                       cornerNames[currentActiveCorner], currentActiveCorner);
            } else if (lastActiveCorner >= 0) {
                printf(" Test 4: Coin désélectionné (index: %d → -1)\n", lastActiveCorner);
            }
            lastActiveCorner = currentActiveCorner;
        }
        
        // Test 5 : Détecter début/fin de calibration
        if (currentlyCalibrating != wasCalibrating) {
            if (currentlyCalibrating) {
                calibrationCount++;
                printf(" Test 5: Début calibration (drag) #%d\n", calibrationCount);
            } else {
                printf(" Test 5: Fin calibration (relâché)\n");
            }
            wasCalibrating = currentlyCalibrating;
        }
        
        // ═══════════════════════════════════════════
        // DESSINER SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("UTILITIES", 120, 80, 60, WHITE);
            DrawText("TEST", 200, 160, 40, YELLOW);
            
            // Grille
            for (int x = 0; x < 500; x += 50) {
                DrawLine(x, 0, x, 400, ColorAlpha(WHITE, 0.15f));
            }
            for (int y = 0; y < 400; y += 50) {
                DrawLine(0, y, 500, y, ColorAlpha(WHITE, 0.15f));
            }
            
            // Cercle
            DrawCircle(250, 250, 60, RED);
            
            // Info calibration
            DrawText(TextFormat("Calibrations: %d", calibrationCount), 20, 350, 20, LIGHTGRAY);
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // DESSINER À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Surface
            RM_DrawSurface(surface);
            
            // UI de calibration
            RM_DrawCalibration(calibration);
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            DrawRectangle(0, 0, 1280, 80, ColorAlpha(BLACK, 0.85f));
            DrawText("Test 10: Calibration Utilities", 20, 15, 24, LIME);
            DrawText("🛠️  Fonctions helper pour calibration", 20, 45, 16, LIGHTGRAY);
            
            // État en temps réel
            DrawRectangle(1280 - 350, 0, 350, 180, ColorAlpha(BLACK, 0.85f));
            DrawText("ÉTAT EN TEMPS RÉEL:", 1280 - 340, 15, 18, WHITE);
            
            int corner = RM_GetActiveCorner(calibration);
            bool calibrating = RM_IsCalibrate(calibration);
            
            DrawText(TextFormat("Coin actif: %d", corner), 1280 - 340, 45, 16, 
                    corner >= 0 ? YELLOW : GRAY);
            
            if (corner >= 0) {
                const char *cornerNames[] = {"TL", "TR", "BR", "BL"};
                DrawText(TextFormat("(%s)", cornerNames[corner]), 1280 - 180, 45, 16, YELLOW);
            }
            
            DrawText(TextFormat("IsCalibrating: %s", calibrating ? "YES" : "NO"), 
                    1280 - 340, 70, 16, calibrating ? GREEN : RED);
            
            DrawText(TextFormat("Total drags: %d", calibrationCount), 1280 - 340, 95, 14, LIGHTGRAY);
            
            // Position souris
            Vector2 mousePos = GetMousePosition();
            DrawText(TextFormat("Mouse: (%.0f, %.0f)", mousePos.x, mousePos.y), 
                    1280 - 340, 120, 14, DARKGRAY);
            
            // Indicateur visuel de calibration
            if (calibrating) {
                DrawRectangle(1280 - 340, 145, 320, 25, ColorAlpha(GREEN, 0.3f));
                DrawText(" CALIBRATION EN COURS", 1280 - 335, 150, 14, GREEN);
            }
            
            // Contrôles
            DrawRectangle(0, 720 - 140, 350, 140, ColorAlpha(BLACK, 0.85f));
            DrawText("Contrôles:", 10, 720 - 130, 18, YELLOW);
            DrawText("CLIC: Drag corners", 10, 720 - 105, 14, LIGHTGRAY);
            DrawText("R: Reset (centré)", 10, 720 - 85, 14, LIGHTGRAY);
            DrawText("T: Reset (plein écran)", 10, 720 - 65, 14, LIGHTGRAY);
            DrawText("C: Afficher état", 10, 720 - 45, 14, LIGHTGRAY);
            
            DrawFPS(1280 - 100, 720 - 30);
            
        EndDrawing();
    }
    
    RM_DestroyCalibration(calibration);
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════\n");
    printf("   Test terminé\n");
    printf("═══════════════════════════════════════\n");
    printf("\nRésumé des tests:\n");
    printf("   Test 1: Reset quad (centré)\n");
    printf("   Test 2: GetActiveCorner initial (-1)\n");
    printf("   Test 3: IsCalibrating initial (false)\n");
    printf("   Test 4: Sélection/désélection de coin\n");
    printf("   Test 5: Début/fin de calibration\n");
    printf("   Total calibrations: %d\n\n", calibrationCount);
    
    return 0;
}
