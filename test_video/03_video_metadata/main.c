/*
 * Test 03_video_metadata - Étape 1.3 : Métadonnées vidéo
 * 
 * OBJECTIF:
 *   Valider RMV_GetVideoInfo() et l'extraction des métadonnées.
 * 
 * COMMANDE:
 *   make EXAMPLE=03_video_metadata run
 * 
 * CONFIGURATION:
 *   Éditer TEST_VIDEO_PATH ci-dessous
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "raylib.h"

#define RAYMAPVID_IMPLEMENTATION
#include "raymapvid.h"

// ============================================================================
// CONFIGURATION - Éditer ce chemin !
// ============================================================================
#define TEST_VIDEO_PATH "a.mp4"
// ============================================================================

int main(int argc, char **argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║    RayMapVid - Test 03: Métadonnées Vidéo                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Utiliser argument si fourni, sinon chemin par défaut
    const char *videoPath = (argc > 1) ? argv[1] : TEST_VIDEO_PATH;
    
    // Vérifier si fichier existe
    FILE *f = fopen(videoPath, "rb");
    if (!f) {
        printf(" ERROR: Video file not found\n\n");
        printf("Expected: %s\n", videoPath);
        printf("Edit TEST_VIDEO_PATH in source code or pass as argument\n\n");
        return 1;
    }
    fclose(f);
    
    int tests_passed = 0;
    int tests_total = 0;
    
    // Init Raylib
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(100, 100, "Test 03");
    
    // ========================================================================
    // Test 1: GetVideoInfo sur NULL
    // ========================================================================
    tests_total++;
    printf("[Test 1] GetVideoInfo on NULL video\n");
    
    RMV_VideoInfo null_info = RMV_GetVideoInfo(NULL);
    
    if (null_info.width == 0 && null_info.height == 0 && 
        null_info.duration == 0.0f && null_info.fps == 0.0f) {
        printf("  ✓ PASS: NULL video returns empty info\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Should return zeroed structure\n\n");
    }
    
    // ========================================================================
    // Test 2: Load vidéo et extraire métadonnées
    // ========================================================================
    tests_total++;
    printf("[Test 2] Load video and extract metadata\n");
    printf("  File: %s\n", videoPath);
    
    RMV_Video *video = RMV_LoadVideo(videoPath);
    
    if (!video || !RMV_IsVideoLoaded(video)) {
        printf("  ✗ FAIL: Could not load video\n\n");
        CloseWindow();
        return 1;
    }
    
    RMV_VideoInfo info = RMV_GetVideoInfo(video);
    
    printf("\n");
    printf("  Extracted metadata:\n");
    printf("    Resolution:  %dx%d\n", info.width, info.height);
    printf("    Duration:    %.2f seconds\n", info.duration);
    printf("    FPS:         %.2f\n", info.fps);
    printf("    Codec:       %s\n", info.codec ? info.codec : "NULL");
    printf("    Format:      %s\n", info.format ? info.format : "NULL");
    printf("    Has Audio:   %s\n", info.hasAudio ? "Yes" : "No");
    printf("    HW Accel:    %s\n", info.hwaccel == RMV_HWACCEL_NONE ? "None" : "Active");
    printf("\n");
    
    if (info.width > 0 && info.height > 0) {
        printf("  ✓ PASS: Metadata extracted\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid metadata\n\n");
    }
    
    // ========================================================================
    // Test 3: Validation résolution
    // ========================================================================
    tests_total++;
    printf("[Test 3] Resolution validation\n");
    
    if (info.width > 0 && info.height > 0 && 
        info.width <= 8192 && info.height <= 8192) {
        printf("  ✓ PASS: Resolution valid (%dx%d)\n\n", info.width, info.height);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid resolution\n\n");
    }
    
    // ========================================================================
    // Test 4: Validation FPS
    // ========================================================================
    tests_total++;
    printf("[Test 4] FPS validation\n");
    
    if (info.fps > 0.0f && info.fps <= 240.0f) {
        printf("  ✓ PASS: FPS valid (%.2f)\n\n", info.fps);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid FPS (%.2f)\n\n", info.fps);
    }
    
    // ========================================================================
    // Test 5: Validation duration
    // ========================================================================
    tests_total++;
    printf("[Test 5] Duration validation\n");
    
    if (info.duration >= 0.0f) {
        printf("  ✓ PASS: Duration valid (%.2f s)\n\n", info.duration);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid duration\n\n");
    }
    
    // ========================================================================
    // Test 6: Validation codec name
    // ========================================================================
    tests_total++;
    printf("[Test 6] Codec name validation\n");
    
    if (info.codec != NULL && strlen(info.codec) > 0) {
        printf("  ✓ PASS: Codec name present (%s)\n\n", info.codec);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Codec name missing\n\n");
    }
    
    // ========================================================================
    // Test 7: Validation format name
    // ========================================================================
    tests_total++;
    printf("[Test 7] Format name validation\n");
    
    if (info.format != NULL && strlen(info.format) > 0) {
        printf("  ✓ PASS: Format name present (%s)\n\n", info.format);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Format name missing\n\n");
    }
    
    // ========================================================================
    // Test 8: Multiple GetVideoInfo calls
    // ========================================================================
    tests_total++;
    printf("[Test 8] Multiple GetVideoInfo calls\n");
    
    RMV_VideoInfo info2 = RMV_GetVideoInfo(video);
    RMV_VideoInfo info3 = RMV_GetVideoInfo(video);
    
    if (info2.width == info.width && info3.width == info.width &&
        info2.height == info.height && info3.height == info.height &&
        fabs(info2.fps - info.fps) < 0.01f && fabs(info3.fps - info.fps) < 0.01f) {
        printf("  ✓ PASS: Consistent results\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Inconsistent results\n\n");
    }
    
    // Cleanup
    RMV_UnloadVideo(video);
    CloseWindow();
    
    // ========================================================================
    // Résultats
    // ========================================================================
    printf("════════════════════════════════════════════════════════════\n");
    printf("Résultats: %d/%d tests passed\n", tests_passed, tests_total);
    printf("════════════════════════════════════════════════════════════\n");
    
    if (tests_passed == tests_total) {
        printf("\n✓ ÉTAPE 1.3 VALIDÉE - Métadonnées OK!\n\n");
        
        printf("Next: Étape 1.4 - Texture access\n");
        printf("  make EXAMPLE=04_texture_access run\n\n");
        
        return 0;
    } else {
        printf("\n✗ FAIL - %d test(s) failed\n\n", tests_total - tests_passed);
        return 1;
    }
}
