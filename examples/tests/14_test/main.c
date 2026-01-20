#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "Test 15: Interactive Calibration with Save/Load");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 15: Interactive Calibration with Persistence\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════════════════════
    // SETUP
    // ═══════════════════════════════════════════════════════════
    
    const char *configFile = "../calibration.txt";
    
    // Créer surface
    RM_Surface *surface = RM_CreateSurface(1280, 720, RM_MAP_HOMOGRAPHY);
    
    // Essayer de charger la config sauvegardée
    bool configLoaded = RM_LoadConfig(surface, configFile);
    
    if (configLoaded) {
        printf("✅ Configuration chargée depuis '%s'\n", configFile);
        
        RM_Quad quad = RM_GetQuad(surface);
        printf("   TL: (%.0f, %.0f)  TR: (%.0f, %.0f)\n", 
               quad.topLeft.x, quad.topLeft.y, quad.topRight.x, quad.topRight.y);
        printf("   BL: (%.0f, %.0f)  BR: (%.0f, %.0f)\n\n", 
               quad.bottomLeft.x, quad.bottomLeft.y, quad.bottomRight.x, quad.bottomRight.y);
    } else {
        printf("ℹ️  Aucune config trouvée, utilisation du quad par défaut\n");
        
        // Quad par défaut centré
        RM_Quad defaultQuad = {
            {320, 180},
            {1600, 180},
            {320, 900},
            {1600, 900}
        };
        RM_SetQuad(surface, defaultQuad);
        printf("   Quad centré créé\n\n");
    }
    
    // Créer la calibration
    RM_Calibration *calibration = RM_CreateCalibration(surface);
    
    // Configuration visuelle
    RM_CalibrationConfig *config = RM_GetCalibrationConfig(calibration);
    config->cornerRadius = 20.0f;
    config->gridResolutionX = 16;
    config->gridResolutionY = 9;
    
    printf("Contrôles:\n");
    printf("  DRAG     : Déplacer les coins (0-1-2-3)\n");
    printf("  S        : Sauvegarder la configuration\n");
    printf("  L        : Recharger depuis le fichier\n");
    printf("  R        : Reset au quad par défaut\n");
    printf("  G        : Toggle grille\n");
    printf("  C        : Toggle coins\n");
    printf("  B        : Toggle bordure\n");
    printf("  M        : Changer mode (BILINEAR/HOMOGRAPHY)\n");
    printf("  +/-      : Ajuster résolution mesh\n");
    printf("  ESC      : Quitter (sauvegarde auto)\n\n");
    
    bool showInfo = true;
    bool autoSaved = false;
    float saveMessageTimer = 0.0f;
    const float SAVE_MESSAGE_DURATION = 2.0f;
    char statusMessage[128] = "";
    
    // ═══════════════════════════════════════════════════════════
    // BOUCLE PRINCIPALE
    // ═══════════════════════════════════════════════════════════
    
    while (!WindowShouldClose()) {
        
        float dt = GetFrameTime();
        
        // Décrémenter le timer de message
        if (saveMessageTimer > 0.0f) {
            saveMessageTimer -= dt;
        }
        
        // ═══════════════════════════════════════════
        // CALIBRATION
        // ═══════════════════════════════════════════
        
        RM_UpdateCalibration(calibration);
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Sauvegarder
        if (IsKeyPressed(KEY_S)) {
            if (RM_SaveConfig(surface, configFile)) {
                sprintf(statusMessage, "✅ Configuration sauvegardée !");
                saveMessageTimer = SAVE_MESSAGE_DURATION;
            } else {
                sprintf(statusMessage, "❌ Erreur de sauvegarde");
                saveMessageTimer = SAVE_MESSAGE_DURATION;
            }
        }
        
        // Recharger
        if (IsKeyPressed(KEY_L)) {
            if (RM_LoadConfig(surface, configFile)) {
                sprintf(statusMessage, "✅ Configuration rechargée !");
                saveMessageTimer = SAVE_MESSAGE_DURATION;
            } else {
                sprintf(statusMessage, "❌ Fichier introuvable");
                saveMessageTimer = SAVE_MESSAGE_DURATION;
            }
        }
        
        // Reset
        if (IsKeyPressed(KEY_R)) {
            RM_ResetQuad(surface, screenWidth, screenHeight);
            sprintf(statusMessage, "🔄 Quad réinitialisé");
            saveMessageTimer = SAVE_MESSAGE_DURATION;
        }
        
        // Toggle grille
        if (IsKeyPressed(KEY_G)) {
            config->showGrid = !config->showGrid;
        }
        
        // Toggle coins
        if (IsKeyPressed(KEY_C)) {
            config->showCorners = !config->showCorners;
        }
        
        // Toggle bordure
        if (IsKeyPressed(KEY_B)) {
            config->showBorder = !config->showBorder;
        }
        
        // Toggle info
        if (IsKeyPressed(KEY_I)) {
            showInfo = !showInfo;
        }
        
        // Changer mode
        if (IsKeyPressed(KEY_M)) {
            RM_MapMode currentMode = RM_GetMapMode(surface);
            RM_MapMode newMode = (currentMode == RM_MAP_BILINEAR) ? RM_MAP_HOMOGRAPHY : RM_MAP_BILINEAR;
            RM_SetMapMode(surface, newMode);
            
            sprintf(statusMessage, "Mode: %s", newMode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY");
            saveMessageTimer = SAVE_MESSAGE_DURATION;
        }
        
        // Ajuster résolution
        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
            int cols, rows;
            RM_GetMeshResolution(surface, &cols, &rows);
            RM_SetMeshResolution(surface, cols + 4, rows + 4);
            
            sprintf(statusMessage, "Résolution: %dx%d", cols + 4, rows + 4);
            saveMessageTimer = SAVE_MESSAGE_DURATION;
        }
        
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            int cols, rows;
            RM_GetMeshResolution(surface, &cols, &rows);
            RM_SetMeshResolution(surface, cols - 4, rows - 4);
            
            sprintf(statusMessage, "Résolution: %dx%d", cols - 4, rows - 4);
            saveMessageTimer = SAVE_MESSAGE_DURATION;
        }
        
        // ═══════════════════════════════════════════
        // RENDU SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground((Color){20, 25, 35, 255});
            
            // Titre
            DrawText("PROJECTION MAPPING", 420, 280, 80, RAYWHITE);
            DrawText("Interactive Calibration", 480, 380, 40, LIGHTGRAY);
            
            // Grille de fond
            for (int x = 0; x < 1280; x += 80) {
                DrawLine(x, 0, x, 720, ColorAlpha(WHITE, 0.1f));
            }
            for (int y = 0; y < 720; y += 80) {
                DrawLine(0, y, 1280, y, ColorAlpha(WHITE, 0.1f));
            }
            
            // Diagonales de test
            DrawLineEx((Vector2){0, 0}, (Vector2){1280, 720}, 5, GREEN);
            DrawLineEx((Vector2){1280, 0}, (Vector2){0, 720}, 5, ORANGE);
            
            // Croix centrale
            DrawLineEx((Vector2){640, 0}, (Vector2){640, 720}, 3, RED);
            DrawLineEx((Vector2){0, 360}, (Vector2){1280, 360}, 3, BLUE);
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // AFFICHAGE
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Dessiner la surface
            RM_DrawSurface(surface);
            
            // Dessiner la calibration par-dessus
            RM_DrawCalibration(calibration);
            
            // Interface
            if (showInfo) {
                // Header
                DrawRectangle(0, 0, screenWidth, 100, ColorAlpha(BLACK, 0.8f));
                DrawText("Test 15: Interactive Calibration", 20, 15, 30, LIME);
                DrawText("Drag corners • S=Save • L=Load • R=Reset • M=Mode • ESC=Quit", 20, 55, 20, YELLOW);
                
                // Info panel
                DrawRectangle(20, 120, 400, 280, ColorAlpha(BLACK, 0.85f));
                
                int yPos = 135;
                DrawText("CONFIGURATION", 35, yPos, 20, LIGHTGRAY);
                yPos += 35;
                
                RM_MapMode mode = RM_GetMapMode(surface);
                DrawText(TextFormat("Mode: %s", mode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY"), 
                         35, yPos, 18, WHITE);
                yPos += 30;
                
                int cols, rows;
                RM_GetMeshResolution(surface, &cols, &rows);
                DrawText(TextFormat("Mesh: %dx%d (%d vertices)", cols, rows, (cols+1)*(rows+1)), 
                         35, yPos, 18, WHITE);
                yPos += 30;
                
                RM_Quad quad = RM_GetQuad(surface);
                DrawText("Quad corners:", 35, yPos, 18, LIGHTGRAY);
                yPos += 25;
                DrawText(TextFormat("  TL: (%.0f, %.0f)", quad.topLeft.x, quad.topLeft.y), 
                         35, yPos, 16, GRAY);
                yPos += 20;
                DrawText(TextFormat("  TR: (%.0f, %.0f)", quad.topRight.x, quad.topRight.y), 
                         35, yPos, 16, GRAY);
                yPos += 20;
                DrawText(TextFormat("  BL: (%.0f, %.0f)", quad.bottomLeft.x, quad.bottomLeft.y), 
                         35, yPos, 16, GRAY);
                yPos += 20;
                DrawText(TextFormat("  BR: (%.0f, %.0f)", quad.bottomRight.x, quad.bottomRight.y), 
                         35, yPos, 16, GRAY);
                yPos += 30;
                
                int activeCorner = RM_GetActiveCorner(calibration);
                if (activeCorner >= 0) {
                    DrawText(TextFormat("✋ Dragging corner %d", activeCorner), 
                             35, yPos, 18, YELLOW);
                } else {
                    DrawText("Calibration ready", 35, yPos, 18, GREEN);
                }
                
                // Toggles
                DrawRectangle(20, 420, 400, 120, ColorAlpha(BLACK, 0.85f));
                DrawText("DISPLAY", 35, 435, 20, LIGHTGRAY);
                DrawText(TextFormat("[G] Grid:    %s", config->showGrid ? "ON " : "OFF"), 
                         35, 465, 16, config->showGrid ? GREEN : GRAY);
                DrawText(TextFormat("[C] Corners: %s", config->showCorners ? "ON " : "OFF"), 
                         35, 490, 16, config->showCorners ? GREEN : GRAY);
                DrawText(TextFormat("[B] Border:  %s", config->showBorder ? "ON " : "OFF"), 
                         35, 515, 16, config->showBorder ? GREEN : GRAY);
            }
            
            // Message de statut
            if (saveMessageTimer > 0.0f) {
                float alpha = saveMessageTimer / SAVE_MESSAGE_DURATION;
                DrawRectangle(screenWidth/2 - 250, 50, 500, 60, ColorAlpha(BLACK, 0.9f * alpha));
                DrawText(statusMessage, screenWidth/2 - 230, 65, 30, ColorAlpha(LIME, alpha));
            }
            
            // FPS
            DrawFPS(screenWidth - 100, 10);
            
        EndDrawing();
    }
    
    // ═══════════════════════════════════════════════════════════
    // AUTO-SAVE AU QUIT
    // ═══════════════════════════════════════════════════════════
    
    printf("\n🔄 Sauvegarde automatique avant de quitter...\n");
    
    if (RM_SaveConfig(surface, configFile)) {
        printf("✅ Configuration sauvegardée dans '%s'\n", configFile);
        autoSaved = true;
    } else {
        printf("❌ Échec de la sauvegarde automatique\n");
    }
    
    // Cleanup
    RM_DestroyCalibration(calibration);
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  Session terminée\n");
    printf("═══════════════════════════════════════════════════════════\n");
    
    if (autoSaved) {
        printf("\n💾 Configuration préservée pour la prochaine session\n");
        printf("   Relancez le programme pour retrouver votre calibration\n\n");
    }
    
    return 0;
}
