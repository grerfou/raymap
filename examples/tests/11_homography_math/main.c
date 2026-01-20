#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <math.h>

// Helper: afficher une matrice 3x3
void print_matrix(const char *name, float m[3][3]) {
    printf("%s:\n", name);
    for (int i = 0; i < 3; i++) {
        printf("  [%8.4f  %8.4f  %8.4f]\n", m[i][0], m[i][1], m[i][2]);
    }
    printf("\n");
}

// Helper: vérifier si deux floats sont approximativement égaux
bool float_equals(float a, float b, float epsilon) {
    return fabsf(a - b) < epsilon;
}

// Helper: vérifier si une matrice est identité
bool is_identity_matrix(float m[3][3], float epsilon) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float expected = (i == j) ? 1.0f : 0.0f;
            if (!float_equals(m[i][j], expected, epsilon)) {
                return false;
            }
        }
    }
    return true;
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Test 11: Homography Math - RayMap");
    SetTargetFPS(60);
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 11: Homographie - Validation Mathématique\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    int tests_passed = 0;
    int tests_total = 4;
    
    // ═══════════════════════════════════════════════════════════
    // TEST 1: Quad rectangle → matrice diagonale (scaling)
    // ═══════════════════════════════════════════════════════════

    printf("TEST 1: Quad rectangle → matrice diagonale (scaling)\n");
    printf("──────────────────────────────────────────────────────\n");

    RM_Surface *surface1 = RM_CreateSurface(400, 300, RM_MAP_HOMOGRAPHY);

    // Quad rectangle (0,0) → (400,300)
    RM_Quad rectQuad = {
        {0, 0},
        {400, 0},
        {0, 300},
        {400, 300}
    };

    RM_SetQuad(surface1, rectQuad);

    // Forcer le calcul de l'homographie
    RM_BeginSurface(surface1);
    ClearBackground(BLACK);
    RM_EndSurface(surface1);
    RM_DrawSurface(surface1);

    printf("Quad rectangle (0,0) → (400,300)\n");
    print_matrix("Homographie calculée", surface1->homography.m);

    // Vérifier que c'est une matrice diagonale (scaling pur)
    bool test1_passed = true;

    // 1. Vérifier termes non-diagonaux ≈ 0
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != j) {  // Termes non-diagonaux
                if (fabsf(surface1->homography.m[i][j]) > 0.01f) {
                    test1_passed = false;
                }
            }
        }
    }

    // 2. Vérifier h22 = 1
    if (fabsf(surface1->homography.m[2][2] - 1.0f) > 0.01f) {
        test1_passed = false;
    }

    // 3. Vérifier que les scales correspondent
    float expected_scale_x = 400.0f;
    float expected_scale_y = 300.0f;
    float actual_scale_x = surface1->homography.m[0][0];
    float actual_scale_y = surface1->homography.m[1][1];

    if (fabsf(actual_scale_x - expected_scale_x) > 1.0f ||
        fabsf(actual_scale_y - expected_scale_y) > 1.0f) {
        test1_passed = false;
    }

    if (test1_passed) {
        printf("✅ TEST 1 RÉUSSI: Matrice diagonale (scaling pur)\n");
        printf("   Scale X: %.1f (attendu: %.1f)\n", actual_scale_x, expected_scale_x);
        printf("   Scale Y: %.1f (attendu: %.1f)\n", actual_scale_y, expected_scale_y);
        printf("   Cette matrice transforme correctement (0,0)→(1,1) vers (0,0)→(400,300)\n");
        tests_passed++;
    } else {
        printf("❌ TEST 1 ÉCHOUÉ: Matrice n'est pas une scaling diagonale correcte\n");
    }
    printf("\n");

    RM_DestroySurface(surface1);
    
    // ═══════════════════════════════════════════════════════════
    // TEST 2: Quad déformé → matrice correcte (non-singulière)
    // ═══════════════════════════════════════════════════════════
    
    printf("TEST 2: Quad déformé → matrice correcte\n");
    printf("────────────────────────────────────────\n");
    
    RM_Surface *surface2 = RM_CreateSurface(400, 300, RM_MAP_HOMOGRAPHY);
    
    // Quad trapèze
    RM_Quad trapeze = {
        {100, 150},
        {700, 100},
        {50, 800},
        {750, 750}
    };
    
    RM_SetQuad(surface2, trapeze);
    
    // Forcer calcul
    RM_BeginSurface(surface2);
    ClearBackground(BLACK);
    RM_EndSurface(surface2);
    RM_DrawSurface(surface2);
    
    printf("Quad trapèze:\n");
    printf("  TL: (%.0f, %.0f)\n", trapeze.topLeft.x, trapeze.topLeft.y);
    printf("  TR: (%.0f, %.0f)\n", trapeze.topRight.x, trapeze.topRight.y);
    printf("  BL: (%.0f, %.0f)\n", trapeze.bottomLeft.x, trapeze.bottomLeft.y);
    printf("  BR: (%.0f, %.0f)\n\n", trapeze.bottomRight.x, trapeze.bottomRight.y);
    
    print_matrix("Homographie calculée", surface2->homography.m);
    
    // Vérifier que la matrice n'est pas singulière (déterminant != 0)
    float det = surface2->homography.m[0][0] * (surface2->homography.m[1][1] * surface2->homography.m[2][2] - 
                                                  surface2->homography.m[1][2] * surface2->homography.m[2][1])
              - surface2->homography.m[0][1] * (surface2->homography.m[1][0] * surface2->homography.m[2][2] - 
                                                  surface2->homography.m[1][2] * surface2->homography.m[2][0])
              + surface2->homography.m[0][2] * (surface2->homography.m[1][0] * surface2->homography.m[2][1] - 
                                                  surface2->homography.m[1][1] * surface2->homography.m[2][0]);
    
    bool test2_passed = (fabsf(det) > 0.0001f);
    
    printf("Déterminant: %.6f\n", det);
    
    if (test2_passed) {
        printf("✅ TEST 2 RÉUSSI: Matrice non-singulière (det ≠ 0)\n");
        tests_passed++;
    } else {
        printf("❌ TEST 2 ÉCHOUÉ: Matrice singulière (det ≈ 0)\n");
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    // TEST 3: ApplyHomography sur points connus
    // ═══════════════════════════════════════════════════════════
    
    printf("TEST 3: ApplyHomography sur points connus\n");
    printf("──────────────────────────────────────────\n");
    
    // Tester que les 4 coins se transforment correctement
    Vector2 test_points[4] = {
        {0, 0},   // Top-left (normalized)
        {1, 0},   // Top-right
        {0, 1},   // Bottom-left
        {1, 1}    // Bottom-right
    };
    
    Vector2 expected[4] = {
        trapeze.topLeft,
        trapeze.topRight,
        trapeze.bottomLeft,
        trapeze.bottomRight
    };
    
    bool test3_passed = true;
    float max_error = 0.0f;
    
    for (int i = 0; i < 4; i++) {
        // Appliquer l'homographie
        float u = test_points[i].x;
        float v = test_points[i].y;
        
        float x = surface2->homography.m[0][0] * u + surface2->homography.m[0][1] * v + surface2->homography.m[0][2];
        float y = surface2->homography.m[1][0] * u + surface2->homography.m[1][1] * v + surface2->homography.m[1][2];
        float w = surface2->homography.m[2][0] * u + surface2->homography.m[2][1] * v + surface2->homography.m[2][2];
        
        if (fabsf(w) > 0.000001f) {
            x /= w;
            y /= w;
        }
        
        Vector2 result = {x, y};
        
        // Calculer l'erreur
        float error_x = fabsf(result.x - expected[i].x);
        float error_y = fabsf(result.y - expected[i].y);
        float error = sqrtf(error_x * error_x + error_y * error_y);
        
        if (error > max_error) max_error = error;
        
        const char *corner_names[] = {"TL", "TR", "BL", "BR"};
        printf("Point %s (%.1f, %.1f):\n", corner_names[i], u, v);
        printf("  Attendu:  (%.1f, %.1f)\n", expected[i].x, expected[i].y);
        printf("  Calculé:  (%.1f, %.1f)\n", result.x, result.y);
        printf("  Erreur:   %.4f pixels\n\n", error);
        
        // Tolérance: 1 pixel
        if (error > 1.0f) {
            test3_passed = false;
        }
    }
    
    printf("Erreur maximale: %.4f pixels\n", max_error);
    
    if (test3_passed) {
        printf("✅ TEST 3 RÉUSSI: Transformation correcte (erreur < 1 pixel)\n");
        tests_passed++;
    } else {
        printf("❌ TEST 3 ÉCHOUÉ: Erreur trop importante (> 1 pixel)\n");
    }
    printf("\n");
    
    // ═══════════════════════════════════════════════════════════
    // TEST 4: Inverse de matrice correcte
    // ═══════════════════════════════════════════════════════════
    
    printf("TEST 4: Inverse de matrice correcte\n");
    printf("────────────────────────────────────\n");
    
    // Calculer H^-1
    float H[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            H[i][j] = surface2->homography.m[i][j];
        }
    }
    
    float det_H = H[0][0] * (H[1][1] * H[2][2] - H[1][2] * H[2][1])
                - H[0][1] * (H[1][0] * H[2][2] - H[1][2] * H[2][0])
                + H[0][2] * (H[1][0] * H[2][1] - H[1][1] * H[2][0]);
    
    if (fabsf(det_H) < 0.000001f) {
        printf("❌ TEST 4 ÉCHOUÉ: Matrice singulière, pas d'inverse\n\n");
    } else {
        float inv_det = 1.0f / det_H;
        
        float H_inv[3][3];
        H_inv[0][0] = (H[1][1] * H[2][2] - H[1][2] * H[2][1]) * inv_det;
        H_inv[0][1] = (H[0][2] * H[2][1] - H[0][1] * H[2][2]) * inv_det;
        H_inv[0][2] = (H[0][1] * H[1][2] - H[0][2] * H[1][1]) * inv_det;
        
        H_inv[1][0] = (H[1][2] * H[2][0] - H[1][0] * H[2][2]) * inv_det;
        H_inv[1][1] = (H[0][0] * H[2][2] - H[0][2] * H[2][0]) * inv_det;
        H_inv[1][2] = (H[0][2] * H[1][0] - H[0][0] * H[1][2]) * inv_det;
        
        H_inv[2][0] = (H[1][0] * H[2][1] - H[1][1] * H[2][0]) * inv_det;
        H_inv[2][1] = (H[0][1] * H[2][0] - H[0][0] * H[2][1]) * inv_det;
        H_inv[2][2] = (H[0][0] * H[1][1] - H[0][1] * H[1][0]) * inv_det;
        
        print_matrix("H inverse", H_inv);
        
        // Vérifier H · H^-1 = I
        float product[3][3] = {0};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    product[i][j] += H[i][k] * H_inv[k][j];
                }
            }
        }
        
        print_matrix("H · H^-1 (devrait être I)", product);
        
        bool test4_passed = is_identity_matrix(product, 0.01f);
        
        if (test4_passed) {
            printf("✅ TEST 4 RÉUSSI: H · H^-1 = I\n");
            tests_passed++;
        } else {
            printf("❌ TEST 4 ÉCHOUÉ: H · H^-1 ≠ I\n");
        }
        printf("\n");
    }
    
    RM_DestroySurface(surface2);
    
    // ═══════════════════════════════════════════════════════════
    // RÉSUMÉ
    // ═══════════════════════════════════════════════════════════
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  RÉSUMÉ DES TESTS\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Tests réussis: %d/%d\n", tests_passed, tests_total);
    
    if (tests_passed == tests_total) {
        printf("\n🎉 TOUS LES TESTS SONT PASSÉS ! 🎉\n");
        printf("L'homographie fonctionne correctement !\n");
    } else {
        printf("\n⚠️  %d test(s) ont échoué\n", tests_total - tests_passed);
        printf("Vérifiez l'implémentation de l'homographie.\n");
    }
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════════════════════
    // TEST VISUEL (comparaison MESH vs PERSPECTIVE)
    // ═══════════════════════════════════════════════════════════
    
    printf("Lancement du test visuel...\n");
    printf("Appuyez sur ESC pour quitter.\n\n");
    
    // Créer deux surfaces pour comparaison
    RM_Surface *surfaceMesh = RM_CreateSurface(400, 300, RM_MAP_BILINEAR);
    RM_Surface *surfacePersp = RM_CreateSurface(400, 300, RM_MAP_HOMOGRAPHY);
    
    RM_Quad visualQuad1 = {
        {100, 150},
        {600, 100},
        {50, 570},
        {650, 520}
    };
    RM_SetQuad(surfaceMesh, visualQuad1);
    
    RM_Quad visualQuad2 = {
        {700, 150},
        {1200, 100},
        {650, 570},
        {1250, 520}
    };
    RM_SetQuad(surfacePersp, visualQuad2);
    
    bool showInfo = true;
    
    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_I)) {
            showInfo = !showInfo;
        }
        
        // Dessiner dans les surfaces
        RM_BeginSurface(surfaceMesh);
            ClearBackground(DARKBLUE);
            DrawText("MESH", 150, 50, 40, YELLOW);
            
            // Grille + diagonales
            for (int x = 0; x < 400; x += 50) {
                DrawLine(x, 0, x, 300, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y < 300; y += 50) {
                DrawLine(0, y, 400, y, ColorAlpha(WHITE, 0.2f));
            }
            DrawLineEx((Vector2){0, 0}, (Vector2){400, 300}, 4.0f, GREEN);
            DrawLineEx((Vector2){400, 0}, (Vector2){0, 300}, 4.0f, ORANGE);
        RM_EndSurface(surfaceMesh);
        
        RM_BeginSurface(surfacePersp);
            ClearBackground(DARKGREEN);
            DrawText("PERSPECTIVE", 80, 50, 40, ORANGE);
            
            for (int x = 0; x < 400; x += 50) {
                DrawLine(x, 0, x, 300, ColorAlpha(WHITE, 0.2f));
            }
            for (int y = 0; y < 300; y += 50) {
                DrawLine(0, y, 400, y, ColorAlpha(WHITE, 0.2f));
            }
            DrawLineEx((Vector2){0, 0}, (Vector2){400, 300}, 4.0f, GREEN);
            DrawLineEx((Vector2){400, 0}, (Vector2){0, 300}, 4.0f, ORANGE);
        RM_EndSurface(surfacePersp);
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            RM_DrawSurface(surfaceMesh);
            RM_DrawSurface(surfacePersp);
            
            DrawLine(640, 0, 640, 720, GRAY);
            
            if (showInfo) {
                DrawRectangle(0, 0, 1280, 60, ColorAlpha(BLACK, 0.85f));
                DrawText("Test Visuel: MESH (gauche) vs PERSPECTIVE (droite)", 20, 15, 20, LIME);
                DrawText("👁️  DIAGONALES: Gauche=COURBES, Droite=DROITES", 20, 40, 16, YELLOW);
                
                DrawText("I: Toggle info | ESC: Quit", 1280 - 250, 720 - 25, 16, LIGHTGRAY);
            }
            
            DrawFPS(10, 720 - 30);
            
        EndDrawing();
    }
    
    RM_DestroySurface(surfaceMesh);
    RM_DestroySurface(surfacePersp);
    
    CloseWindow();
    
    return (tests_passed == tests_total) ? 0 : 1;
}
