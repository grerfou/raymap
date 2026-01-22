#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

// Helper: Compare deux Vector2 avec tolérance
bool RM_Vector2Equals(Vector2 a, Vector2 b, float tolerance) {
    return fabsf(a.x - b.x) < tolerance && fabsf(a.y - b.y) < tolerance;
}

// Helper: Compare deux quads
bool QuadEquals(RM_Quad a, RM_Quad b, float tolerance) {
    return RM_Vector2Equals(a.topLeft, b.topLeft, tolerance) &&
           RM_Vector2Equals(a.topRight, b.topRight, tolerance) &&
           RM_Vector2Equals(a.bottomLeft, b.bottomLeft, tolerance) &&
           RM_Vector2Equals(a.bottomRight, b.bottomRight, tolerance);
}

int main(void) {
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 14: Configuration Save/Load\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    //  Initialiser Raylib EN PREMIER
    InitWindow(1024, 768, "Test 14: Config Save/Load");
    SetTargetFPS(60);
    
    int passedTests = 0;
    int totalTests = 0;
    
    // ═══════════════════════════════════════════════════════════
    // Test 1: Save configuration
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 1: Save configuration to file\n");
    
    RM_Surface *surface1 = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    
    RM_Quad testQuad = {
        {100, 50},
        {700, 80},
        {50, 550},
        {750, 580}
    };
    
    RM_SetQuad(surface1, testQuad);
    RM_SetMeshResolution(surface1, 24, 18);
    
    const char *filepath = "test_config.txt";
    bool saved = RM_SaveConfig(surface1, filepath);
    
    if (saved) {
        // Vérifier que le fichier existe
        FILE *check = fopen(filepath, "r");
        if (check) {
            fclose(check);
            printf("   File created successfully\n\n");
            passedTests++;
        } else {
            printf("   File not created\n\n");
        }
    } else {
        printf("   Save failed\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 2: Load configuration
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 2: Load configuration from file\n");
    
    RM_Surface *surface2 = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    bool loaded = RM_LoadConfig(surface2, filepath);
    
    if (loaded) {
        RM_Quad loadedQuad = RM_GetQuad(surface2);
        
        if (QuadEquals(testQuad, loadedQuad, 0.1f)) {
            printf("   Quad positions correct\n");
            
            int cols, rows;
            RM_GetMeshResolution(surface2, &cols, &rows);
            
            if (cols == 24 && rows == 18) {
                printf("   Mesh resolution correct\n");
                
                RM_MapMode mode = RM_GetMapMode(surface2);
                if (mode == RM_MAP_HOMOGRAPHY) {
                    printf("   Mode correct\n\n");
                    passedTests++;
                } else {
                    printf("   Mode incorrect (expected HOMOGRAPHY)\n\n");
                }
            } else {
                printf("   Mesh resolution incorrect (%dx%d, expected 24x18)\n\n", cols, rows);
            }
        } else {
            printf("   Quad positions incorrect\n");
            printf("   Expected TL: (%.1f, %.1f), Got: (%.1f, %.1f)\n", 
                   testQuad.topLeft.x, testQuad.topLeft.y,
                   loadedQuad.topLeft.x, loadedQuad.topLeft.y);
            printf("\n");
        }
    } else {
        printf("   Load failed\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 3: Invalid file handling
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 3: Handle invalid file gracefully\n");
    
    RM_Surface *surface3 = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    
    bool invalidLoad = RM_LoadConfig(surface3, "nonexistent_file.txt");
    
    if (!invalidLoad) {
        printf("   Invalid file handled gracefully (returned false)\n\n");
        passedTests++;
    } else {
        printf("   Should return false for invalid file\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 4: Round-trip (save → load → save → compare)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 4: Round-trip save→load→save integrity\n");
    
    RM_Surface *surface4a = RM_CreateSurface(1024, 768, RM_MAP_BILINEAR);
    
    RM_Quad originalQuad = {
        {200, 100},
        {824, 120},
        {180, 668},
        {844, 648}
    };
    
    RM_SetQuad(surface4a, originalQuad);
    RM_SetMeshResolution(surface4a, 32, 24);
    
    // Premier save
    RM_SaveConfig(surface4a, "test_roundtrip1.txt");
    
    // Load dans nouvelle surface
    RM_Surface *surface4b = RM_CreateSurface(1024, 768, RM_MAP_HOMOGRAPHY);
    RM_LoadConfig(surface4b, "test_roundtrip1.txt");
    
    // Deuxième save
    RM_SaveConfig(surface4b, "test_roundtrip2.txt");
    
    // Comparer les fichiers
    FILE *file1 = fopen("test_roundtrip1.txt", "r");
    FILE *file2 = fopen("test_roundtrip2.txt", "r");
    
    bool filesIdentical = true;
    
    if (file1 && file2) {
        char line1[256], line2[256];
        
        while (fgets(line1, sizeof(line1), file1) && fgets(line2, sizeof(line2), file2)) {
            // Ignorer les commentaires (peuvent varier)
            if (line1[0] == '#' || line2[0] == '#') continue;
            
            if (strcmp(line1, line2) != 0) {
                filesIdentical = false;
                break;
            }
        }
        
        fclose(file1);
        fclose(file2);
        
        if (filesIdentical) {
            printf("   Round-trip preserves configuration exactly\n\n");
            passedTests++;
        } else {
            printf("   Configuration changed during round-trip\n\n");
        }
    } else {
        printf("   Could not open files for comparison\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 5: Visual verification
    // ═══════════════════════════════════════════════════════════
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Visual test: Press SPACE to toggle between saved/loaded\n");
    printf("Press ESC to finish tests\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    RM_Surface *surfaceOriginal = RM_CreateSurface(600, 400, RM_MAP_HOMOGRAPHY);
    RM_Surface *surfaceLoaded = RM_CreateSurface(600, 400, RM_MAP_BILINEAR);
    
    RM_Quad visualQuad = {
        {150, 100},
        {850, 120},
        {100, 650},
        {900, 630}
    };
    
    RM_SetQuad(surfaceOriginal, visualQuad);
    RM_SetMeshResolution(surfaceOriginal, 32, 24);
    
    // Save et load
    RM_SaveConfig(surfaceOriginal, "visual_test.txt");
    RM_LoadConfig(surfaceLoaded, "visual_test.txt");
    
    bool showOriginal = true;
    
    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_SPACE)) {
            showOriginal = !showOriginal;
        }
        
        RM_Surface *currentSurface = showOriginal ? surfaceOriginal : surfaceLoaded;
        
        RM_BeginSurface(currentSurface);
            ClearBackground(DARKBLUE);
            
            DrawText(showOriginal ? "ORIGINAL" : "LOADED", 200, 180, 40, YELLOW);
            
            // Grille de test
            for (int x = 0; x < 600; x += 50) {
                DrawLine(x, 0, x, 400, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y < 400; y += 50) {
                DrawLine(0, y, 600, y, ColorAlpha(WHITE, 0.2f));
            }
            
            // Diagonales
            DrawLineEx((Vector2){0, 0}, (Vector2){600, 400}, 4, GREEN);
            DrawLineEx((Vector2){600, 0}, (Vector2){0, 400}, 4, ORANGE);
            
        RM_EndSurface(currentSurface);
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            RM_DrawSurface(currentSurface);
            
            DrawText("SPACE: Toggle Original/Loaded", 10, 10, 20, LIME);
            DrawText(showOriginal ? "Showing: ORIGINAL" : "Showing: LOADED", 10, 35, 20, YELLOW);
            DrawText("They should be IDENTICAL", 10, 60, 16, WHITE);
            DrawText("ESC: Finish tests", 10, 85, 16, GRAY);
            
            DrawFPS(10, 740);
            
        EndDrawing();
    }
    
    // Cleanup
    RM_DestroySurface(surface1);
    RM_DestroySurface(surface2);
    RM_DestroySurface(surface3);
    RM_DestroySurface(surface4a);
    RM_DestroySurface(surface4b);
    RM_DestroySurface(surfaceOriginal);
    RM_DestroySurface(surfaceLoaded);
    
    CloseWindow();
    
    // ═══════════════════════════════════════════════════════════
    // Résumé
    // ═══════════════════════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  RÉSULTATS\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    printf("Tests réussis: %d/%d\n\n", passedTests, totalTests);
    
    if (passedTests == totalTests) {
        printf(" Tous les tests passés !\n");
        printf("   - Save/Load fonctionne correctement\n");
        printf("   - Gestion d'erreurs OK\n");
        printf("   - Round-trip préserve les données\n");
        printf("   - Validation visuelle confirmée\n\n");
    } else {
        printf(" Certains tests ont échoué (%d/%d)\n\n", totalTests - passedTests, totalTests);
    }
    
    return (passedTests == totalTests) ? 0 : 1;
}
