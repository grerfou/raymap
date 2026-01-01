#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(1280, 720, "Test 07C: Resolution Impact - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════\n");
    printf("  Test: Impact de la Résolution\n");
    printf("═══════════════════════════════════════\n\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(500, 400, RM_MAP_MESH);
    
    // Quad TRÈS déformé (pour accentuer les différences)
    RM_Quad quad = {
        { 50, 50 },
        { 1230, 20 },
        { 20, 700 },
        { 1260, 670 }
    };
    RM_SetQuad(surface, quad);
    
    // Résolutions à tester (écarts extrêmes)
    typedef struct {
        int cols;
        int rows;
        const char *name;
        const char *quality;
        Color color;
    } ResPreset;
    
    ResPreset presets[] = {
        { 4, 4, "4x4", "TRÈS BASSE (visible)", RED },
        { 8, 8, "8x8", "BASSE", ORANGE },
        { 16, 16, "16x16", "MOYENNE (défaut MESH)", YELLOW },
        { 32, 32, "32x32", "HAUTE (défaut PERSP)", GREEN },
        { 48, 48, "48x48", "TRÈS HAUTE", SKYBLUE },
        { 64, 64, "64x64", "MAXIMUM", BLUE }
    };
    int presetCount = 6;
    
    int currentPreset = 0;  // Commence à 4x4 (pour voir les artefacts)
    RM_SetMeshResolution(surface, 
        presets[currentPreset].cols, 
        presets[currentPreset].rows);
    
    float timeSinceSwitch = 0.0f;
    float autoSwitchDelay = 4.0f;  // 4 secondes par preset
    bool autoSwitch = true;
    
    printf("Contrôles:\n");
    printf("  UP/DOWN   : Changer résolution\n");
    printf("  1-6       : Presets directs\n");
    printf("  A         : Toggle auto-cycle\n");
    printf("  ESC       : Quitter\n\n");
    
    printf("🔍 Observez les DIAGONALES et le CERCLE\n");
    printf("   Basse résolution = lignes angulaires\n");
    printf("   Haute résolution = lignes lisses\n\n");
    
    while (!WindowShouldClose()) {
        
        float dt = GetFrameTime();
        timeSinceSwitch += dt;
        
        // ═══════════════════════════════════════════
        // AUTO CYCLE
        // ═══════════════════════════════════════════
        
        if (autoSwitch && timeSinceSwitch >= autoSwitchDelay) {
            currentPreset = (currentPreset + 1) % presetCount;
            
            RM_SetMeshResolution(surface, 
                presets[currentPreset].cols, 
                presets[currentPreset].rows);
            
            printf(" Résolution: %s (%s)\n", 
                   presets[currentPreset].name,
                   presets[currentPreset].quality);
            
            timeSinceSwitch = 0.0f;
        }
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        if (IsKeyPressed(KEY_UP)) {
            if (currentPreset < presetCount - 1) {
                currentPreset++;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("⬆️  %s\n", presets[currentPreset].name);
                timeSinceSwitch = 0.0f;
            }
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            if (currentPreset > 0) {
                currentPreset--;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("⬇️  %s\n", presets[currentPreset].name);
                timeSinceSwitch = 0.0f;
            }
        }
        
        // Presets directs
        for (int i = 0; i < presetCount; i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                currentPreset = i;
                RM_SetMeshResolution(surface, 
                    presets[currentPreset].cols, 
                    presets[currentPreset].rows);
                printf("🎯 %s\n", presets[currentPreset].name);
                timeSinceSwitch = 0.0f;
            }
        }
        
        if (IsKeyPressed(KEY_A)) {
            autoSwitch = !autoSwitch;
            printf("⚙️  Auto-cycle: %s\n", autoSwitch ? "ON" : "OFF");
        }
        
        // ═══════════════════════════════════════════
        // DESSINER DANS LA SURFACE
        // ═══════════════════════════════════════════
        
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            // Titre avec résolution
            int c, r;
            RM_GetMeshResolution(surface, &c, &r);
            DrawText(TextFormat("%dx%d", c, r), 150, 30, 70, WHITE);
            
            // ═══════════════════════════════════════════
            // ÉLÉMENTS DE TEST (pour voir les artefacts)
            // ═══════════════════════════════════════════
            
            // 1. Grille FINE (artefacts visibles en basse résolution)
            for (int x = 0; x < 500; x += 25) {
                DrawLine(x, 0, x, 400, ColorAlpha(WHITE, 0.15f));
            }
            for (int y = 0; y < 400; y += 25) {
                DrawLine(0, y, 500, y, ColorAlpha(WHITE, 0.15f));
            }
            
            // 2. Cercle (doit être lisse)
            DrawCircle(250, 200, 100, ColorAlpha(RED, 0.8f));
            DrawCircleLines(250, 200, 105, WHITE);
            DrawCircleLines(250, 200, 110, ColorAlpha(YELLOW, 0.5f));
            
            // 3. Diagonales CRITIQUES (montrent clairement la résolution)
            for (int i = 0; i < 8; i++) {
                float offset = i * 15.0f;
                Color lineColor = ColorAlpha(GREEN, 0.4f);
                
                // Diagonale 1
                DrawLineEx(
                    (Vector2){0 + offset, 0}, 
                    (Vector2){500 - offset, 400}, 
                    3.0f, lineColor
                );
                
                // Diagonale 2
                DrawLineEx(
                    (Vector2){500 - offset, 0}, 
                    (Vector2){0 + offset, 400}, 
                    3.0f, ColorAlpha(ORANGE, 0.4f)
                );
            }
            
            // 4. Texte (déformation visible)
            DrawText("QUALITY", 150, 320, 50, ColorAlpha(WHITE, 0.7f));
            DrawText("TEST", 200, 360, 40, ColorAlpha(YELLOW, 0.7f));
            
        RM_EndSurface(surface);
        
        // ═══════════════════════════════════════════
        // DESSINER À L'ÉCRAN
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            RM_DrawSurface(surface);
            
            // ═══════════════════════════════════════════
            // HUD
            // ═══════════════════════════════════════════
            
            // Titre
            DrawRectangle(0, 0, 1280, 80, ColorAlpha(BLACK, 0.85f));
            DrawText("Test 07C: Impact de la Résolution", 20, 15, 28, LIME);
            DrawText(" Observez les DIAGONALES vertes/oranges", 20, 50, 18, LIGHTGRAY);
            
            // Info résolution actuelle (GROS)
            DrawRectangle(1280 - 450, 0, 450, 280, ColorAlpha(BLACK, 0.85f));
            
            DrawText("RÉSOLUTION:", 1280 - 440, 15, 22, WHITE);
            DrawText(presets[currentPreset].name, 1280 - 440, 45, 48, presets[currentPreset].color);
            DrawText(presets[currentPreset].quality, 1280 - 440, 100, 20, LIGHTGRAY);
            
            int cols, rows;
            RM_GetMeshResolution(surface, &cols, &rows);
            int verts = (cols + 1) * (rows + 1);
            int tris = cols * rows * 2;
            
            DrawText(TextFormat("Grille: %dx%d", cols, rows), 1280 - 440, 140, 18, WHITE);
            DrawText(TextFormat("Vertices: %d", verts), 1280 - 440, 165, 16, LIGHTGRAY);
            DrawText(TextFormat("Triangles: %d", tris), 1280 - 440, 185, 16, LIGHTGRAY);
            
            // Indicateur de qualité visuelle
            const char *visual = "";
            if (cols <= 8) {
                visual = " Artefacts TRÈS visibles";
            } else if (cols <= 16) {
                visual = "  Artefacts visibles";
            } else if (cols <= 32) {
                visual = " Qualité correcte";
            } else {
                visual = " Qualité excellente";
            }
            DrawText(visual, 1280 - 440, 220, 16, 
                    cols <= 8 ? RED : (cols <= 16 ? ORANGE : GREEN));
            
            DrawFPS(1280 - 440, 250);
            
            // Liste des presets
            DrawRectangle(0, 720 - 260, 380, 260, ColorAlpha(BLACK, 0.85f));
            DrawText("Presets disponibles:", 10, 720 - 250, 18, YELLOW);
            
            for (int i = 0; i < presetCount; i++) {
                Color textColor = (i == currentPreset) ? presets[i].color : DARKGRAY;
                DrawText(TextFormat("%d: %s - %s", i + 1, 
                        presets[i].name, 
                        presets[i].quality), 
                        20, 720 - 220 + i * 30, 16, textColor);
                
                if (i == currentPreset) {
                    DrawRectangle(10, 720 - 220 + i * 30, 5, 22, presets[i].color);
                }
            }
            
            DrawText(TextFormat("Auto: %s", autoSwitch ? "ON" : "OFF"), 
                    10, 720 - 30, 16, autoSwitch ? GREEN : RED);
            
            if (autoSwitch) {
                float progress = timeSinceSwitch / autoSwitchDelay;
                DrawRectangle(80, 720 - 25, 200, 12, DARKGRAY);
                DrawRectangle(80, 720 - 25, (int)(200 * progress), 12, GREEN);
            }
            
            // Instructions
            DrawRectangle(1280/2 - 300, 720 - 80, 600, 80, ColorAlpha(BLACK, 0.85f));
            DrawText("UP/DOWN: Changer | 1-6: Presets | A: Auto-cycle", 
                    1280/2 - 290, 720 - 65, 16, LIGHTGRAY);
            
            // Message clé
            const char *msg = "";
            Color msgColor = WHITE;
            
            if (cols <= 4) {
                msg = " 4x4: Regardez les diagonales - TRÈS angulaires!";
                msgColor = RED;
            } else if (cols == 8) {
                msg = " 8x8: Encore angulaire, mais mieux que 4x4";
                msgColor = ORANGE;
            } else if (cols == 16) {
                msg = " 16x16: Bon compromis qualité/performance";
                msgColor = YELLOW;
            } else if (cols == 32) {
                msg = " 32x32: Diagonales lisses, excellente qualité";
                msgColor = GREEN;
            } else {
                msg = " 48x48+: Qualité maximale, diagonales parfaites";
                msgColor = SKYBLUE;
            }
            
            DrawText(msg, 1280/2 - MeasureText(msg, 18)/2, 720 - 35, 18, msgColor);
            
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf("\n═══════════════════════════════════════\n");
    printf("   Test terminé\n");
    printf("═══════════════════════════════════════\n");
    
    return 0;
}
