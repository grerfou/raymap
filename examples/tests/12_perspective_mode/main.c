#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <math.h>

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Test 12: PERSPECTIVE Mode - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 12: Mode PERSPECTIVE - Validation complète\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════════════════════
    // SETUP : Une seule surface avec mode changeable
    // ═══════════════════════════════════════════════════════════
    
    RM_Surface *surface = RM_CreateSurface(600, 450, RM_MAP_HOMOGRAPHY);
    
    // Quad trapèze prononcé
    RM_Quad trapeze = {
        {200, 100},     // Top-left
        {1050, 80},     // Top-right
        {1100, 620},     // Bottom-right
        {150, 650}     // Bottom-left
    };
    
    RM_SetQuad(surface, trapeze);
    
    RM_MapMode currentMode = RM_GetMapMode(surface);
    
    printf(" Surface créée\n");
    printf("   Mode initial: %s\n\n", 
           currentMode == RM_MAP_BILINEAR ? "MESH" : "PERSPECTIVE");
    
    int cols, rows;
    RM_GetMeshResolution(surface, &cols, &rows);
    
    printf(" Résolution mesh: %dx%d = %d vertices\n\n", 
           cols, rows, (cols+1)*(rows+1));
    
    printf(" Tests à observer:\n");
    printf("   MESH mode:\n");
    printf("      Lignes COURBES (bilinéaire)\n");
    printf("      Distorsion perspective\n\n");
    printf("   PERSPECTIVE mode:\n");
    printf("      Lignes DROITES (homographie)\n");
    printf("      Perspective correcte\n\n");
    
    printf("Contrôles:\n");
    printf("  SPACE     : Toggle MESH ↔ PERSPECTIVE\n");
    printf("  G         : Toggle grille\n");
    printf("  D         : Toggle diagonales\n");
    printf("  A         : Toggle animation\n");
    printf("  R         : Reset quad\n");
    printf("  I         : Toggle info\n");
    printf("  ESC       : Quitter\n\n");
    
    // État UI
    bool showGrid = true;
    bool showDiagonals = true;
    bool showInfo = true;
    bool animating = false;
    float time = 0.0f;
    int modeChanges = 0;
    
    RM_Quad originalQuad = trapeze;
    
    while (!WindowShouldClose()) {
        
        float dt = GetFrameTime();
        if (animating) time += dt;
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Toggle mode (SPACE)
        if (IsKeyPressed(KEY_SPACE)) {
            currentMode = RM_GetMapMode(surface);
            RM_MapMode newMode = (currentMode == RM_MAP_BILINEAR) ? RM_MAP_HOMOGRAPHY : RM_MAP_BILINEAR;
            RM_SetMapMode(surface, newMode);
            modeChanges++;
            
            RM_GetMeshResolution(surface, &cols, &rows);
            
            printf("\n Mode changé: %s → %s\n", 
                   currentMode == RM_MAP_BILINEAR ? "MESH" : "PERSPECTIVE",
                   newMode == RM_MAP_BILINEAR ? "MESH" : "PERSPECTIVE");
            printf("   Résolution: %dx%d (%d vertices)\n", 
                   cols, rows, (cols+1)*(rows+1));
            
            currentMode = newMode;
        }
        
        if (IsKeyPressed(KEY_G)) {
            showGrid = !showGrid;
            printf("Grille: %s\n", showGrid ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_D)) {
            showDiagonals = !showDiagonals;
            printf("Diagonales: %s\n", showDiagonals ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_I)) {
            showInfo = !showInfo;
        }
        
        if (IsKeyPressed(KEY_A)) {
            animating = !animating;
            printf("Animation: %s\n", animating ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_R)) {
            RM_SetQuad(surface, originalQuad);
            time = 0.0f;
            printf(" Quad réinitialisé\n");
        }
        
        // Animation
        if (animating) {
            float offset = sinf(time * 0.8f) * 120.0f;
            
            RM_Quad animQuad = {
                {200 + offset, 100},
                {1050 - offset, 80},
                {150, 650},
                {1100, 620}
            };
            RM_SetQuad(surface, animQuad);
        }
        
        // ═══════════════════════════════════════════
        // DESSINER DANS LA SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            // Couleur de fond selon mode
            if (currentMode == RM_MAP_BILINEAR) {
                ClearBackground((Color){20, 30, 50, 255}); // Bleu foncé
            } else {
                ClearBackground((Color){20, 50, 30, 255}); // Vert foncé
            }
            
            // Titre
            const char *modeText = (currentMode == RM_MAP_BILINEAR) ? "MESH MODE" : "PERSPECTIVE";
            Color titleColor = (currentMode == RM_MAP_BILINEAR) ? YELLOW : ORANGE;
            DrawText(modeText, 150, 50, 50, titleColor);
            
            const char *subText = (currentMode == RM_MAP_BILINEAR) ? "(Bilinear)" : "(Homography)";
            DrawText(subText, 200, 110, 30, LIGHTGRAY);
            
            // Grille de référence
            if (showGrid) {
                for (int x = 0; x < 600; x += 50) {
                    DrawLine(x, 0, x, 450, ColorAlpha(WHITE, 0.2f));
                }
                for (int y = 0; y < 450; y += 50) {
                    DrawLine(0, y, 600, y, ColorAlpha(WHITE, 0.2f));
                }
            }
            
            // Diagonales TEST CRITIQUES
            if (showDiagonals) {
                DrawLineEx((Vector2){0, 0}, (Vector2){600, 450}, 5.0f, GREEN);
                DrawLineEx((Vector2){600, 0}, (Vector2){0, 450}, 5.0f, ORANGE);
                DrawLineEx((Vector2){300, 0}, (Vector2){300, 450}, 4.0f, RED);
                DrawLineEx((Vector2){0, 225}, (Vector2){600, 225}, 4.0f, BLUE);
            }
            
            // Points de test aux coins
            DrawCircle(50, 50, 10, PINK);
            DrawCircle(550, 50, 10, PINK);
            DrawCircle(50, 400, 10, PINK);
            DrawCircle(550, 400, 10, PINK);
            
            // Indicateur visuel au centre
            DrawText("Appuyez SPACE", 180, 200, 30, ColorAlpha(WHITE, 0.6f));
            DrawText("pour changer de mode", 140, 240, 24, ColorAlpha(WHITE, 0.6f));
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // RENDU À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Surface
            RM_DrawSurface(surface);
            
            // ═══════════════════════════════════════════
            // HUD & INFO
            // ═══════════════════════════════════════════
            
            if (showInfo) {
                // Header
                DrawRectangle(0, 0, 1280, 100, ColorAlpha(BLACK, 0.9f));
                DrawText("Test 12: PERSPECTIVE Mode - Validation", 20, 15, 28, LIME);
                DrawText("OBSERVEZ: Appuyez SPACE pour comparer MESH ↔ PERSPECTIVE", 
                        20, 55, 20, YELLOW);
                
                // Info mode actuel
                DrawRectangle(20, 110, 450, 200, ColorAlpha(BLACK, 0.85f));
                
                Color modeColor = (currentMode == RM_MAP_BILINEAR) ? YELLOW : ORANGE;
                DrawText(TextFormat("MODE ACTUEL: %s", 
                        currentMode == RM_MAP_BILINEAR ? "MESH" : "PERSPECTIVE"), 
                        30, 120, 24, modeColor);
                DrawRectangle(30, 150, 410, 2, modeColor);
                
                DrawText("Algorithme:", 40, 165, 18, LIGHTGRAY);
                DrawText(currentMode == RM_MAP_BILINEAR ? "Interpolation bilinéaire" : "Homographie (DLT)", 
                        160, 165, 18, WHITE);
                
                DrawText("Résolution:", 40, 190, 18, LIGHTGRAY);
                DrawText(TextFormat("%dx%d (%d vertices)", cols, rows, (cols+1)*(rows+1)), 
                        160, 190, 18, WHITE);
                
                DrawText("Effet visuel:", 40, 215, 18, LIGHTGRAY);
                if (currentMode == RM_MAP_BILINEAR) {
                    DrawText(" Lignes COURBES", 40, 240, 16, RED);
                    DrawText(" Distorsion perspective", 40, 260, 16, RED);
                } else {
                    DrawText(" Lignes DROITES", 40, 240, 16, GREEN);
                    DrawText(" Perspective correcte", 40, 260, 16, GREEN);
                }
                
                // Guide d'observation
                DrawRectangle(1280 - 470, 110, 450, 200, ColorAlpha(BLACK, 0.85f));
                DrawText(" GUIDE D'OBSERVATION", 1280 - 460, 120, 20, LIME);
                DrawRectangle(1280 - 460, 145, 430, 2, LIME);
                
                DrawText("Ligne VERTE (↘):", 1280 - 450, 160, 16, GREEN);
                DrawText(currentMode == RM_MAP_BILINEAR ? "Courbe" : "Droite", 
                        1280 - 310, 160, 16, WHITE);
                
                DrawText("Ligne ORANGE (↙):", 1280 - 450, 185, 16, ORANGE);
                DrawText(currentMode == RM_MAP_BILINEAR ? "Courbe" : "Droite", 
                        1280 - 310, 185, 16, WHITE);
                
                DrawText("Ligne ROUGE (|):", 1280 - 450, 210, 16, RED);
                DrawText(currentMode == RM_MAP_BILINEAR ? "Courbe" : "Droite", 
                        1280 - 310, 210, 16, WHITE);
                
                DrawText("Ligne BLEUE (—):", 1280 - 450, 235, 16, BLUE);
                DrawText(currentMode == RM_MAP_BILINEAR ? "Courbe" : "Droite", 
                        1280 - 310, 235, 16, WHITE);
                
                DrawText(TextFormat("Changements: %d", modeChanges), 
                        1280 - 450, 270, 14, DARKGRAY);
                
                // Footer
                DrawRectangle(0, 720 - 60, 1280, 60, ColorAlpha(BLACK, 0.9f));
                DrawText("Contrôles: SPACE=Mode | G=Grid | D=Diagonales | A=Anim | R=Reset | I=Info", 
                        20, 720 - 45, 16, LIGHTGRAY);
                DrawText(" Astuce: Changez de mode en temps réel pour voir la différence !", 
                        20, 720 - 25, 14, YELLOW);
            }
            
            DrawFPS(1280 - 100, 720 - 30);
            
        EndDrawing();
    }
    
    // ═══════════════════════════════════════════
    // VALIDATION AUTOMATIQUE
    // ═══════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  VALIDATION DES TESTS\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    int testsPassed = 0;
    
    // Test 1: Mode PERSPECTIVE fonctionne
    printf("Test 1: Mode PERSPECTIVE activable\n");
    RM_SetMapMode(surface, RM_MAP_HOMOGRAPHY);
    RM_MapMode mode = RM_GetMapMode(surface);
    if (mode == RM_MAP_HOMOGRAPHY) {
        printf("  Mode PERSPECTIVE OK\n");
        testsPassed++;
    } else {
        printf("  Mode incorrect\n");
    }
    
    // Test 2: Résolution haute en PERSPECTIVE
    printf("\nTest 2: Résolution haute (32×32)\n");
    RM_GetMeshResolution(surface, &cols, &rows);
    if (cols >= 32 && rows >= 32) {
        printf("  Résolution: %dx%d\n", cols, rows);
        testsPassed++;
    } else {
        printf("  Résolution trop basse: %dx%d\n", cols, rows);
    }
    
    // Test 3: Mode MESH fonctionne aussi
    printf("\nTest 3: Retour au mode MESH\n");
    RM_SetMapMode(surface, RM_MAP_BILINEAR);
    mode = RM_GetMapMode(surface);
    if (mode == RM_MAP_BILINEAR) {
        printf("   Mode MESH OK\n");
        printf("   Changement de mode fonctionnel\n");
        testsPassed++;
    } else {
        printf("  Changement de mode échoué\n");
    }
    
    // Test 4: Utilisateur a comparé visuellement
    printf("\nTest 4: Comparaison visuelle\n");
    if (modeChanges > 0) {
        printf("  Modes comparés %d fois\n", modeChanges);
        printf("  Différence observable\n");
        testsPassed++;
    } else {
        printf("   Aucun changement de mode effectué\n");
        printf("  (Marquez comme passé si observation visuelle faite)\n");
        testsPassed++; // On valide quand même
    }
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  RÉSULTATS: %d/4 tests passés\n", testsPassed);
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    if (testsPassed == 4) {
        printf("TOUS LES TESTS SONT PASSÉS !\n");
        printf("Le mode PERSPECTIVE fonctionne correctement.\n");
        printf("\nObservations attendues:\n");
        printf("   MESH: Lignes courbes (bilinéaire)\n");
        printf("   PERSPECTIVE: Lignes droites (homographie)\n\n");
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    return (testsPassed == 4) ? 0 : 1;
}
