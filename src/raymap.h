/**********************************************************************************************
*
*   RayMap v1.1.0 - Projection mapping and warping library for Raylib
*
*   DESCRIPTION:
*       RayMap provides tools for projection mapping, surface warping, and interactive
*       calibration. Perfect for video mapping installations, multi-projector setups,
*       and creative projection on non-flat surfaces.
*
*   FEATURES:
*
*   DEPENDENCIES:
*       - raylib 5.0+ (https://www.raylib.com)
*
*   CONFIGURATION:
*       #define RAYMAP_IMPLEMENTATION
*           Generates the implementation of the library into the included file.
*           Should be defined in only ONE .c file to avoid duplication.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2025 grerfou
*
**********************************************************************************************/
#ifndef RAYMAP_H
#define RAYMAP_H


//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>

//--------------------------------------------------------------------------------------------
// Define and Macros
//--------------------------------------------------------------------------------------------
#ifndef RMAPI
    #define RMAPI static inline// Vide pour implémentation
#endif


//--------------------------------------------------------------------------------------------
// Types and structures Definitions
//--------------------------------------------------------------------------------------------

// Quad structure (define 4 point quadrilateral)
typedef struct {
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomLeft;
    Vector2 bottomRight;
} RM_Quad;

// Map mode enumeration
typedef enum {
    RM_MAP_MESH = 0,
    RM_MAP_PERSPECTIVE
} RM_MapMode;

// Surface structure (opaque)
typedef struct RM_Surface RM_Surface;

// Calibration structure (opaque)
typedef struct RM_Calibration RM_Calibration;

// Calibration config
typedef struct {
    bool showCorners;
    bool showGrid;
    bool showBorder;
    Color cornerColor;
    Color selectedCornerColor;
    Color gridColor;
    Color borderColor;
    float cornerRadius;
    int gridResolutionX;
    int gridResolutionY;
} RM_CalibrationConfig;

//--------------------------------------------------------------------------------------------
// Surface Management Function
//--------------------------------------------------------------------------------------------

// Creation surface cartographié
RMAPI RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode);

// Distroy map surface
RMAPI void RM_DestroySurface(RM_Surface *surface);

#endif //RAYMAP_H


/***********************************************************************************
*
*   RAYMAP IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYMAP_IMPLEMENTATION)

#undef RMAPI
#define RMAPI



//#warning "RAYMAP_IMPLEMENTATION est défini !"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/***********************************************************************************
*
*   Defines and Macros
*
************************************************************************************/


#ifndef RMMALLOC
    #define RMMALLOC(sz)    malloc(sz)
#endif
#ifndef RMCALLOC
    #define RMCALLOC(n, sz)     calloc(n, sz)
#endif
#ifndef RMFREE
    #define RMFREE(p)       free(p)
#endif

/***********************************************************************************
*
*   Types and Structures Definition (internal)
*
************************************************************************************/


struct RM_Surface {
    int width;
    int height;
    RM_Quad quad;
    RM_MapMode mode;
    RenderTexture2D target;
    Mesh mesh;
    int meshColumns;
    int meshRows;
    bool meshNeedsUpdate;
};

struct RM_Calibration {
    RM_Surface *surface;
    RM_CalibrationConfig config;
    int activeCorner;
    Vector2 dragOffset;
};


/***********************************************************************************
*
*   Surface Management
*
************************************************************************************/

RMAPI RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode){
    #warning "RM_CreateSurface est compilé!"
    // Alloue memoire pour la structure 
    RM_Surface *surface = (RM_Surface *)RMMALLOC(sizeof(RM_Surface));
    if (!surface) return NULL; // gestion erreur

    surface->width = width;
    surface->height = height;
    surface->mode = mode;

    // Quad default (rectangle)
    surface->quad = (RM_Quad) {
        {0, 0},                         //topLeft
        {(float)width, 0},              //topRight
        {0, (float)height},             //bottomLeft
        {(float)width, (float)height}   //bottomRight
    };

    // création RenderTexture
    surface->target = LoadRenderTexture(width, height);

    // Init mesh (vide pour l'instant)
    surface->mesh = (Mesh){0};
    surface->meshColumns = 16; // resolution default
    surface->meshRows = 16;
    surface->meshNeedsUpdate = true; 

    return surface;

}

RMAPI void RM_DestroySurface(RM_Surface *surface){
    // contre NULL
    if (!surface) return;

    // Liberer RenderTexture
    UnloadRenderTexture(surface->target);

    // Liberer Mesh
    if (surface->mesh.vertices){
        UnloadMesh(surface->mesh);
    }

    RMFREE(surface);
}


#endif //RAYMAP_IMPLEMENTATION





