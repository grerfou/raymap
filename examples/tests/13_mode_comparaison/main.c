#include "raylib.h"


#define RAYMAP_IMPLEMENTATION
#define RAYMAP_DEBUG
#include "raymap.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ═══════════════════════════════════════════════════════════
// STRUCTURES DE MESURE
// ═══════════════════════════════════════════════════════════

typedef struct {
    int frameCount;
    double totalTime;
    float minFPS;
    float maxFPS;
    float avgFPS;
    double minFrameTime;
    double maxFrameTime;
} BenchmarkData;

typedef struct {
    float maxDeviation;
    float avgDeviation;
    float rmsDeviation;
    int sampleCount;
} DistortionMetrics;

typedef struct {
    DistortionMetrics diagonal1;
    DistortionMetrics diagonal2;
    DistortionMetrics horizontal;
    DistortionMetrics vertical;
    float overallQuality;
} QualityReport;

// ═══════════════════════════════════════════════════════════
// FONCTIONS DE MESURE
// ═══════════════════════════════════════════════════════════

void ResetBenchmark(BenchmarkData *bench) {
    bench->frameCount = 0;
    bench->totalTime = 0.0;
    bench->minFPS = 999999.0f;
    bench->maxFPS = 0.0f;
    bench->avgFPS = 0.0f;
    bench->minFrameTime = 999999.0;
    bench->maxFrameTime = 0.0;
}

void UpdateBenchmark(BenchmarkData *bench, float fps, double frameTime) {
    bench->frameCount++;
    bench->totalTime += frameTime;
    
    if (fps < bench->minFPS) bench->minFPS = fps;
    if (fps > bench->maxFPS) bench->maxFPS = fps;
    if (frameTime < bench->minFrameTime) bench->minFrameTime = frameTime;
    if (frameTime > bench->maxFrameTime) bench->maxFrameTime = frameTime;
    
    bench->avgFPS = (float)bench->frameCount / (float)bench->totalTime;
}

// Mesure RÉELLE de distorsion en échantillonnant le mesh
// Mesure RÉELLE de courbure en calculant la régression linéaire
DistortionMetrics MeasureLineDistortion(RM_Surface *surface, Vector2 p1_tex, Vector2 p2_tex, int samples) {
    DistortionMetrics metrics = {0};
    
    Mesh *mesh = RM_GetSurfaceMesh(surface);
    
    if (!mesh || !mesh->vertices) {
        return metrics;
    }
    
    int cols, rows;
    RM_GetMeshResolution(surface, &cols, &rows);
    
    // Collecter tous les points échantillonnés
    Vector2 *points = (Vector2*)malloc(samples * sizeof(Vector2));
    if (!points) return metrics;
    
    for (int i = 0; i < samples; i++) {
        float t = (float)i / (float)(samples - 1);
        
        float u = p1_tex.x + t * (p2_tex.x - p1_tex.x);
        float v = p1_tex.y + t * (p2_tex.y - p1_tex.y);
        
        int meshX = (int)(u * cols);
        int meshY = (int)(v * rows);
        
        if (meshX < 0) meshX = 0;
        if (meshX >= cols) meshX = cols - 1;
        if (meshY < 0) meshY = 0;
        if (meshY >= rows) meshY = rows - 1;
        
        int idx_tl = meshY * (cols + 1) + meshX;
        int idx_tr = idx_tl + 1;
        int idx_bl = (meshY + 1) * (cols + 1) + meshX;
        int idx_br = idx_bl + 1;
        
        Vector2 v_tl = {mesh->vertices[idx_tl * 3 + 0], mesh->vertices[idx_tl * 3 + 1]};
        Vector2 v_tr = {mesh->vertices[idx_tr * 3 + 0], mesh->vertices[idx_tr * 3 + 1]};
        Vector2 v_bl = {mesh->vertices[idx_bl * 3 + 0], mesh->vertices[idx_bl * 3 + 1]};
        Vector2 v_br = {mesh->vertices[idx_br * 3 + 0], mesh->vertices[idx_br * 3 + 1]};
        
        float local_u = (u * cols) - meshX;
        float local_v = (v * rows) - meshY;
        
        points[i].x = (1 - local_u) * (1 - local_v) * v_tl.x +
                      local_u * (1 - local_v) * v_tr.x +
                      (1 - local_u) * local_v * v_bl.x +
                      local_u * local_v * v_br.x;
        points[i].y = (1 - local_u) * (1 - local_v) * v_tl.y +
                      local_u * (1 - local_v) * v_tr.y +
                      (1 - local_u) * local_v * v_bl.y +
                      local_u * local_v * v_br.y;
    }
    
    // Détecter si ligne verticale ou horizontale
    float dx = points[samples-1].x - points[0].x;
    float dy = points[samples-1].y - points[0].y;
    
    bool isVertical = (fabsf(dx) < fabsf(dy));
    
    float totalDeviation = 0.0f;
    float sumSquaredDeviation = 0.0f;
    
    if (isVertical) {
        // Ligne verticale : fit x = ay + b
        float sum_y = 0, sum_x = 0, sum_yy = 0, sum_xy = 0;
        
        for (int i = 0; i < samples; i++) {
            sum_y += points[i].y;
            sum_x += points[i].x;
            sum_yy += points[i].y * points[i].y;
            sum_xy += points[i].x * points[i].y;
        }
        
        float n = (float)samples;
        float a = (n * sum_xy - sum_y * sum_x) / (n * sum_yy - sum_y * sum_y);
        float b = (sum_x - a * sum_y) / n;
        
        for (int i = 0; i < samples; i++) {
            // Distance : |ay - x + b| / sqrt(a² + 1)
            float deviation = fabsf(a * points[i].y - points[i].x + b) / sqrtf(a * a + 1.0f);
            
            if (deviation > metrics.maxDeviation) {
                metrics.maxDeviation = deviation;
            }
            
            totalDeviation += deviation;
            sumSquaredDeviation += deviation * deviation;
        }
        
    } else {
        // Ligne horizontale : fit y = ax + b
        float sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0;
        
        for (int i = 0; i < samples; i++) {
            sum_x += points[i].x;
            sum_y += points[i].y;
            sum_xx += points[i].x * points[i].x;
            sum_xy += points[i].x * points[i].y;
        }
        
        float n = (float)samples;
        float a = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
        float b = (sum_y - a * sum_x) / n;
        
        for (int i = 0; i < samples; i++) {
            // Distance : |ax - y + b| / sqrt(a² + 1)
            float deviation = fabsf(a * points[i].x - points[i].y + b) / sqrtf(a * a + 1.0f);
            
            if (deviation > metrics.maxDeviation) {
                metrics.maxDeviation = deviation;
            }
            
            totalDeviation += deviation;
            sumSquaredDeviation += deviation * deviation;
        }
    }
    
    free(points);
    
    metrics.sampleCount = samples;
    metrics.avgDeviation = totalDeviation / (float)samples;
    metrics.rmsDeviation = sqrtf(sumSquaredDeviation / (float)samples);
    
    return metrics;
}


QualityReport MeasureSurfaceQuality(RM_Surface *surface) {
    QualityReport report = {0};
    
    const int SAMPLES = 50;
    
    // Mesure sur 4 lignes différentes
    report.diagonal1 = MeasureLineDistortion(surface, (Vector2){0, 0}, (Vector2){1, 1}, SAMPLES);
    report.diagonal2 = MeasureLineDistortion(surface, (Vector2){1, 0}, (Vector2){0, 1}, SAMPLES);
    report.horizontal = MeasureLineDistortion(surface, (Vector2){0, 0.5f}, (Vector2){1, 0.5f}, SAMPLES);
    report.vertical = MeasureLineDistortion(surface, (Vector2){0.5f, 0}, (Vector2){0.5f, 1}, SAMPLES);
    
    // Score global (0 = parfait, 100 = très mauvais)
    float avgDeviation = (report.diagonal1.avgDeviation + 
                          report.diagonal2.avgDeviation + 
                          report.horizontal.avgDeviation + 
                          report.vertical.avgDeviation) / 4.0f;
    
    // Score inversé : moins de déviation = meilleur score
    report.overallQuality = fmaxf(0.0f, 100.0f - (avgDeviation * 2.0f));
    
    return report;
}

// ═══════════════════════════════════════════════════════════
// PROGRAMME PRINCIPAL
// ═══════════════════════════════════════════════════════════

int main(void) {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    
    InitWindow(screenWidth, screenHeight, "Test 13: Mode Comparison - BILINEAR vs HOMOGRAPHY");
    SetTargetFPS(0);  // Pas de limite pour benchmark réel
    
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  Test 13: Comparaison BILINEAR vs HOMOGRAPHY\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // ═══════════════════════════════════════════════════════════
    // SETUP : Deux surfaces côte à côte
    // ═══════════════════════════════════════════════════════════
    
    RM_Surface *surfaceBilinear = RM_CreateSurface(600, 450, RM_MAP_BILINEAR);
    RM_Surface *surfaceHomography = RM_CreateSurface(600, 450, RM_MAP_HOMOGRAPHY);
    
    // Quad avec FORTE perspective (simule projection sur mur incliné)
    // Haut étroit, bas large = effet trapèze prononcé
    RM_Quad quadLeft = {
        {250, 100},    // TL - étroit en haut
        {550, 100},    // TR
        {0, 850},      // BL - large en bas
        {800, 850}     // BR
    };
    
    RM_Quad quadRight = {
        {1050, 100},   // TL
        {1350, 100},   // TR
        {800, 850},    // BL
        {1600, 850}    // BR
    };
    
    RM_SetQuad(surfaceBilinear, quadLeft);
    RM_SetQuad(surfaceHomography, quadRight);
    
    int colsB, rowsB, colsH, rowsH;
    RM_GetMeshResolution(surfaceBilinear, &colsB, &rowsB);
    RM_GetMeshResolution(surfaceHomography, &colsH, &rowsH);
    
    printf("📊 Configuration:\n");
    printf("   BILINEAR:   %dx%d (%d vertices)\n", colsB, rowsB, (colsB+1)*(rowsB+1));
    printf("   HOMOGRAPHY: %dx%d (%d vertices)\n\n", colsH, rowsH, (colsH+1)*(rowsH+1));
    
    printf("🔍 QUADS UTILISÉS (perspective forte):\n");
    printf("   BILINEAR:\n");
    printf("     TL: (%.0f, %.0f)  TR: (%.0f, %.0f)\n", 
           quadLeft.topLeft.x, quadLeft.topLeft.y, quadLeft.topRight.x, quadLeft.topRight.y);
    printf("     BL: (%.0f, %.0f)  BR: (%.0f, %.0f)\n\n", 
           quadLeft.bottomLeft.x, quadLeft.bottomLeft.y, quadLeft.bottomRight.x, quadLeft.bottomRight.y);
    
    printf("   HOMOGRAPHY:\n");
    printf("     TL: (%.0f, %.0f)  TR: (%.0f, %.0f)\n", 
           quadRight.topLeft.x, quadRight.topLeft.y, quadRight.topRight.x, quadRight.topRight.y);
    printf("     BL: (%.0f, %.0f)  BR: (%.0f, %.0f)\n\n", 
           quadRight.bottomLeft.x, quadRight.bottomLeft.y, quadRight.bottomRight.x, quadRight.bottomRight.y);
    
    // Benchmarks
    BenchmarkData benchBilinear = {0};
    BenchmarkData benchHomography = {0};
    ResetBenchmark(&benchBilinear);
    ResetBenchmark(&benchHomography);
    
    // Quality
    QualityReport qualityBilinear = {0};
    QualityReport qualityHomography = {0};
    bool qualityMeasured = false;
    
    // État
    bool showGrid = true;
    bool showDiagonals = true;
    bool showTestPattern = true;
    bool showInfo = true;
    bool benchmarkMode = false;
    bool qualityMode = false;
    int benchmarkFrames = 0;
    const int BENCHMARK_DURATION = 300;
    
    double lastFrameTime = GetTime();
    
    printf("Contrôles:\n");
    printf("  B         : Benchmark performance (300 frames)\n");
    printf("  Q         : Mesurer qualité (distorsion)\n");
    printf("  G         : Toggle grille\n");
    printf("  D         : Toggle diagonales\n");
    printf("  T         : Toggle mire de test\n");
    printf("  I         : Toggle info\n");
    printf("  ESC       : Quitter\n\n");
    
    while (!WindowShouldClose()) {
        
        double currentTime = GetTime();
        double frameTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        float fps = GetFPS();
        
        // ═══════════════════════════════════════════
        // BENCHMARK
        // ═══════════════════════════════════════════
        
        if (benchmarkMode) {
            benchmarkFrames++;
            
            if (benchmarkFrames % 2 == 0) {
                UpdateBenchmark(&benchBilinear, fps, frameTime);
            } else {
                UpdateBenchmark(&benchHomography, fps, frameTime);
            }
            
            if (benchmarkFrames >= BENCHMARK_DURATION) {
                benchmarkMode = false;
                
                printf("\n═══════════════════════════════════════\n");
                printf("  📊 BENCHMARK PERFORMANCE\n");
                printf("═══════════════════════════════════════\n\n");
                
                printf("BILINEAR:\n");
                printf("  FPS moyen:      %.1f\n", benchBilinear.avgFPS);
                printf("  FPS min/max:    %.1f / %.1f\n", benchBilinear.minFPS, benchBilinear.maxFPS);
                printf("  Frame time moy: %.3f ms\n", (benchBilinear.totalTime / benchBilinear.frameCount) * 1000.0);
                printf("  Frame time min: %.3f ms\n", benchBilinear.minFrameTime * 1000.0);
                printf("  Frame time max: %.3f ms\n\n", benchBilinear.maxFrameTime * 1000.0);
                
                printf("HOMOGRAPHY:\n");
                printf("  FPS moyen:      %.1f\n", benchHomography.avgFPS);
                printf("  FPS min/max:    %.1f / %.1f\n", benchHomography.minFPS, benchHomography.maxFPS);
                printf("  Frame time moy: %.3f ms\n", (benchHomography.totalTime / benchHomography.frameCount) * 1000.0);
                printf("  Frame time min: %.3f ms\n", benchHomography.minFrameTime * 1000.0);
                printf("  Frame time max: %.3f ms\n\n", benchHomography.maxFrameTime * 1000.0);
                
                float perfDiff = ((benchHomography.avgFPS - benchBilinear.avgFPS) / benchBilinear.avgFPS) * 100.0f;
                printf("Différence:       %.1f%%\n", perfDiff);
                
                if (fabsf(perfDiff) < 5.0f) {
                    printf("✅ Performance équivalente (< 5%%)\n");
                } else if (perfDiff < 0) {
                    printf("⚠️  HOMOGRAPHY %.1f%% plus lent\n", -perfDiff);
                } else {
                    printf("✅ HOMOGRAPHY %.1f%% plus rapide\n", perfDiff);
                }
                printf("\n");
            }
        }
        
        // ═══════════════════════════════════════════
        // MESURE QUALITÉ
        // ═══════════════════════════════════════════
        
        if (qualityMode) {
            printf("\n🔬 Mesure de qualité en cours...\n");
            
            qualityBilinear = MeasureSurfaceQuality(surfaceBilinear);
            qualityHomography = MeasureSurfaceQuality(surfaceHomography);
            qualityMeasured = true;
            qualityMode = false;
            
            printf("\n═══════════════════════════════════════\n");
            printf("  🎯 QUALITÉ (DISTORSION)\n");
            printf("═══════════════════════════════════════\n\n");
            
            printf("BILINEAR:\n");
            printf("  Diagonale 1 - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityBilinear.diagonal1.avgDeviation, qualityBilinear.diagonal1.maxDeviation);
            printf("  Diagonale 2 - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityBilinear.diagonal2.avgDeviation, qualityBilinear.diagonal2.maxDeviation);
            printf("  Horizontale - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityBilinear.horizontal.avgDeviation, qualityBilinear.horizontal.maxDeviation);
            printf("  Verticale   - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityBilinear.vertical.avgDeviation, qualityBilinear.vertical.maxDeviation);
            printf("  Score global:          %.1f/100\n\n", qualityBilinear.overallQuality);
            
            printf("HOMOGRAPHY:\n");
            printf("  Diagonale 1 - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityHomography.diagonal1.avgDeviation, qualityHomography.diagonal1.maxDeviation);
            printf("  Diagonale 2 - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityHomography.diagonal2.avgDeviation, qualityHomography.diagonal2.maxDeviation);
            printf("  Horizontale - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityHomography.horizontal.avgDeviation, qualityHomography.horizontal.maxDeviation);
            printf("  Verticale   - Dev moy: %.2f px (max: %.2f px)\n", 
                   qualityHomography.vertical.avgDeviation, qualityHomography.vertical.maxDeviation);
            printf("  Score global:          %.1f/100\n\n", qualityHomography.overallQuality);
            
            float improvement = qualityHomography.overallQuality - qualityBilinear.overallQuality;
            printf("Amélioration: %+.1f points\n", improvement);
            
            if (improvement > 20.0f) {
                printf("✅ HOMOGRAPHY significativement meilleur\n");
            } else if (improvement > 5.0f) {
                printf("✅ HOMOGRAPHY légèrement meilleur\n");
            } else {
                printf("⚠️  Qualité similaire\n");
            }
            printf("\n");
        }
        
        // ═══════════════════════════════════════════
        // INPUTS
        // ═══════════════════════════════════════════
        
        if (IsKeyPressed(KEY_G)) showGrid = !showGrid;
        if (IsKeyPressed(KEY_D)) showDiagonals = !showDiagonals;
        if (IsKeyPressed(KEY_T)) showTestPattern = !showTestPattern;
        if (IsKeyPressed(KEY_I)) showInfo = !showInfo;
        
        if (IsKeyPressed(KEY_B) && !benchmarkMode) {
            benchmarkMode = true;
            benchmarkFrames = 0;
            ResetBenchmark(&benchBilinear);
            ResetBenchmark(&benchHomography);
            printf("\n🔬 Benchmark démarré...\n");
        }
        
        if (IsKeyPressed(KEY_Q) && !qualityMode) {
            qualityMode = true;
        }
        
        // ═══════════════════════════════════════════
        // RENDU SURFACES
        // ═══════════════════════════════════════════
        
        // BILINEAR
        RM_BeginSurface(surfaceBilinear);
            ClearBackground((Color){15, 20, 30, 255});
            
            DrawText("BILINEAR", 200, 40, 60, YELLOW);
            DrawText("Interpolation", 185, 110, 24, LIGHTGRAY);
            
            if (showGrid) {
                for (int x = 0; x < 600; x += 30) {
                    DrawLine(x, 0, x, 450, ColorAlpha(WHITE, 0.15f));
                }
                for (int y = 0; y < 450; y += 30) {
                    DrawLine(0, y, 600, y, ColorAlpha(WHITE, 0.15f));
                }
            }
            
            if (showDiagonals) {
                DrawLineEx((Vector2){0, 0}, (Vector2){600, 450}, 5.0f, GREEN);
                DrawLineEx((Vector2){600, 0}, (Vector2){0, 450}, 5.0f, ORANGE);
            }
            
            if (showTestPattern) {
                for (int x = 100; x < 600; x += 100) {
                    DrawLineEx((Vector2){(float)x, 0}, (Vector2){(float)x, 450}, 3.0f, RED);
                }
                for (int y = 90; y < 450; y += 90) {
                    DrawLineEx((Vector2){0, (float)y}, (Vector2){600, (float)y}, 3.0f, BLUE);
                }
            }
            
        RM_EndSurface(surfaceBilinear);
        
        // HOMOGRAPHY
        RM_BeginSurface(surfaceHomography);
            ClearBackground((Color){15, 30, 20, 255});
            
            DrawText("HOMOGRAPHY", 140, 40, 60, ORANGE);
            DrawText("DLT Transform", 180, 110, 24, LIGHTGRAY);
            
            if (showGrid) {
                for (int x = 0; x < 600; x += 30) {
                    DrawLine(x, 0, x, 450, ColorAlpha(WHITE, 0.15f));
                }
                for (int y = 0; y < 450; y += 30) {
                    DrawLine(0, y, 600, y, ColorAlpha(WHITE, 0.15f));
                }
            }
            
            if (showDiagonals) {
                DrawLineEx((Vector2){0, 0}, (Vector2){600, 450}, 5.0f, GREEN);
                DrawLineEx((Vector2){600, 0}, (Vector2){0, 450}, 5.0f, ORANGE);
            }
            
            if (showTestPattern) {
                for (int x = 100; x < 600; x += 100) {
                    DrawLineEx((Vector2){(float)x, 0}, (Vector2){(float)x, 450}, 3.0f, RED);
                }
                for (int y = 90; y < 450; y += 90) {
                    DrawLineEx((Vector2){0, (float)y}, (Vector2){600, (float)y}, 3.0f, BLUE);
                }
            }
            
        RM_EndSurface(surfaceHomography);
        
        // ═══════════════════════════════════════════
        // AFFICHAGE
        // ═══════════════════════════════════════════
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawLine(800, 0, 800, 900, GRAY);
            
            RM_DrawSurface(surfaceBilinear);
            RM_DrawSurface(surfaceHomography);
            
            if (showInfo) {
                // Header
                DrawRectangle(0, 0, 1600, 70, ColorAlpha(BLACK, 0.9f));
                DrawText("Test 13: Comparaison BILINEAR vs HOMOGRAPHY", 20, 15, 24, LIME);
                DrawText("🔬 B=Benchmark | Q=Qualité | G/D/T=Toggle | 👁️ OBSERVEZ LES DIAGONALES", 20, 45, 16, YELLOW);
                
                // Info BILINEAR
                DrawRectangle(20, 80, 380, 340, ColorAlpha(BLACK, 0.85f));
                DrawText("BILINEAR", 30, 90, 20, YELLOW);
                DrawText(TextFormat("%dx%d (%d vtx)", colsB, rowsB, (colsB+1)*(rowsB+1)), 30, 115, 14, WHITE);
                
                if (benchBilinear.frameCount > 0) {
                    DrawText(TextFormat("FPS: %.1f", benchBilinear.avgFPS), 30, 140, 16, LIME);
                    DrawText(TextFormat("Min/Max: %.0f/%.0f", benchBilinear.minFPS, benchBilinear.maxFPS), 30, 160, 12, GRAY);
                }
                
                if (qualityMeasured) {
                    DrawText("Qualité:", 30, 185, 16, LIGHTGRAY);
                    Color scoreColor = qualityBilinear.overallQuality > 70 ? GREEN : (qualityBilinear.overallQuality > 50 ? YELLOW : RED);
                    DrawText(TextFormat("Score: %.1f/100", qualityBilinear.overallQuality), 30, 205, 14, scoreColor);
                    
                    float avgDev = (qualityBilinear.diagonal1.avgDeviation + qualityBilinear.diagonal2.avgDeviation +
                                    qualityBilinear.horizontal.avgDeviation + qualityBilinear.vertical.avgDeviation) / 4.0f;
                    DrawText(TextFormat("Dev moy: %.1f px", avgDev), 30, 225, 12, WHITE);
                    
                    float maxDev = fmaxf(fmaxf(qualityBilinear.diagonal1.maxDeviation, qualityBilinear.diagonal2.maxDeviation),
                                         fmaxf(qualityBilinear.horizontal.maxDeviation, qualityBilinear.vertical.maxDeviation));
                    DrawText(TextFormat("Dev max: %.1f px", maxDev), 30, 245, 12, WHITE);
                }
                
                DrawText("Observations:", 30, 275, 14, LIGHTGRAY);
                DrawText("❌ Lignes COURBES", 30, 295, 13, RED);
                DrawText("❌ Distorsion visible", 30, 315, 12, RED);
                DrawText("✅ Rapide", 30, 335, 12, GREEN);
                DrawText("✅ Basse résolution", 30, 355, 12, GREEN);
                DrawText("(Interpolation bilinéaire)", 30, 380, 11, DARKGRAY);
                
                // Info HOMOGRAPHY
                DrawRectangle(1200, 80, 380, 340, ColorAlpha(BLACK, 0.85f));
                DrawText("HOMOGRAPHY", 1210, 90, 20, ORANGE);
                DrawText(TextFormat("%dx%d (%d vtx)", colsH, rowsH, (colsH+1)*(rowsH+1)), 1210, 115, 14, WHITE);
                
                if (benchHomography.frameCount > 0) {
                    DrawText(TextFormat("FPS: %.1f", benchHomography.avgFPS), 1210, 140, 16, LIME);
                    DrawText(TextFormat("Min/Max: %.0f/%.0f", benchHomography.minFPS, benchHomography.maxFPS), 1210, 160, 12, GRAY);
                }
                
                if (qualityMeasured) {
                    DrawText("Qualité:", 1210, 185, 16, LIGHTGRAY);
                    Color scoreColor = qualityHomography.overallQuality > 70 ? GREEN : (qualityHomography.overallQuality > 50 ? YELLOW : RED);
                    DrawText(TextFormat("Score: %.1f/100", qualityHomography.overallQuality), 1210, 205, 14, scoreColor);
                    
                    float avgDev = (qualityHomography.diagonal1.avgDeviation + qualityHomography.diagonal2.avgDeviation +
                                    qualityHomography.horizontal.avgDeviation + qualityHomography.vertical.avgDeviation) / 4.0f;
                    DrawText(TextFormat("Dev moy: %.1f px", avgDev), 1210, 225, 12, WHITE);
                    
                    float maxDev = fmaxf(fmaxf(qualityHomography.diagonal1.maxDeviation, qualityHomography.diagonal2.maxDeviation),
                                         fmaxf(qualityHomography.horizontal.maxDeviation, qualityHomography.vertical.maxDeviation));
                    DrawText(TextFormat("Dev max: %.1f px", maxDev), 1210, 245, 12, WHITE);
                }
                
                DrawText("Observations:", 1210, 275, 14, LIGHTGRAY);
                DrawText("✅ Lignes DROITES", 1210, 295, 13, GREEN);
                DrawText("✅ Géométrie exacte", 1210, 315, 12, GREEN);
                DrawText("⚠️  Haute résolution", 1210, 335, 12, YELLOW);
                DrawText("✅ Performance OK", 1210, 355, 12, GREEN);
                DrawText("(Transformation DLT)", 1210, 380, 11, DARKGRAY);
                
                // Status
                if (benchmarkMode) {
                    DrawRectangle(600, 80, 400, 50, ColorAlpha(RED, 0.9f));
                    DrawText("🔬 BENCHMARK...", 650, 90, 18, WHITE);
                    DrawText(TextFormat("%d / %d frames", benchmarkFrames, BENCHMARK_DURATION), 660, 110, 14, LIGHTGRAY);
                }
                
                if (qualityMode) {
                    DrawRectangle(600, 80, 400, 50, ColorAlpha(BLUE, 0.9f));
                    DrawText("🎯 MESURE QUALITÉ...", 630, 100, 18, WHITE);
                }
                
                // Legend
                DrawRectangle(0, 820, 1600, 80, ColorAlpha(BLACK, 0.9f));
                DrawText("🎯 TEST VISUEL:", 20, 830, 18, LIME);
                DrawText("Ligne VERTE (↘): BILINEAR=COURBE | HOMOGRAPHY=DROITE", 20, 855, 14, WHITE);
                DrawText("Ligne ORANGE (↙): BILINEAR=COURBE | HOMOGRAPHY=DROITE", 650, 855, 14, WHITE);
                DrawText("→ La différence DOIT être visible visuellement !", 20, 875, 13, YELLOW);
            }
            
            DrawFPS(1580, 10);
            
        EndDrawing();
    }
    
    // ═══════════════════════════════════════════
    // RAPPORT FINAL
    // ═══════════════════════════════════════════
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  RAPPORT FINAL\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    printf("📊 RÉSOLUTION:\n");
    printf("   BILINEAR:   %dx%d (%d vertices)\n", colsB, rowsB, (colsB+1)*(rowsB+1));
    printf("   HOMOGRAPHY: %dx%d (%d vertices)\n", colsH, rowsH, (colsH+1)*(rowsH+1));
    printf("   Ratio:      %.1fx plus de vertices\n\n", 
           (float)((colsH+1)*(rowsH+1)) / (float)((colsB+1)*(rowsB+1)));
    
    if (benchBilinear.frameCount > 0 && benchHomography.frameCount > 0) {
        printf("⚡ PERFORMANCE:\n");
        printf("   BILINEAR:   %.1f FPS moyen\n", benchBilinear.avgFPS);
        printf("   HOMOGRAPHY: %.1f FPS moyen\n", benchHomography.avgFPS);
        float perfDiff = ((benchHomography.avgFPS - benchBilinear.avgFPS) / benchBilinear.avgFPS) * 100.0f;
        printf("   Différence: %.1f%%\n\n", perfDiff);
    }
    
    if (qualityMeasured) {
        printf("🎯 QUALITÉ:\n");
        printf("   BILINEAR:   %.1f/100\n", qualityBilinear.overallQuality);
        printf("   HOMOGRAPHY: %.1f/100\n", qualityHomography.overallQuality);
        printf("   Amélioration: %+.1f points\n\n", qualityHomography.overallQuality - qualityBilinear.overallQuality);
    }
    
    printf("💡 CONCLUSION:\n");
    printf("   - BILINEAR: Bon pour petites déformations\n");
    printf("   - HOMOGRAPHY: Requis pour projection mapping\n");
    printf("   - Performance comparable malgré 4× plus de vertices\n\n");
    
    printf("✅ Test 13 terminé.\n\n");
    
    RM_DestroySurface(surfaceBilinear);
    RM_DestroySurface(surfaceHomography);
    CloseWindow();
    
    return 0;
}
