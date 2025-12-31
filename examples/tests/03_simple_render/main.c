#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void) {
    InitWindow(800, 600, "Test Simple Render");
    SetTargetFPS(60);
    
    printf("=== Test Simple Render ===\n");
    
    // Créer une surface
    RM_Surface *surface = RM_CreateSurface(400, 300, RM_MAP_MESH);
    
    if (!surface) {
        printf(" Échec création surface\n");
        CloseWindow();
        return 1;
    }
    
    printf(" Surface créée (400x300)\n");
    
    // Boucle principale
    while (!WindowShouldClose()) {
        
        // 1. Dessiner DANS la surface
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            
            DrawText("HELLO RAYMAP!", 50, 50, 30, WHITE);
            DrawCircle(200, 150, 40, RED);
            DrawRectangle(250, 100, 100, 50, GREEN);
            
        RM_EndSurface(surface);
        
        // 2. Dessiner à l'écran
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Afficher la surface
            RM_DrawSurface(surface);
            
            // HUD
            DrawText("Test: Simple Render", 10, 10, 20, LIME);
            DrawFPS(10, 30);
            
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    
    printf(" Test terminé\n");
    return 0;
}
