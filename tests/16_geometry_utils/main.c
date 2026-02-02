#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <math.h>

int main(void) {
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 16: Geometry Utilities\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    int passedTests = 0;
    int totalTests = 0;
    
    // ═══════════════════════════════════════════════════════════
    // Test 1: Point dans quad (centre)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 1: Point inside quad (center)\n");
    
    RM_Quad testQuad = {
        {100, 100},  // TL
        {500, 100},  // TR
        {500, 400},   // BR
        {100, 400}  // BL
    };
    
    Vector2 centerPoint = {300, 250};
    
    if (RM_PointInQuad(centerPoint, testQuad)) {
        printf("  ✅ Center point correctly detected inside\n\n");
        passedTests++;
    } else {
        printf("  ❌ Center point should be inside\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 2: Point hors quad
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 2: Point outside quad\n");
    
    Vector2 outsidePoint = {50, 50};
    
    if (!RM_PointInQuad(outsidePoint, testQuad)) {
        printf("  ✅ Outside point correctly detected\n\n");
        passedTests++;
    } else {
        printf("  ❌ Point should be outside\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 3: Points sur les bords
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 3: Points on edges\n");
    
    Vector2 edgePoint = {300, 100};  // Sur le bord supérieur
    
    if (RM_PointInQuad(edgePoint, testQuad)) {
        printf("  ✅ Edge point detected as inside (expected)\n\n");
        passedTests++;
    } else {
        printf("  ❌ Edge point should be inside\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 4: Bounding box
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 4: Bounding box calculation\n");
    
    Rectangle bounds = RM_GetQuadBounds(testQuad);
    
    bool boundsCorrect = (
        fabsf(bounds.x - 100.0f) < 0.1f &&
        fabsf(bounds.y - 100.0f) < 0.1f &&
        fabsf(bounds.width - 400.0f) < 0.1f &&
        fabsf(bounds.height - 300.0f) < 0.1f
    );
    
    if (boundsCorrect) {
        printf("  ✅ Bounds correct: (%.0f, %.0f, %.0f×%.0f)\n\n", 
               bounds.x, bounds.y, bounds.width, bounds.height);
        passedTests++;
    } else {
        printf("  ❌ Bounds incorrect\n");
        printf("     Got: (%.0f, %.0f, %.0f×%.0f)\n", 
               bounds.x, bounds.y, bounds.width, bounds.height);
        printf("     Expected: (100, 100, 400×300)\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 5: Quad trapèze (déformé)
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 5: Trapezoid quad (perspective)\n");
    
    RM_Quad trapezoid = {
        {200, 100},  // TL - étroit en haut
        {400, 100},  // TR
        {500, 400},   // BR
        {100, 400}  // BL - large en bas
    };
    
    Vector2 insideTrap = {300, 250};
    Vector2 outsideTrap = {50, 250};
    
    bool trapTestPass = (
        RM_PointInQuad(insideTrap, trapezoid) &&
        !RM_PointInQuad(outsideTrap, trapezoid)
    );
    
    if (trapTestPass) {
        printf("  ✅ Trapezoid collision correct\n\n");
        passedTests++;
    } else {
        printf("  ❌ Trapezoid collision incorrect\n\n");
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 6: Centre du quad
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 6: Quad center calculation\n");
    
    Vector2 center = RM_GetQuadCenter(testQuad);
    
    bool centerCorrect = (
        fabsf(center.x - 300.0f) < 0.1f &&
        fabsf(center.y - 250.0f) < 0.1f
    );
    
    if (centerCorrect) {
        printf("  ✅ Center correct: (%.0f, %.0f)\n\n", center.x, center.y);
        passedTests++;
    } else {
        printf("  ❌ Center incorrect: (%.0f, %.0f), expected (300, 250)\n\n", 
               center.x, center.y);
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test 7: Aire du quad
    // ═══════════════════════════════════════════════════════════
    
    totalTests++;
    printf("Test 7: Quad area calculation\n");
    
    float area = RM_GetQuadArea(testQuad);
    float expectedArea = 400.0f * 300.0f;  // Rectangle 400×300
    
    bool areaCorrect = fabsf(area - expectedArea) < 1.0f;
    
    if (areaCorrect) {
        printf("  ✅ Area correct: %.0f pixels²\n\n", area);
        passedTests++;
    } else {
        printf("  ❌ Area incorrect: %.0f, expected %.0f\n\n", area, expectedArea);
    }
    
    // ═══════════════════════════════════════════════════════════
    // Test visuel interactif
    // ═══════════════════════════════════════════════════════════
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Visual test: Move mouse to test point collision\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    InitWindow(1200, 800, "Test 16: Geometry Utilities");
    SetTargetFPS(60);
    
    RM_Quad visualQuad = {
        {300, 200},
        {900, 250},
        {950, 550},
        {250, 600}
    };
    
    int testPointsInside = 0;
    int testPointsOutside = 0;
    
    while (!WindowShouldClose()) {
        
        Vector2 mousePos = GetMousePosition();
        bool isInside = RM_PointInQuad(mousePos, visualQuad);
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isInside) {
                testPointsInside++;
            } else {
                testPointsOutside++;
            }
        }
        
        BeginDrawing();
            ClearBackground((Color){20, 20, 30, 255});
            
            // Titre
            DrawText("Geometry Utilities Test", 20, 20, 30, LIME);
            DrawText("Move mouse • Click to test collision", 20, 60, 20, LIGHTGRAY);
            
            // Dessiner le quad
            DrawLineV(visualQuad.topLeft, visualQuad.topRight, YELLOW);
            DrawLineV(visualQuad.topRight, visualQuad.bottomRight, YELLOW);
            DrawLineV(visualQuad.bottomRight, visualQuad.bottomLeft, YELLOW);
            DrawLineV(visualQuad.bottomLeft, visualQuad.topLeft, YELLOW);
            
            // Remplir le quad (semi-transparent)
            Color fillColor = isInside ? 
                ColorAlpha(GREEN, 0.2f) : ColorAlpha(RED, 0.1f);
            
            // Triangulation simple pour remplissage
            DrawTriangle(visualQuad.topLeft, visualQuad.topRight, visualQuad.bottomLeft, fillColor);
            DrawTriangle(visualQuad.topRight, visualQuad.bottomRight, visualQuad.bottomLeft, fillColor);
            
            // Coins
            DrawCircleV(visualQuad.topLeft, 8, YELLOW);
            DrawCircleV(visualQuad.topRight, 8, YELLOW);
            DrawCircleV(visualQuad.bottomLeft, 8, YELLOW);
            DrawCircleV(visualQuad.bottomRight, 8, YELLOW);
            
            // Labels coins
            DrawText("TL", (int)visualQuad.topLeft.x - 20, (int)visualQuad.topLeft.y - 25, 16, WHITE);
            DrawText("TR", (int)visualQuad.topRight.x + 10, (int)visualQuad.topRight.y - 25, 16, WHITE);
            DrawText("BL", (int)visualQuad.bottomLeft.x - 20, (int)visualQuad.bottomLeft.y + 10, 16, WHITE);
            DrawText("BR", (int)visualQuad.bottomRight.x + 10, (int)visualQuad.bottomRight.y + 10, 16, WHITE);
            
            // Bounding box
            Rectangle bounds = RM_GetQuadBounds(visualQuad);
            DrawRectangleLinesEx(bounds, 2, ColorAlpha(BLUE, 0.5f));
            DrawText("Bounding Box", (int)bounds.x, (int)bounds.y - 25, 16, BLUE);
            
            // Centre
            Vector2 center = RM_GetQuadCenter(visualQuad);
            DrawCircleV(center, 6, ORANGE);
            DrawText("Center", (int)center.x + 10, (int)center.y - 5, 16, ORANGE);
            
            // Curseur
            Color cursorColor = isInside ? GREEN : RED;
            DrawCircleV(mousePos, 10, cursorColor);
            DrawCircleLines((int)mousePos.x, (int)mousePos.y, 10, WHITE);
            
            // Info
            DrawRectangle(20, 680, 400, 100, ColorAlpha(BLACK, 0.8f));
            DrawText(isInside ? "Mouse: INSIDE" : "Mouse: OUTSIDE", 
                     35, 695, 24, isInside ? GREEN : RED);
            DrawText(TextFormat("Position: (%.0f, %.0f)", mousePos.x, mousePos.y), 
                     35, 725, 18, WHITE);
            DrawText(TextFormat("Clicks: %d inside, %d outside", 
                     testPointsInside, testPointsOutside), 
                     35, 750, 16, GRAY);
            
            // Stats
            DrawRectangle(800, 680, 380, 100, ColorAlpha(BLACK, 0.8f));
            float area = RM_GetQuadArea(visualQuad);
            DrawText(TextFormat("Area: %.0f px²", area), 815, 695, 20, WHITE);
            DrawText(TextFormat("Bounds: %.0f×%.0f", bounds.width, bounds.height), 
                     815, 725, 18, GRAY);
            DrawText(TextFormat("Center: (%.0f, %.0f)", center.x, center.y), 
                     815, 750, 18, GRAY);
            
            DrawFPS(1150, 20);
            
        EndDrawing();
    }
    
    CloseWindow();
    
    // ═══════════════════════════════════════════════════════════
    // Résumé
    // ═══════════════════════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  RÉSULTATS\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    printf("Tests automatiques: %d/%d\n", passedTests, totalTests);
    printf("Tests visuels:      %d inside, %d outside\n\n", 
           testPointsInside, testPointsOutside);
    
    if (passedTests == totalTests) {
        printf("✅ Tous les tests passés !\n");
        printf("   - Point-in-quad fonctionne correctement\n");
        printf("   - Bounding box correct\n");
        printf("   - Centre et aire corrects\n");
        printf("   - Fonctionne avec quads déformés\n\n");
    } else {
        printf("❌ Certains tests ont échoué (%d/%d)\n\n", 
               totalTests - passedTests, totalTests);
    }
    
    return (passedTests == totalTests) ? 0 : 1;
}
