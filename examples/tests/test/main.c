// test_diagnostic.c
#include "raylib.h"
#include <stdio.h>

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

void PrintSurfaceState(const RM_Surface *surface, const char *label) {
    if (!surface) {
        printf("[%s] Surface is NULL\n", label);
        return;
    }
    
    printf("[%s] Surface state:\n", label);
    printf("  - target.id = %u\n", surface->target.id);
    printf("  - target.texture.id = %u\n", surface->target.texture.id);
    printf("  - material.shader.id = %u\n", surface->material.shader.id);
    printf("  - mesh.vertices = %p\n", (void*)surface->mesh.vertices);
    printf("  - mesh.vboId[0] = %u\n", surface->mesh.vboId[0]);
    printf("\n");
}

int main(void) {
    InitWindow(800, 600, "RayMap - Diagnostic Test");
    SetTargetFPS(60);
    
    printf("=== CREATING SURFACE ===\n");
    RM_Surface *surface = RM_CreateSurface(400, 300, RM_MAP_BILINEAR);
    PrintSurfaceState(surface, "AFTER CREATE");
    
    int frameCount = 0;
    
    while (!WindowShouldClose() && frameCount < 5) {
        frameCount++;
        printf("=== FRAME %d ===\n", frameCount);
        
        PrintSurfaceState(surface, "BEFORE BeginSurface");
        
        RM_BeginSurface(surface);
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("Frame %d", frameCount), 10, 10, 20, BLACK);
        RM_EndSurface(surface);
        
        PrintSurfaceState(surface, "AFTER EndSurface");
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        PrintSurfaceState(surface, "BEFORE DrawSurface");
        RM_DrawSurface(surface);
        PrintSurfaceState(surface, "AFTER DrawSurface");
        
        DrawText(TextFormat("Frame: %d", frameCount), 10, 10, 20, GREEN);
        DrawFPS(10, 35);
        
        EndDrawing();
        
        printf("\n");
    }
    
    printf("=== DESTROYING SURFACE ===\n");
    PrintSurfaceState(surface, "BEFORE DESTROY");
    RM_DestroySurface(surface);
    
    CloseWindow();
    return 0;
}
