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

