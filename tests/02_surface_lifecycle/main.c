#include "raylib.h"
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

#include <stdio.h>

int main(void){
    InitWindow(800, 600, "Test surface lifgecycle");

    printf("\nTest1: Créeet detruire une surface\n");
    RM_Surface *s = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);

    if(s) {
        printf("\nsurface crée\n");
        RM_DestroySurface(s);
        printf("\nsurface détruite\n");
    }

    printf("\n Test2: Crée et detruire 100 surface \n");
    for(int i = 0; i < 100; i++){
        RM_Surface *s1 = RM_CreateSurface(400, 300, RM_MAP_BILINEAR);
        RM_DestroySurface(s1);
    }
    printf("\n 100 surface crée/detruite \n");


    printf("\nTest3: Vérifier RenderTexture");
    RM_Surface *s3 = RM_CreateSurface(400, 300, RM_MAP_HOMOGRAPHY);
    if(s3){
        // RenderTexture --> Opaque. Verifier que l'on peut crée la surface
        printf("Surface RenderTexture crée");
        RM_DestroySurface(s3);
    }

    printf("\n Tout les tests passé");

    CloseWindow();
    return 0;
}
