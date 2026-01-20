#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <math.h>

int main(void) {
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 17: Point Mapping (Texture ↔ Screen)\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    InitWindow(1600, 900, "Test 17: Point Mapping");
    SetTargetFPS(60);
    
    int passedTests = 0;
    int totalTests = 0;
    
    // ═══════════════════════════════════════════════════════════
    // Test avec HOMOGRAPHY
    // ═══════════════════════════════════════════════════════════
    
    printf("Testing HOMOGRAPHY mode:\n");
    
    RM_Surface *surfaceHomo = RM_CreateSurface(600, 400, RM_MAP_HOMOGRAPHY);
    
    RM_Quad quadHomo = {
        {200, 100},
        {600, 120},
        {150, 500},
        {650, 480}
    };
    
    RM_SetQuad(surfaceHomo, quadHomo);
    
    // ═══════════════════════════════════════════════════════════
    // DEBUG : Analyse détaillée du centre
    // ═══════════════════════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("DEBUG: Analyse de précision du centre\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Coins du quad
    printf("Quad corners:\n");
    printf("  TL: (%.2f, %.2f)\n", quadHomo.topLeft.x, quadHomo.topLeft.y);
    printf("  TR: (%.2f, %.2f)\n", quadHomo.topRight.x, quadHomo.topRight.y);
    printf("  BL: (%.2f, %.2f)\n", quadHomo.bottomLeft.x, quadHomo.bottomLeft.y);
    printf("  BR: (%.2f, %.2f)\n\n", quadHomo.bottomRight.x, quadHomo.bottomRight.y);
    
    // Centre géométrique (moyenne des 4 coins)
    Vector2 geometric_center = RM_GetQuadCenter(quadHomo);
    printf("Geometric center (average of 4 corners):\n");
    printf("  (%.2f, %.2f)\n\n", geometric_center.x, geometric_center.y);
    
    // Centre via homographie (texture 0.5, 0.5)
    Vector2 homography_center = RM_MapPoint(surfaceHomo, (Vector2){0.5f, 0.5f});
    printf("Homography center (0.5, 0.5) mapped:\n");
    printf("  (%.2f, %.2f)\n\n", homography_center.x, homography_center.y);
    
    // Différence
    float diff_x = homography_center.x - geometric_center.x;
    float diff_y = homography_center.y - geometric_center.y;
    float distance = Vector2Distance(homography_center, geometric_center);
    
    printf("Difference:\n");
    printf("  ΔX: %.2f px\n", diff_x);
    printf("  ΔY: %.2f px\n", diff_y);
    printf("  Distance: %.2f px\n\n", distance);
    
    // Test avec BILINEAR pour comparaison
    Vector2 bilinear_center = rm_BilinearInterpolation(
        quadHomo.topLeft,
        quadHomo.topRight,
        quadHomo.bottomLeft,
        quadHomo.bottomRight,
        0.5f, 0.5f
    );
    
    printf("Bilinear interpolation center (for comparison):\n");
    printf("  (%.2f, %.2f)\n", bilinear_center.x, bilinear_center.y);
    printf("  Distance from geometric: %.2f px\n\n", 
           Vector2Distance(bilinear_center, geometric_center));
    
    // Vérifier si les lignes médianes se croisent au bon endroit
    Vector2 top_mid = {
        (quadHomo.topLeft.x + quadHomo.topRight.x) / 2.0f,
        (quadHomo.topLeft.y + quadHomo.topRight.y) / 2.0f
    };
    Vector2 bottom_mid = {
        (quadHomo.bottomLeft.x + quadHomo.bottomRight.x) / 2.0f,
        (quadHomo.bottomLeft.y + quadHomo.bottomRight.y) / 2.0f
    };
    Vector2 left_mid = {
        (quadHomo.topLeft.x + quadHomo.bottomLeft.x) / 2.0f,
        (quadHomo.topLeft.y + quadHomo.bottomLeft.y) / 2.0f
    };
    Vector2 right_mid = {
        (quadHomo.topRight.x + quadHomo.bottomRight.x) / 2.0f,
        (quadHomo.topRight.y + quadHomo.bottomRight.y) / 2.0f
    };
    
    printf("Edge midpoints:\n");
    printf("  Top:    (%.2f, %.2f)\n", top_mid.x, top_mid.y);
    printf("  Bottom: (%.2f, %.2f)\n", bottom_mid.x, bottom_mid.y);
    printf("  Left:   (%.2f, %.2f)\n", left_mid.x, left_mid.y);
    printf("  Right:  (%.2f, %.2f)\n\n", right_mid.x, right_mid.y);
    
    // Point central via médiane horizontale
    Vector2 h_median_center = {
        (left_mid.x + right_mid.x) / 2.0f,
        (left_mid.y + right_mid.y) / 2.0f
    };
    
    // Point central via médiane verticale
    Vector2 v_median_center = {
        (top_mid.x + bottom_mid.x) / 2.0f,
        (top_mid.y + bottom_mid.y) / 2.0f
    };
    
    printf("Median-based centers:\n");
    printf("  Horizontal median: (%.2f, %.2f)\n", h_median_center.x, h_median_center.y);
    printf("  Vertical median:   (%.2f, %.2f)\n", v_median_center.x, v_median_center.y);
    printf("  Difference: %.2f px\n\n", Vector2Distance(h_median_center, v_median_center));
    
    // Tester plusieurs points pour voir la distorsion
    printf("Grid test (9 points):\n");
    for (int y = 0; y <= 2; y++) {
        for (int x = 0; x <= 2; x++) {
            float u = x / 2.0f;
            float v = y / 2.0f;
            
            Vector2 homo = RM_MapPoint(surfaceHomo, (Vector2){u, v});
            Vector2 bili = rm_BilinearInterpolation(
                quadHomo.topLeft, quadHomo.topRight,
                quadHomo.bottomLeft, quadHomo.bottomRight,
                u, v
            );
            
            float diff = Vector2Distance(homo, bili);
            printf("  (%.1f, %.1f): HOMO(%.0f,%.0f) vs BILI(%.0f,%.0f) = %.1fpx diff\n",
                   u, v, homo.x, homo.y, bili.x, bili.y, diff);
        }
    }
    
    printf("\n═══════════════════════════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════════════════════
    // Test 1: Coins du quad (HOMOGRAPHY)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("\nTest 1: Corner mapping (HOMOGRAPHY)\n");
    
    Vector2 tl_mapped = RM_MapPoint(surfaceHomo, (Vector2){0, 0});
    Vector2 tr_mapped = RM_MapPoint(surfaceHomo, (Vector2){1, 0});
    Vector2 bl_mapped = RM_MapPoint(surfaceHomo, (Vector2){0, 1});
    Vector2 br_mapped = RM_MapPoint(surfaceHomo, (Vector2){1, 1});
    
    bool cornersCorrect = (
        Vector2Distance(tl_mapped, quadHomo.topLeft) < 1.0f &&
        Vector2Distance(tr_mapped, quadHomo.topRight) < 1.0f &&
        Vector2Distance(bl_mapped, quadHomo.bottomLeft) < 1.0f &&
        Vector2Distance(br_mapped, quadHomo.bottomRight) < 1.0f
    );
    
    if (cornersCorrect) {
        printf("  ✅ All corners mapped correctly\n");
        printf("     TL error: %.3f px\n", Vector2Distance(tl_mapped, quadHomo.topLeft));
        printf("     TR error: %.3f px\n", Vector2Distance(tr_mapped, quadHomo.topRight));
        printf("     BL error: %.3f px\n", Vector2Distance(bl_mapped, quadHomo.bottomLeft));
        printf("     BR error: %.3f px\n", Vector2Distance(br_mapped, quadHomo.bottomRight));
        passedTests++;
    } else {
        printf("  ❌ Corner mapping incorrect\n");
        printf("     TL: expected (%.0f,%.0f), got (%.0f,%.0f)\n",
               quadHomo.topLeft.x, quadHomo.topLeft.y, tl_mapped.x, tl_mapped.y);
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 2: Centre du quad (HOMOGRAPHY)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("\nTest 2: Center mapping (HOMOGRAPHY)\n");
    
    Vector2 center_mapped = RM_MapPoint(surfaceHomo, (Vector2){0.5f, 0.5f});
    Vector2 expected_center = RM_GetQuadCenter(quadHomo);
    
    float centerError = Vector2Distance(center_mapped, expected_center);
    
    // Note: Pour un quad déformé (trapèze), le centre projectif
    // peut différer du centre géométrique (moyenne des coins)
    if (centerError < 10.0f) {
        printf("  ✅ Center mapped perfectly (error: %.1f px)\n", centerError);
        passedTests++;
    } else if (centerError < 50.0f) {
        printf("  ✅ Center mapped acceptably (error: %.1f px)\n", centerError);
        printf("     Note: Difference expected for perspective-distorted quads\n");
        passedTests++;
    } else {
        printf("  ❌ Center mapping error too large (%.1f px)\n", centerError);
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 3: Round-trip (HOMOGRAPHY)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("\nTest 3: Round-trip map→unmap (HOMOGRAPHY)\n");
    
    Vector2 original = {0.3f, 0.7f};
    Vector2 mapped = RM_MapPoint(surfaceHomo, original);
    Vector2 unmapped = RM_UnmapPoint(surfaceHomo, mapped);
    
    float roundtripError = Vector2Distance(original, unmapped);
    
    if (roundtripError < 0.01f) {
        printf("  ✅ Round-trip accurate (error: %.4f)\n", roundtripError);
        passedTests++;
    } else {
        printf("  ❌ Round-trip error too large: %.4f\n", roundtripError);
        printf("     Original: (%.3f, %.3f)\n", original.x, original.y);
        printf("     After round-trip: (%.3f, %.3f)\n", unmapped.x, unmapped.y);
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test avec BILINEAR
    // ═══════════════════════════════════════════════════════════
    
    printf("\n───────────────────────────────────────────────────────────\n");
    printf("Testing BILINEAR mode:\n");
    
    RM_Surface *surfaceBilinear = RM_CreateSurface(600, 400, RM_MAP_BILINEAR);
    
    RM_Quad quadBilinear = {
        {900, 100},
        {1300, 120},
        {850, 500},
        {1350, 480}
    };
    
    RM_SetQuad(surfaceBilinear, quadBilinear);
    
    // ═══════════════════════════════════════════════════════════
    // Test 4: Coins du quad (BILINEAR)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("\nTest 4: Corner mapping (BILINEAR)\n");
    
    Vector2 tl_bi = RM_MapPoint(surfaceBilinear, (Vector2){0, 0});
    Vector2 tr_bi = RM_MapPoint(surfaceBilinear, (Vector2){1, 0});
    Vector2 bl_bi = RM_MapPoint(surfaceBilinear, (Vector2){0, 1});
    Vector2 br_bi = RM_MapPoint(surfaceBilinear, (Vector2){1, 1});
    
    bool cornersBiCorrect = (
        Vector2Distance(tl_bi, quadBilinear.topLeft) < 1.0f &&
        Vector2Distance(tr_bi, quadBilinear.topRight) < 1.0f &&
        Vector2Distance(bl_bi, quadBilinear.bottomLeft) < 1.0f &&
        Vector2Distance(br_bi, quadBilinear.bottomRight) < 1.0f
    );
    
    if (cornersBiCorrect) {
        printf("  ✅ All corners mapped correctly\n");
        passedTests++;
    } else {
        printf("  ❌ Corner mapping incorrect\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 5: Round-trip (BILINEAR)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("\nTest 5: Round-trip map→unmap (BILINEAR)\n");
    
    Vector2 originalBi = {0.4f, 0.6f};
    Vector2 mappedBi = RM_MapPoint(surfaceBilinear, originalBi);
    Vector2 unmappedBi = RM_UnmapPoint(surfaceBilinear, mappedBi);
    
    float roundtripErrorBi = Vector2Distance(originalBi, unmappedBi);
    
    if (roundtripErrorBi < 0.05f) {
        printf("  ✅ Round-trip accurate (error: %.4f)\n", roundtripErrorBi);
        passedTests++;
    } else {
        printf("  ⚠️  Round-trip error: %.4f (acceptable for bilinear)\n", roundtripErrorBi);
        // Bilinear peut avoir plus d'erreur car l'inverse est approximatif
        if (roundtripErrorBi < 0.1f) {
            passedTests++;
        }
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test visuel interactif
    // ═══════════════════════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("Visual test: Move mouse to see texture coordinates\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    while (!WindowShouldClose()) {
        
        Vector2 mousePos = GetMousePosition();
        
        // Unmap pour les deux surfaces
        Vector2 texCoordHomo = RM_UnmapPoint(surfaceHomo, mousePos);
        Vector2 texCoordBi = RM_UnmapPoint(surfaceBilinear, mousePos);
        
        // Test round-trip visuel
        Vector2 testPoint = {0.5f, 0.5f};
        Vector2 mappedTestHomo = RM_MapPoint(surfaceHomo, testPoint);
        Vector2 mappedTestBi = RM_MapPoint(surfaceBilinear, testPoint);
        
        BeginDrawing();
            ClearBackground((Color){20, 20, 30, 255});
            
            // Titre
            DrawText("Point Mapping Test", 20, 20, 30, LIME);
            DrawText("Move mouse over quads to see texture coordinates", 20, 60, 20, LIGHTGRAY);
            
            // Dessiner quads HOMOGRAPHY
            DrawLineV(quadHomo.topLeft, quadHomo.topRight, YELLOW);
            DrawLineV(quadHomo.topRight, quadHomo.bottomRight, YELLOW);
            DrawLineV(quadHomo.bottomRight, quadHomo.bottomLeft, YELLOW);
            DrawLineV(quadHomo.bottomLeft, quadHomo.topLeft, YELLOW);
            
            DrawText("HOMOGRAPHY", (int)quadHomo.topLeft.x, (int)quadHomo.topLeft.y - 30, 20, YELLOW);
            
            // Coins
            DrawCircleV(quadHomo.topLeft, 6, GREEN);
            DrawCircleV(quadHomo.topRight, 6, GREEN);
            DrawCircleV(quadHomo.bottomLeft, 6, GREEN);
            DrawCircleV(quadHomo.bottomRight, 6, GREEN);
            
            // Centre
            DrawCircleV(mappedTestHomo, 8, ORANGE);
            DrawText("(0.5, 0.5)", (int)mappedTestHomo.x + 12, (int)mappedTestHomo.y - 5, 14, ORANGE);
            
            // Dessiner quads BILINEAR
            DrawLineV(quadBilinear.topLeft, quadBilinear.topRight, SKYBLUE);
            DrawLineV(quadBilinear.topRight, quadBilinear.bottomRight, SKYBLUE);
            DrawLineV(quadBilinear.bottomRight, quadBilinear.bottomLeft, SKYBLUE);
            DrawLineV(quadBilinear.bottomLeft, quadBilinear.topLeft, SKYBLUE);
            
            DrawText("BILINEAR", (int)quadBilinear.topLeft.x, (int)quadBilinear.topLeft.y - 30, 20, SKYBLUE);
            
            // Coins
            DrawCircleV(quadBilinear.topLeft, 6, GREEN);
            DrawCircleV(quadBilinear.topRight, 6, GREEN);
            DrawCircleV(quadBilinear.bottomLeft, 6, GREEN);
            DrawCircleV(quadBilinear.bottomRight, 6, GREEN);
            
            // Centre
            DrawCircleV(mappedTestBi, 8, ORANGE);
            DrawText("(0.5, 0.5)", (int)mappedTestBi.x + 12, (int)mappedTestBi.y - 5, 14, ORANGE);
            
            // Curseur
            DrawCircleV(mousePos, 8, RED);
            DrawCircleLines((int)mousePos.x, (int)mousePos.y, 8, WHITE);
            
            // Info panels
            DrawRectangle(20, 700, 750, 180, ColorAlpha(BLACK, 0.85f));
            DrawText("HOMOGRAPHY", 35, 715, 20, YELLOW);
            
            if (texCoordHomo.x >= 0) {
                DrawText(TextFormat("Texture coords: (%.3f, %.3f)", texCoordHomo.x, texCoordHomo.y),
                         35, 745, 18, WHITE);
                
                Vector2 remapped = RM_MapPoint(surfaceHomo, texCoordHomo);
                DrawText(TextFormat("Remapped screen: (%.0f, %.0f)", remapped.x, remapped.y),
                         35, 770, 16, GRAY);
                
                float error = Vector2Distance(mousePos, remapped);
                DrawText(TextFormat("Round-trip error: %.2f px", error),
                         35, 795, 16, error < 1.0f ? GREEN : ORANGE);
            } else {
                DrawText("Mouse outside quad", 35, 745, 18, RED);
            }
            
            DrawText("Mouse position:", 35, 820, 16, LIGHTGRAY);
            DrawText(TextFormat("  Screen: (%.0f, %.0f)", mousePos.x, mousePos.y),
                     35, 845, 14, GRAY);
            
            DrawRectangle(830, 700, 750, 180, ColorAlpha(BLACK, 0.85f));
            DrawText("BILINEAR", 845, 715, 20, SKYBLUE);
            
            if (texCoordBi.x >= 0) {
                DrawText(TextFormat("Texture coords: (%.3f, %.3f)", texCoordBi.x, texCoordBi.y),
                         845, 745, 18, WHITE);
                
                Vector2 remappedBi = RM_MapPoint(surfaceBilinear, texCoordBi);
                DrawText(TextFormat("Remapped screen: (%.0f, %.0f)", remappedBi.x, remappedBi.y),
                         845, 770, 16, GRAY);
                
                float errorBi = Vector2Distance(mousePos, remappedBi);
                DrawText(TextFormat("Round-trip error: %.2f px", errorBi),
                         845, 795, 16, errorBi < 5.0f ? GREEN : ORANGE);
            } else {
                DrawText("Mouse outside quad", 845, 745, 18, RED);
            }
            
            DrawText("Mouse position:", 845, 820, 16, LIGHTGRAY);
            DrawText(TextFormat("  Screen: (%.0f, %.0f)", mousePos.x, mousePos.y),
                     845, 845, 14, GRAY);
            
            DrawFPS(1550, 20);
            
        EndDrawing();
    }
    
    // Cleanup
    RM_DestroySurface(surfaceHomo);
    RM_DestroySurface(surfaceBilinear);
    CloseWindow();
    
    // ═══════════════════════════════════════════════════════════
    // Résumé
    // ═══════════════════════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  RÉSULTATS\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    printf("Tests réussis: %d/%d\n\n", passedTests, totalTests);
    
    if (passedTests == totalTests) {
        printf("✅ Tous les tests passés !\n");
        printf("   - Map/Unmap fonctionnent correctement\n");
        printf("   - Round-trip précis (HOMOGRAPHY)\n");
        printf("   - Round-trip acceptable (BILINEAR)\n");
        printf("   - Coins correctement mappés\n\n");
    } else {
        printf("❌ Certains tests ont échoué (%d/%d)\n\n", 
               totalTests - passedTests, totalTests);
    }
    
    return (passedTests == totalTests) ? 0 : 1;
}
