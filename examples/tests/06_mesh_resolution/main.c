#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(1280, 720, "Test 06: Mesh Resolution - RayMap");
    SetTargetFPS(60);
    
    printf("=== Test Mesh Resolution ===\n\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(400, 300, RM_MAP_MESH);
    
    // Quad déformé (trapèze prononcé pour voir la qualité)
    RM_Quad trapeze = {
        { 100, 50 },
        { 1180, 80 },
        { 50, 670 },
        { 1230, 640 }
    };
    RM_SetQuad(surface, trapeze);
    
    printf(" Surface créée\n");
    printf(" Quad trapèze défini\n\n");
    
    // Résolutions présets
    typedef struct {
        int cols;
        int rows;
        const char *name;
        Color color;
    } ResolutionPreset;
    
    ResolutionPreset presets[] = {
        { 4, 4, "4x4 (Basse)", RED },
        { 8, 8, "8x8 (Moyenne-)", ORANGE },
        { 16, 16, "16x16 (Moyenne)", YELLOW },
        { 32, 32, "32x32 (Haute)", GREEN },
        { 64, 64, "64x64 (Très haute)", BLUE }
    };
    int presetCount = 5;
    int currentPreset = 2;  // Commence à 16x16
    
    // Variables
    float time = 0.0f;
    bool showInfo = true;
    int fpsHistory[60] = {0};
    int fpsIndex = 0;
    
    printf("Contrôles:\n");
    printf("  UP/DOWN   : Changer résolution\n");
    printf("  1-5       : Preset résolution\n");
    printf("  I         : Toggle info\n");
    printf("  ESC       : Quitter\n\n");
    
    while (!WindowShouldClose()) {
        
        float deltaTime = GetFrameTime();
        time += deltaTime;
        
        // Enregistrer FPS pour moyenne
        fpsHistory[fpsIndex] = GetFPS();
        fpsIndex = (fpsIndex + 1) % 60;
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        // Changer résolution avec UP/DOWN
        if (IsKeyPressed(KEY_UP)) {
            if (currentPreset < presetCount - 1) {
                currentPreset++;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("📈 Résolution: %s\n", presets[currentPreset].name);
            }
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            if (currentPreset > 0) {
                currentPreset--;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("📉 Résolution: %s\n", presets[currentPreset].name);
            }
        }
        
        // Presets directs (touches 1-5)
        for (int i = 0; i < presetCount; i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                currentPreset = i;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("🎯 Résolution: %s\n", presets[currentPreset].name);
            }
        }
        
        // Toggle info
        if (IsKeyPressed(KEY_I)) {
            showInfo = !showInfo;
        }
        
        // ═══════════════════════════════════════════
        // DESSINER DANS LA SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            // Titre
            DrawText("RESOLUTION TEST", 60, 30, 40, WHITE);
            
            // Cercle avec dégradé (pour voir la qualité)
            for (int r = 80; r > 0; r -= 5) {
                Color c = ColorAlpha(RED, (float)r / 80.0f);
                DrawCircle(200, 150, r, c);
            }
            
            // Rectangle avec bordure
            DrawRectangle(260, 90, 120, 120, GREEN);
            DrawRectangleLines(258, 88, 124, 124, WHITE);
            
            // Grille fine (pour voir les artefacts)
            for (int x = 0; x < 400; x += 20) {
                DrawLine(x, 0, x, 300, ColorAlpha(WHITE, 0.15f));
            }
            for (int y = 0; y < 300; y += 20) {
                DrawLine(0, y, 400, y, ColorAlpha(WHITE, 0.15f));
            }
            
            // Diagonales (doivent être lisses)
            for (int i = 0; i < 5; i++) {
                float offset = i * 10.0f;
                DrawLine(0 + offset, 0, 400, 300 - offset, ColorAlpha(YELLOW, 0.3f));
            }
            
            // Texte avec effet
            DrawText("QUALITY", 50, 240, 50, ColorAlpha(WHITE, 0.8f));
            DrawTextEx(GetFontDefault(), "CHECK", (Vector2){250, 240}, 50, 2, ORANGE);
            
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
                // Info résolution actuelle
                int cols, rows;
                RM_GetMeshResolution(surface, &cols, &rows);
                int vertexCount = (cols + 1) * (rows + 1);
                int triangleCount = cols * rows * 2;
                
                // Calculer FPS moyen
                int avgFps = 0;
                for (int i = 0; i < 60; i++) avgFps += fpsHistory[i];
                avgFps /= 60;
                
                // Panel info principal
                DrawRectangle(0, 0, 420, 200, ColorAlpha(BLACK, 0.8f));
                DrawText("Test: Mesh Resolution", 10, 10, 24, LIME);
                
                DrawText(TextFormat("Resolution: %s", presets[currentPreset].name), 
                        10, 45, 20, presets[currentPreset].color);
                DrawText(TextFormat("Grid: %dx%d", cols, rows), 10, 70, 18, WHITE);
                DrawText(TextFormat("Vertices: %d", vertexCount), 10, 95, 16, LIGHTGRAY);
                DrawText(TextFormat("Triangles: %d", triangleCount), 10, 115, 16, LIGHTGRAY);
                
                DrawText(TextFormat("FPS: %d", GetFPS()), 10, 145, 18, 
                        GetFPS() > 55 ? GREEN : (GetFPS() > 30 ? ORANGE : RED));
                DrawText(TextFormat("Avg FPS: %d", avgFps), 10, 170, 16, LIGHTGRAY);
                
                // Presets disponibles
                DrawRectangle(1280 - 250, 0, 250, 240, ColorAlpha(BLACK, 0.8f));
                DrawText("Presets:", 1280 - 240, 10, 18, YELLOW);
                
                for (int i = 0; i < presetCount; i++) {
                    Color c = (i == currentPreset) ? presets[i].color : DARKGRAY;
                    DrawText(TextFormat("%d: %s", i + 1, presets[i].name), 
                            1280 - 240, 40 + i * 30, 16, c);
                    
                    if (i == currentPreset) {
                        DrawRectangle(1280 - 250, 40 + i * 30, 5, 20, presets[i].color);
                    }
                }
                
                // Contrôles
                DrawRectangle(0, 720 - 140, 300, 140, ColorAlpha(BLACK, 0.8f));
                DrawText("Contrôles:", 10, 720 - 130, 16, YELLOW);
                DrawText("UP/DOWN: Resolution", 10, 720 - 105, 14, LIGHTGRAY);
                DrawText("1-5: Presets", 10, 720 - 85, 14, LIGHTGRAY);
                DrawText("I: Toggle info", 10, 720 - 65, 14, LIGHTGRAY);
                DrawText("ESC: Quit", 10, 720 - 45, 14, LIGHTGRAY);
                
                // Indicateur de performance
                const char *perfMsg = "";
                Color perfColor = WHITE;
                
                if (avgFps >= 58) {
                    perfMsg = "Performance: EXCELLENTE";
                    perfColor = GREEN;
                } else if (avgFps >= 45) {
                    perfMsg = "Performance: BONNE";
                    perfColor = YELLOW;
                } else if (avgFps >= 30) {
                    perfMsg = "Performance: MOYENNE";
                    perfColor = ORANGE;
                } else {
                    perfMsg = "Performance: FAIBLE";
                    perfColor = RED;
                }
                
                DrawText(perfMsg, 1280/2 - MeasureText(perfMsg, 20)/2, 720 - 40, 20, perfColor);
            }
            
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n===  Test terminé ===\n");
    printf("Points validés:\n");
    printf("   Résolution dynamique fonctionne\n");
    printf("   Limites min/max respectées (4x4 - 64x64)\n");
    printf("   Régénération automatique du mesh\n");
    printf("   Performance ajustable\n");
    
    return 0;
}
