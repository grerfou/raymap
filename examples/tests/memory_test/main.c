/**********************************************************************************************
*
*   RayMap Memory Management Tests - COMPLETE SUITE
*
*   DESCRIPTION:
*       Comprehensive test suite for validating memory management in RayMap.
*       Tests allocation, deallocation, stress scenarios, and edge cases.
*
*   USAGE:
*       Compile: gcc -fsanitize=address -g test_memory.c -o test_memory -lraylib -lm
*       Run: ./test_memory
*       Valgrind: valgrind --leak-check=full --show-leak-kinds=all ./test_memory
*
**********************************************************************************************/
/*
#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// Test result tracking
static int g_testsRun = 0;
static int g_testsPassed = 0;
static int g_testsFailed = 0;

// Macro helpers
#define TEST_START(name) \
    do { \
        printf("  [%d] %s... ", ++g_testsRun, name); \
        fflush(stdout); \
    } while(0)

#define TEST_PASS() \
    do { \
        printf("✓ PASS\n"); \
        g_testsPassed++; \
    } while(0)

#define TEST_FAIL(reason) \
    do { \
        printf("✗ FAIL: %s\n", reason); \
        g_testsFailed++; \
    } while(0)

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            TEST_FAIL(msg); \
            return; \
        } \
    } while(0)

//--------------------------------------------------------------------------------------------
// Basic Tests
//--------------------------------------------------------------------------------------------

void test_create_destroy_bilinear() {
    TEST_START("Create/Destroy Bilinear Surface");
    
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_create_destroy_homography() {
    TEST_START("Create/Destroy Homography Surface");
    
    RM_Surface *surface = RM_CreateSurface(1024, 768, RM_MAP_HOMOGRAPHY);
    ASSERT(surface != NULL, "Failed to create surface");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_null_safety() {
    TEST_START("NULL Safety Checks");
    
    // Suppress warnings for this test only
    SetTraceLogLevel(LOG_ERROR);
    
    // All these should NOT crash
    RM_DestroySurface(NULL);
    RM_SetMeshResolution(NULL, 16, 16);
    RM_DrawSurface(NULL);
    RM_BeginSurface(NULL);
    RM_EndSurface(NULL);
    
    RM_Quad quad = RM_GetQuad(NULL);
    (void)quad;
    
    int w, h;
    RM_GetSurfaceSize(NULL, &w, &h);
    
    // Restore log level
    SetTraceLogLevel(LOG_WARNING);
    
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Stress Tests
//--------------------------------------------------------------------------------------------

void test_create_destroy_stress_1000() {
    TEST_START("Create/Destroy Stress (1000 iterations)");
    
    for (int i = 0; i < 1000; i++) {
        RM_Surface *surface = RM_CreateSurface(256, 256, RM_MAP_BILINEAR);
        if (!surface) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Failed at iteration %d", i);
            TEST_FAIL(msg);
            return;
        }
        RM_DestroySurface(surface);
    }
    
    TEST_PASS();
}

void test_alternating_modes_stress() {
    TEST_START("Alternating Modes Stress (500 iterations)");
    
    for (int i = 0; i < 500; i++) {
        RM_MapMode mode = (i % 2 == 0) ? RM_MAP_BILINEAR : RM_MAP_HOMOGRAPHY;
        RM_Surface *surface = RM_CreateSurface(512, 384, mode);
        
        if (!surface) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Failed at iteration %d", i);
            TEST_FAIL(msg);
            return;
        }
        
        RM_DestroySurface(surface);
    }
    
    TEST_PASS();
}

void test_various_sizes_stress() {
    TEST_START("Various Sizes Stress");
    
    int sizes[][2] = {
        {64, 64}, {128, 128}, {256, 256}, {512, 512},
        {800, 600}, {1024, 768}, {1280, 720}, {1920, 1080},
        {100, 200}, {300, 150}, {640, 480}
    };
    
    int count = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < count; i++) {
        RM_Surface *surface = RM_CreateSurface(sizes[i][0], sizes[i][1], RM_MAP_BILINEAR);
        
        if (!surface) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Failed for size %dx%d", sizes[i][0], sizes[i][1]);
            TEST_FAIL(msg);
            return;
        }
        
        RM_DestroySurface(surface);
    }
    
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Mesh Resolution Tests
//--------------------------------------------------------------------------------------------

void test_mesh_resolution_change() {
    TEST_START("Mesh Resolution Change (Reallocation)");
    
    RM_Surface *surface = RM_CreateSurface(512, 512, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // Change resolution multiple times (triggers mesh regeneration)
    int resolutions[] = {4, 8, 16, 24, 32, 16, 8};
    
    for (size_t i = 0; i < sizeof(resolutions) / sizeof(int); i++) {
        RM_SetMeshResolution(surface, resolutions[i], resolutions[i]);
        
        // Force mesh update by modifying quad
        RM_Quad quad = RM_GetQuad(surface);
        quad.topLeft.x += 0.1f;
        bool result = RM_SetQuad(surface, quad);
        
        if (!result) {
            TEST_FAIL("SetQuad failed during resolution change");
            RM_DestroySurface(surface);
            return;
        }
    }
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_mesh_resolution_limits() {
    TEST_START("Mesh Resolution Limits");
    
    RM_Surface *surface = RM_CreateSurface(512, 512, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // Test upper limit (should be clamped to 64)
    RM_SetMeshResolution(surface, 100, 100);
    
    int cols, rows;
    RM_GetMeshResolution(surface, &cols, &rows);
    
    ASSERT(cols == 64 && rows == 64, "Resolution not clamped to maximum (64)");
    
    // Test lower limit (should be clamped to 4)
    RM_SetMeshResolution(surface, 2, 2);
    RM_GetMeshResolution(surface, &cols, &rows);
    
    ASSERT(cols == 4 && rows == 4, "Resolution not clamped to minimum (4)");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_mesh_excessive_resolution() {
    TEST_START("Mesh Excessive Resolution (>256 should fail)");
    
    RM_Surface *surface = RM_CreateSurface(512, 512, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // This should be rejected (too high) after clamping
    // But with our fix, SetMeshResolution clamps to 64 max
    // So we test that internally we don't crash with huge numbers
    RM_SetMeshResolution(surface, 300, 300);
    
    // Surface should still be valid
    int cols, rows;
    RM_GetMeshResolution(surface, &cols, &rows);
    
    ASSERT(cols == 64 && rows == 64, "Excessive resolution not handled");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Quad Manipulation Tests
//--------------------------------------------------------------------------------------------

void test_quad_modification_stress() {
    TEST_START("Quad Modification Stress (100 changes)");
    
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    ASSERT(surface != NULL, "Failed to create surface");
    
    for (int i = 0; i < 100; i++) {
        RM_Quad quad = {
            { 50.0f + i, 50.0f + i },
            { 750.0f - i, 50.0f + i },
            { 750.0f - i, 550.0f - i },
            { 50.0f + i, 550.0f - i }
        };
        
        bool result = RM_SetQuad(surface, quad);
        if (!result) {
            char msg[64];
            snprintf(msg, sizeof(msg), "SetQuad failed at iteration %d", i);
            TEST_FAIL(msg);
            RM_DestroySurface(surface);
            return;
        }
    }
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_degenerate_quad_rejection() {
    TEST_START("Degenerate Quad Rejection");
    
    // Suppress warnings for this test
    SetTraceLogLevel(LOG_ERROR);
    
    RM_Surface *surface = RM_CreateSurface(512, 512, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // Try to set quad with coincident corners (should be rejected)
    RM_Quad degenerate = {
        { 100.0f, 100.0f },
        { 100.0f, 100.0f },  // Same as topLeft
        { 200.0f, 200.0f },
        { 50.0f, 200.0f }
    };
    
    bool result = RM_SetQuad(surface, degenerate);
    ASSERT(!result, "Degenerate quad was accepted (should be rejected)");
    
    // Try quad with very small area
    RM_Quad tiny = {
        { 100.0f, 100.0f },
        { 101.0f, 100.0f },
        { 101.0f, 101.0f },
        { 100.0f, 101.0f }
    };
    
    result = RM_SetQuad(surface, tiny);
    ASSERT(!result, "Tiny quad was accepted (area < 100)");
    
    // Restore log level
    SetTraceLogLevel(LOG_WARNING);
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Mode Switching Tests
//--------------------------------------------------------------------------------------------

void test_mode_switching() {
    TEST_START("Mode Switching (Bilinear ↔ Homography)");
    
    RM_Surface *surface = RM_CreateSurface(640, 480, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // Switch modes multiple times
    for (int i = 0; i < 10; i++) {
        RM_MapMode newMode = (i % 2 == 0) ? RM_MAP_HOMOGRAPHY : RM_MAP_BILINEAR;
        RM_SetMapMode(surface, newMode);
        
        RM_MapMode current = RM_GetMapMode(surface);
        if (current != newMode) {
            TEST_FAIL("Mode not properly switched");
            RM_DestroySurface(surface);
            return;
        }
    }
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Edge Case Tests
//--------------------------------------------------------------------------------------------

void test_minimum_size_surface() {
    TEST_START("Minimum Size Surface (1x1)");
    
    RM_Surface *surface = RM_CreateSurface(1, 1, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create 1x1 surface");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_maximum_size_surface() {
    TEST_START("Maximum Size Surface (8192x8192)");
    
    RM_Surface *surface = RM_CreateSurface(8192, 8192, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create 8192x8192 surface");
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

void test_invalid_size_rejection() {
    TEST_START("Invalid Size Rejection");
    
    // Suppress errors for this test
    SetTraceLogLevel(LOG_NONE);
    
    // Negative sizes
    RM_Surface *s1 = RM_CreateSurface(-100, 100, RM_MAP_BILINEAR);
    ASSERT(s1 == NULL, "Negative width accepted");
    
    RM_Surface *s2 = RM_CreateSurface(100, -100, RM_MAP_BILINEAR);
    ASSERT(s2 == NULL, "Negative height accepted");
    
    // Zero sizes
    RM_Surface *s3 = RM_CreateSurface(0, 100, RM_MAP_BILINEAR);
    ASSERT(s3 == NULL, "Zero width accepted");
    
    RM_Surface *s4 = RM_CreateSurface(100, 0, RM_MAP_BILINEAR);
    ASSERT(s4 == NULL, "Zero height accepted");
    
    // Too large
    RM_Surface *s5 = RM_CreateSurface(10000, 100, RM_MAP_BILINEAR);
    ASSERT(s5 == NULL, "Width > 8192 accepted");
    
    RM_Surface *s6 = RM_CreateSurface(100, 10000, RM_MAP_BILINEAR);
    ASSERT(s6 == NULL, "Height > 8192 accepted");
    
    // Restore log level
    SetTraceLogLevel(LOG_WARNING);
    
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Integration Tests
//--------------------------------------------------------------------------------------------

void test_full_workflow() {
    TEST_START("Full Workflow Integration");
    
    // Create surface
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
    ASSERT(surface != NULL, "Failed to create surface");
    
    // Setup calibration
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    // Modify quad
    RM_Quad quad = {
        { 100.0f, 100.0f },
        { 700.0f, 100.0f },
        { 700.0f, 500.0f },
        { 100.0f, 500.0f }
    };
    bool result = RM_SetQuad(surface, quad);
    ASSERT(result, "SetQuad failed");
    
    // Change resolution
    RM_SetMeshResolution(surface, 24, 24);
    
    // Switch mode
    RM_SetMapMode(surface, RM_MAP_HOMOGRAPHY);
    
    // Reset quad
    RM_ResetCalibrationQuad(&calib, 800, 600);
    
    RM_DestroySurface(surface);
    TEST_PASS();
}

//--------------------------------------------------------------------------------------------
// Main Test Runner
//--------------------------------------------------------------------------------------------

void print_header() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║        RAYMAP MEMORY MANAGEMENT TEST SUITE                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_results() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                     TEST RESULTS                           ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %-4d                                       ║\n", g_testsRun);
    printf("║  Passed:       %-4d  ✓                                    ║\n", g_testsPassed);
    printf("║  Failed:       %-4d  ✗                                    ║\n", g_testsFailed);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    if (g_testsFailed == 0) {
        printf("║  STATUS: ALL TESTS PASSED ✓✓✓                             ║\n");
    } else {
        printf("║  STATUS: SOME TESTS FAILED ✗✗✗                            ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

int main(void) {
    // Initialize raylib (required for RayMap)
    InitWindow(800, 600, "RayMap Memory Tests");
    SetTraceLogLevel(LOG_WARNING);  // Reduce console noise
    
    print_header();
    
    // === BASIC TESTS ===
    printf("┌─ BASIC TESTS ─────────────────────────────────────────────┐\n");
    test_create_destroy_bilinear();
    test_create_destroy_homography();
    test_null_safety();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === STRESS TESTS ===
    printf("┌─ STRESS TESTS ────────────────────────────────────────────┐\n");
    test_create_destroy_stress_1000();
    test_alternating_modes_stress();
    test_various_sizes_stress();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === MESH TESTS ===
    printf("┌─ MESH RESOLUTION TESTS ───────────────────────────────────┐\n");
    test_mesh_resolution_change();
    test_mesh_resolution_limits();
    test_mesh_excessive_resolution();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === QUAD TESTS ===
    printf("┌─ QUAD MANIPULATION TESTS ─────────────────────────────────┐\n");
    test_quad_modification_stress();
    test_degenerate_quad_rejection();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === MODE TESTS ===
    printf("┌─ MODE SWITCHING TESTS ────────────────────────────────────┐\n");
    test_mode_switching();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === EDGE CASES ===
    printf("┌─ EDGE CASE TESTS ─────────────────────────────────────────┐\n");
    test_minimum_size_surface();
    test_maximum_size_surface();
    test_invalid_size_rejection();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    // === INTEGRATION ===
    printf("┌─ INTEGRATION TESTS ───────────────────────────────────────┐\n");
    test_full_workflow();
    printf("└───────────────────────────────────────────────────────────┘\n\n");
    
    print_results();
    
    printf("MEMORY CHECK INSTRUCTIONS:\n");
    printf("  1. Compile with: gcc -fsanitize=address -g test_memory.c -o test_memory -lraylib -lm\n");
    printf("  2. Run with Valgrind: valgrind --leak-check=full ./test_memory\n");
    printf("  3. Expected: \"All heap blocks were freed -- no leaks are possible\"\n");
    printf("\n");
    
    CloseWindow();
    
    return (g_testsFailed == 0) ? 0 : 1;
}
*/

/**********************************************************************************************
*
*   RayMap - Multi-Surface Projection Mapping Example
*
*   DESCRIPTION:
*       Demonstrates simultaneous projection mapping on 3 surfaces with different content.
*       Perfect example for video mapping installations and multi-projector setups.
*
*   FEATURES:
*       - 3 independent surfaces with unique content
*       - Interactive calibration (TAB to toggle)
*       - Different mapping modes per surface
*       - Real-time content updates
*       - Performance stats
*
*   CONTROLS:
*       TAB     - Toggle calibration mode
*       1/2/3   - Select surface to calibrate
*       R       - Reset selected surface
*       ESC     - Exit
*
*   COMPILATION:
*       gcc multi_mapping.c -o multi_mapping -lraylib -lm
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>
#include <math.h>

//--------------------------------------------------------------------------------------------
// Program Constants
//--------------------------------------------------------------------------------------------
#define SCREEN_WIDTH    1920
#define SCREEN_HEIGHT   1080

#define SURFACE_WIDTH   640
#define SURFACE_HEIGHT  480

//--------------------------------------------------------------------------------------------
// Surface Content Generators
//--------------------------------------------------------------------------------------------

// Draw animated gradient on surface 1
void DrawSurface1Content(float time)
{
    // Animated gradient
    for (int y = 0; y < SURFACE_HEIGHT; y++) {
        float t = (float)y / SURFACE_HEIGHT;
        float hue = fmodf(time * 30.0f + t * 360.0f, 360.0f);
        Color color = ColorFromHSV(hue, 0.8f, 0.9f);
        DrawRectangle(0, y, SURFACE_WIDTH, 1, color);
    }

    // Centered text
    const char *text = "SURFACE 1\nBILINEAR";
    int fontSize = 60;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
    DrawText(text,
             (SURFACE_WIDTH - textSize.x) / 2,
             (SURFACE_HEIGHT - textSize.y) / 2,
             fontSize, WHITE);

    // Rotating circle
    float angle = time * 90.0f;
    float radius = 100.0f;
    float cx = SURFACE_WIDTH / 2 + cosf(angle * DEG2RAD) * radius;
    float cy = SURFACE_HEIGHT / 2 + sinf(angle * DEG2RAD) * radius;
    DrawCircle(cx, cy, 30, YELLOW);
}

// Draw particle system on surface 2
void DrawSurface2Content(float time)
{
    ClearBackground((Color){10, 10, 30, 255});

    // Particle effect
    int particleCount = 50;
    for (int i = 0; i < particleCount; i++) {
        float angle = (float)i / particleCount * 360.0f + time * 50.0f;
        float radius = 150.0f + sinf(time * 2.0f + i) * 50.0f;

        float x = SURFACE_WIDTH / 2 + cosf(angle * DEG2RAD) * radius;
        float y = SURFACE_HEIGHT / 2 + sinf(angle * DEG2RAD) * radius;

        float size = 5.0f + sinf(time * 3.0f + i * 0.5f) * 3.0f;
        Color color = ColorFromHSV(fmodf(angle + time * 30.0f, 360.0f), 1.0f, 1.0f);

        DrawCircle(x, y, size, color);
    }

    // Title
    const char *text = "SURFACE 2\nHOMOGRAPHY";
    int fontSize = 50;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
    DrawText(text,
             (SURFACE_WIDTH - textSize.x) / 2,
             20,
             fontSize, WHITE);
}

// Draw video-style content on surface 3
void DrawSurface3Content(float time)
{
    // Animated checkerboard
    int cellSize = 40;
    for (int y = 0; y < SURFACE_HEIGHT; y += cellSize) {
        for (int x = 0; x < SURFACE_WIDTH; x += cellSize) {
            int index = (x / cellSize + y / cellSize) % 2;
            float brightness = 0.5f + 0.5f * sinf(time * 3.0f + x * 0.01f + y * 0.01f);
            Color color = index ?
                (Color){255 * brightness, 100 * brightness, 100 * brightness, 255} :
                (Color){100 * brightness, 100 * brightness, 255 * brightness, 255};
            DrawRectangle(x, y, cellSize, cellSize, color);
        }
    }

    // Wave effect text
    const char *text = "SURFACE 3";
    int fontSize = 70;
    int textWidth = MeasureText(text, fontSize);

    for (int i = 0; text[i] != '\0'; i++) {
        float wave = sinf(time * 5.0f + i * 0.5f) * 20.0f;
        char letter[2] = {text[i], '\0'};
        int letterWidth = MeasureText(letter, fontSize);

        DrawText(letter,
                 (SURFACE_WIDTH - textWidth) / 2 + i * (fontSize * 0.6f),
                 SURFACE_HEIGHT / 2 - fontSize / 2 + wave,
                 fontSize,
                 YELLOW);
    }

    // Mode indicator
    DrawText("BILINEAR", 20, SURFACE_HEIGHT - 40, 30, WHITE);
}

//--------------------------------------------------------------------------------------------
// Main Program
//--------------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayMap - Multi-Surface Mapping");
    SetTargetFPS(60);

    // Create 3 surfaces with different modes
    RM_Surface *surface1 = RM_CreateSurface(SURFACE_WIDTH, SURFACE_HEIGHT, RM_MAP_BILINEAR);
    RM_Surface *surface2 = RM_CreateSurface(SURFACE_WIDTH, SURFACE_HEIGHT, RM_MAP_HOMOGRAPHY);
    RM_Surface *surface3 = RM_CreateSurface(SURFACE_WIDTH, SURFACE_HEIGHT, RM_MAP_BILINEAR);

    if (!surface1 || !surface2 || !surface3) {
        TraceLog(LOG_ERROR, "Failed to create surfaces");
        CloseWindow();
        return -1;
    }

    // Position surfaces in screen space (initial quads)
    // Surface 1: Top-left area
    RM_Quad quad1 = {
        {100, 100},           // Top-left
        {700, 80},            // Top-right
        {720, 500},           // Bottom-right
        {80, 520}             // Bottom-left
    };
    RM_SetQuad(surface1, quad1);

    // Surface 2: Top-right area
    RM_Quad quad2 = {
        {800, 100},
        {1400, 120},
        {1380, 540},
        {820, 520}
    };
    RM_SetQuad(surface2, quad2);

    // Surface 3: Bottom-center area
    RM_Quad quad3 = {
        {450, 600},
        {1050, 580},
        {1070, 1000},
        {430, 1020}
    };
    RM_SetQuad(surface3, quad3);

    // Create calibration for each surface
    RM_Calibration calib1 = RM_CalibrationDefault(surface1);
    RM_Calibration calib2 = RM_CalibrationDefault(surface2);
    RM_Calibration calib3 = RM_CalibrationDefault(surface3);

    // All start disabled
    calib1.enabled = false;
    calib2.enabled = false;
    calib3.enabled = false;

    // Calibration state
    int selectedSurface = 1;  // 1, 2, or 3
    bool calibrationMode = false;

    // Main loop
    while (!WindowShouldClose())
    {
        float time = GetTime();

        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------

        // Toggle calibration mode
        if (IsKeyPressed(KEY_TAB)) {
            calibrationMode = !calibrationMode;
            calib1.enabled = false;
            calib2.enabled = false;
            calib3.enabled = false;

            if (calibrationMode) {
                // Enable calibration for selected surface
                if (selectedSurface == 1) calib1.enabled = true;
                else if (selectedSurface == 2) calib2.enabled = true;
                else if (selectedSurface == 3) calib3.enabled = true;
            }
        }

        // Select surface in calibration mode
        if (calibrationMode) {
            if (IsKeyPressed(KEY_ONE)) {
                selectedSurface = 1;
                calib1.enabled = true;
                calib2.enabled = false;
                calib3.enabled = false;
            }
            if (IsKeyPressed(KEY_TWO)) {
                selectedSurface = 2;
                calib1.enabled = false;
                calib2.enabled = true;
                calib3.enabled = false;
            }
            if (IsKeyPressed(KEY_THREE)) {
                selectedSurface = 3;
                calib1.enabled = false;
                calib2.enabled = false;
                calib3.enabled = true;
            }

            // Reset selected surface
            if (IsKeyPressed(KEY_R)) {
                if (selectedSurface == 1) RM_ResetQuad(surface1, SCREEN_WIDTH, SCREEN_HEIGHT);
                else if (selectedSurface == 2) RM_ResetQuad(surface2, SCREEN_WIDTH, SCREEN_HEIGHT);
                else if (selectedSurface == 3) RM_ResetQuad(surface3, SCREEN_WIDTH, SCREEN_HEIGHT);
            }
        }

        // Update calibrations
        RM_UpdateCalibration(&calib1);
        RM_UpdateCalibration(&calib2);
        RM_UpdateCalibration(&calib3);

        //----------------------------------------------------------------------------------
        // Draw Content to Surfaces
        //----------------------------------------------------------------------------------

        // Draw to surface 1
        RM_BeginSurface(surface1);
            DrawSurface1Content(time);
        RM_EndSurface(surface1);

        // Draw to surface 2
        RM_BeginSurface(surface2);
            DrawSurface2Content(time);
        RM_EndSurface(surface2);

        // Draw to surface 3
        RM_BeginSurface(surface3);
            DrawSurface3Content(time);
        RM_EndSurface(surface3);

        //----------------------------------------------------------------------------------
        // Draw to Screen
        //----------------------------------------------------------------------------------

        BeginDrawing();

            // Background
            ClearBackground(BLACK);

            // Draw all 3 mapped surfaces
            RM_DrawSurface(surface1);
            RM_DrawSurface(surface2);
            RM_DrawSurface(surface3);

            // Draw calibration overlays
            if (calibrationMode) {
                RM_DrawCalibration(calib1);
                RM_DrawCalibration(calib2);
                RM_DrawCalibration(calib3);

                // Highlight selected surface
                Color highlightColor = YELLOW;
                RM_Quad selectedQuad;
                if (selectedSurface == 1) selectedQuad = RM_GetQuad(surface1);
                else if (selectedSurface == 2) selectedQuad = RM_GetQuad(surface2);
                else selectedQuad = RM_GetQuad(surface3);

                DrawLineEx(selectedQuad.topLeft, selectedQuad.topRight, 4, highlightColor);
                DrawLineEx(selectedQuad.topRight, selectedQuad.bottomRight, 4, highlightColor);
                DrawLineEx(selectedQuad.bottomRight, selectedQuad.bottomLeft, 4, highlightColor);
                DrawLineEx(selectedQuad.bottomLeft, selectedQuad.topLeft, 4, highlightColor);
            }

            // UI
            if (calibrationMode) {
                DrawRectangle(0, 0, 400, 200, Fade(BLACK, 0.7f));
                DrawText("CALIBRATION MODE", 20, 20, 30, GREEN);
                DrawText(TextFormat("Selected: SURFACE %d", selectedSurface), 20, 60, 20, YELLOW);
                DrawText("Controls:", 20, 90, 20, WHITE);
                DrawText("  1/2/3 - Select surface", 20, 115, 18, LIGHTGRAY);
                DrawText("  R     - Reset surface", 20, 135, 18, LIGHTGRAY);
                DrawText("  TAB   - Exit calibration", 20, 155, 18, LIGHTGRAY);
            } else {
                DrawRectangle(0, 0, 350, 80, Fade(BLACK, 0.7f));
                DrawText("Multi-Surface Mapping", 20, 20, 25, WHITE);
                DrawText("Press TAB for calibration", 20, 50, 18, LIGHTGRAY);
            }

            // Performance
            DrawText(TextFormat("FPS: %d", GetFPS()), SCREEN_WIDTH - 100, 20, 20, LIME);

        EndDrawing();
    }

    // Cleanup
    RM_DestroySurface(surface1);
    RM_DestroySurface(surface2);
    RM_DestroySurface(surface3);

    CloseWindow();

    return 0;
}
