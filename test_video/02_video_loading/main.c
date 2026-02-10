/*
 * Test 02_video_loading - Étape 1.2 : Chargement vidéo basique
 * 
 * OBJECTIF:
 *   Valider RMV_LoadVideo() et RMV_UnloadVideo() uniquement.
 *   Focus sur la gestion mémoire et les cas d'erreur.
 * 
 * COMMANDE:
 *   make EXAMPLE=02_video_loading run
 * 
 * CONFIGURATION:
 *   Éditer TEST_VIDEO_PATH ci-dessous avec le chemin de votre vidéo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

#define RAYMAPVID_IMPLEMENTATION
#include "raymapvid.h"

// ============================================================================
// CONFIGURATION - Éditer ce chemin !
// ============================================================================
#define TEST_VIDEO_PATH "a.mp4"
// Ou chemin absolu : "/home/user/Videos/test.mp4"
// ============================================================================

int main(int argc, char **argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║    RayMapVid - Test 02: Chargement Vidéo                  ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    int tests_passed = 0;
    int tests_total = 0;
    int tests_skipped = 0;
    
    // Utiliser argument si fourni, sinon chemin par défaut
    const char *videoPath = (argc > 1) ? argv[1] : TEST_VIDEO_PATH;
    
    // Vérifier si fichier existe
    FILE *f = fopen(videoPath, "rb");
    bool videoExists = (f != NULL);
    if (f) fclose(f);
    
    // Init Raylib (requis pour textures)
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(100, 100, "Test 02");
    
    // ========================================================================
    // Test 1: Load NULL pointer
    // ========================================================================
    tests_total++;
    printf("[Test 1] Load NULL pointer\n");
    
    RMV_Video *null_video = RMV_LoadVideo(NULL);
    
    if (null_video == NULL) {
        printf("  ✓ PASS: NULL filepath handled correctly\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Should return NULL for NULL filepath\n\n");
        RMV_UnloadVideo(null_video);
    }
    
    // ========================================================================
    // Test 2: Load fichier inexistant
    // ========================================================================
    tests_total++;
    printf("[Test 2] Load nonexistent file\n");
    
    RMV_Video *invalid_video = RMV_LoadVideo("nonexistent_file_xyz123.mp4");
    
    if (invalid_video == NULL) {
        printf("  ✓ PASS: Nonexistent file rejected\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Should return NULL for nonexistent file\n\n");
        RMV_UnloadVideo(invalid_video);
    }
    
    // ========================================================================
    // Test 3: Unload NULL
    // ========================================================================
    tests_total++;
    printf("[Test 3] Unload NULL video\n");
    
    RMV_UnloadVideo(NULL);
    
    printf("  ✓ PASS: Unload NULL didn't crash\n\n");
    tests_passed++;
    
    // ========================================================================
    // Test 4: Load vidéo valide
    // ========================================================================
    tests_total++;
    printf("[Test 4] Load valid video\n");
    
    if (videoExists) {
        printf("  File: %s\n", videoPath);
        
        RMV_Video *video = RMV_LoadVideo(videoPath);
        
        if (video != NULL && RMV_IsVideoLoaded(video)) {
            printf("  ✓ PASS: Video loaded successfully\n");
            printf("  ✓ PASS: RMV_IsVideoLoaded() returns true\n\n");
            tests_passed++;
            
            // Test 5: Unload NULL safety
            tests_total++;
            printf("[Test 5] Unload NULL safety\n");
            
            // Normal unload
            RMV_UnloadVideo(video);
            
            // Test that NULL is handled gracefully
            RMV_UnloadVideo(NULL);
            
            printf("  ✓ PASS: NULL handled correctly\n\n");
            tests_passed++;
            
        } else {
            printf("  ✗ FAIL: Failed to load video\n\n");
            if (video) RMV_UnloadVideo(video);
            
            // Skip test 5
            tests_total++;
            printf("[Test 5] Unload NULL safety\n");
            printf("  ⊘ SKIPPED: Video load failed\n\n");
            tests_skipped++;
        }
    } else {
        printf("  ⊘ SKIPPED: Video file not found\n");
        printf("    Expected: %s\n", videoPath);
        printf("    Edit TEST_VIDEO_PATH in source code\n\n");
        tests_skipped++;
        
        // Skip test 5
        tests_total++;
        printf("[Test 5] Unload NULL safety\n");
        printf("  ⊘ SKIPPED: No video file\n\n");
        tests_skipped++;
    }
    
    // ========================================================================
    // Test 6: Multiple load/unload cycles
    // ========================================================================
    tests_total++;
    printf("[Test 6] Load/Unload cycles (10x)\n");
    
    if (videoExists) {
        bool cycles_ok = true;
        
        for (int i = 0; i < 10; i++) {
            RMV_Video *v = RMV_LoadVideo(videoPath);
            if (!v || !RMV_IsVideoLoaded(v)) {
                cycles_ok = false;
                if (v) RMV_UnloadVideo(v);
                break;
            }
            RMV_UnloadVideo(v);
        }
        
        if (cycles_ok) {
            printf("  ✓ PASS: 10 cycles completed\n");
            printf("    (Run with ASan/Valgrind to check leaks)\n\n");
            tests_passed++;
        } else {
            printf("  ✗ FAIL: Cycle failed\n\n");
        }
    } else {
        printf("  ⊘ SKIPPED: No video file\n\n");
        tests_skipped++;
    }
    
    // Cleanup
    CloseWindow();
    
    // ========================================================================
    // Résultats
    // ========================================================================
    printf("════════════════════════════════════════════════════════════\n");
    printf("Résultats: %d/%d tests passed", tests_passed, tests_total - tests_skipped);
    if (tests_skipped > 0) {
        printf(" (%d skipped)", tests_skipped);
    }
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    if (tests_passed == (tests_total - tests_skipped)) {
        printf("\n✓ ÉTAPE 1.2 VALIDÉE - Load/Unload OK!\n\n");
        
        if (tests_skipped > 0) {
            printf("⚠ Note: %d tests skipped (no video file)\n", tests_skipped);
            printf("  Edit TEST_VIDEO_PATH in source code to:\n");
            printf("    #define TEST_VIDEO_PATH \"/path/to/your/video.mp4\"\n\n");
        }
        
        printf("Memory tests:\n");
        printf("  make EXAMPLE=02_video_loading test-memory\n\n");
        
        printf("Next: Étape 1.3 - Métadonnées vidéo\n");
        printf("  make EXAMPLE=03_video_metadata run\n\n");
        
        return 0;
    } else {
        printf("\n✗ FAIL - %d test(s) failed\n\n", (tests_total - tests_skipped) - tests_passed);
        return 1;
    }
}
