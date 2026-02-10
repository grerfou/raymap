/*
 * Test 04_texture_access - Étape 1.4 : Texture accessible
 * 
 * OBJECTIF:
 *   Valider RMV_GetVideoTexture() et l'accès à la texture Raylib.
 * 
 * COMMANDE:
 *   make EXAMPLE=04_texture_access run
 * 
 * CONFIGURATION:
 *   Éditer TEST_VIDEO_PATH ci-dessous
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
// ============================================================================

int main(int argc, char **argv) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║    RayMapVid - Test 04: Texture Access                    ║\n");
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
    InitWindow(800, 600, "Test 04 - Texture Access");
    
    // ========================================================================
    // Test 1: GetVideoTexture sur NULL
    // ========================================================================
    tests_total++;
    printf("[Test 1] GetVideoTexture on NULL video\n");
    
    Texture2D null_tex = RMV_GetVideoTexture(NULL);
    
    if (null_tex.id == 0) {
        printf("  ✓ PASS: NULL video returns invalid texture\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Should return texture with id=0\n\n");
    }
    
    // ========================================================================
    // Test 2: Load vidéo et obtenir texture
    // ========================================================================
    tests_total++;
    printf("[Test 2] Load video and get texture\n");
    printf("  File: %s\n", videoPath);
    
    RMV_Video *video = RMV_LoadVideo(videoPath);
    
    if (!video || !RMV_IsVideoLoaded(video)) {
        printf("  ✗ FAIL: Could not load video\n\n");
        CloseWindow();
        return 1;
    }
    
    Texture2D tex = RMV_GetVideoTexture(video);
    RMV_VideoInfo info = RMV_GetVideoInfo(video);
    
    printf("\n");
    printf("  Texture info:\n");
    printf("    ID:      %u\n", tex.id);
    printf("    Size:    %dx%d\n", tex.width, tex.height);
    printf("    Format:  %d\n", tex.format);
    printf("    Mipmaps: %d\n", tex.mipmaps);
    printf("\n");
    
    if (tex.id > 0) {
        printf("  ✓ PASS: Valid texture obtained\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid texture ID\n\n");
    }
    
    // ========================================================================
    // Test 3: Validation ID texture
    // ========================================================================
    tests_total++;
    printf("[Test 3] Texture ID validation\n");
    
    if (tex.id > 0) {
        printf("  ✓ PASS: Texture ID > 0 (%u)\n\n", tex.id);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Invalid texture ID\n\n");
    }
    
    // ========================================================================
    // Test 4: Dimensions texture = dimensions vidéo
    // ========================================================================
    tests_total++;
    printf("[Test 4] Texture dimensions match video\n");
    
    if (tex.width == info.width && tex.height == info.height) {
        printf("  ✓ PASS: Dimensions match (%dx%d)\n\n", tex.width, tex.height);
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Dimensions mismatch\n");
        printf("    Texture: %dx%d\n", tex.width, tex.height);
        printf("    Video:   %dx%d\n\n", info.width, info.height);
    }
    
    // ========================================================================
    // Test 5: Texture utilisable avec DrawTexture
    // ========================================================================
    tests_total++;
    printf("[Test 5] Texture drawable\n");
    
    BeginDrawing();
        ClearBackground(BLACK);
        
        // Try to draw (should not crash)
        DrawTexture(tex, 10, 10, WHITE);
        
        // Draw scaled
        float scale = 0.5f;
        DrawTextureEx(tex, (Vector2){100, 100}, 0.0f, scale, WHITE);
        
    EndDrawing();
    
    printf("  ✓ PASS: DrawTexture didn't crash\n");
    printf("    (Visual: black frame expected)\n\n");
    tests_passed++;
    
    // ========================================================================
    // Test 6: Multiple GetVideoTexture calls
    // ========================================================================
    tests_total++;
    printf("[Test 6] Multiple GetVideoTexture calls\n");
    
    Texture2D tex2 = RMV_GetVideoTexture(video);
    Texture2D tex3 = RMV_GetVideoTexture(video);
    
    if (tex2.id == tex.id && tex3.id == tex.id &&
        tex2.width == tex.width && tex3.width == tex.width) {
        printf("  ✓ PASS: Same texture returned\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Different textures returned\n\n");
    }
    
    // ========================================================================
    // Test 7: Texture persiste après autres calls
    // ========================================================================
    tests_total++;
    printf("[Test 7] Texture persists after other calls\n");
    
    RMV_VideoInfo info2 = RMV_GetVideoInfo(video);
    Texture2D tex_after = RMV_GetVideoTexture(video);
    
    (void)info2; // Unused
    
    if (tex_after.id == tex.id) {
        printf("  ✓ PASS: Texture ID unchanged\n\n");
        tests_passed++;
    } else {
        printf("  ✗ FAIL: Texture ID changed\n\n");
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
        printf("\n✓ ÉTAPE 1.4 VALIDÉE - Texture access OK!\n\n");
        
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║           ✓ PHASE 1 COMPLÈTE - FOUNDATION OK!             ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\n");
        
        printf("All foundation steps validated:\n");
        printf("  ✓ 1.1 - Architecture de base\n");
        printf("  ✓ 1.2 - Chargement vidéo\n");
        printf("  ✓ 1.3 - Métadonnées vidéo\n");
        printf("  ✓ 1.4 - Texture access\n\n");
        
        printf("Memory validation:\n");
        printf("  make EXAMPLE=02_video_loading test-memory\n");
        printf("  make EXAMPLE=03_video_metadata test-memory\n");
        printf("  make EXAMPLE=04_texture_access test-memory\n\n");
        
        printf("Next: Phase 2 - Playback Basique\n");
        printf("  Étape 2.1 - État de lecture (Play/Pause/Stop)\n\n");
        
        return 0;
    } else {
        printf("\n✗ FAIL - %d test(s) failed\n\n", tests_total - tests_passed);
        return 1;
    }
}
