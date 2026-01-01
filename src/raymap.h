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
    #define RMAPI extern //static inline// Vide pour implémentation
#endif


//--------------------------------------------------------------------------------------------
// Types and structures Definitions (Public)
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

// Define corner for surface
RMAPI void RM_SetQuad(RM_Surface *surface, RM_Quad quad);

// Récupére coin corner
RMAPI RM_Quad RM_GetQuad(const RM_Surface *surface);

// Surface dimenssion
RMAPI void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height);

// Maillage resolution
RMAPI void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows);

// Resolution actuelle maillage
RMAPI void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows);

//----------------------------------------------------------------
// Rendering
//----------------------------------------------------------------

// Begin Draw on Surface
RMAPI void RM_BeginSurface(RM_Surface *surface);

// End Draw on Surface
RMAPI void RM_EndSurface(RM_Surface *surface);

// Display Surface
RMAPI void RM_DrawSurface(const RM_Surface *surface);

// Map mode
RMAPI void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode);

// current mode
RMAPI RM_MapMode RM_GetMapMode(const RM_Surface *surface);

#endif //RAYMAP_H


/***********************************************************************************
*
*   RAYMAP IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYMAP_IMPLEMENTATION)

#undef RMAPI
#define RMAPI

//-----------------------------------------------------------------
// Implémentation include
//-----------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/***********************************************************************************
*
*   Implémentation Defines and Macros
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
*   Implémentation Types and Structures Definition (internal/private)
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


//---------------------------------------------------------------
// Implementation : Internal Functions
//---------------------------------------------------------------

static Vector2 rm_BilinearInterpolation(Vector2 p00, Vector2 p10, Vector2 p01, Vector2 p11, float u, float v){

    // p00 = top-left, p10 = top-right
    // p01 = bottom-left, p11 = bottom-right
    // u = horizontal [0, 1], v = vertical [0, 1]

    Vector2 result; 

    // Bilinéar
    result.x = (1.0f - u) * (1.0f - v) * p00.x +
               u * (1.0f - v) * p10.x +
               (1.0f - u) * v * p01.x +
               u * v * p11.x;

    result.y = (1.0f - u) * (1.0f - v) * p00.y +
               u * (1.0f - v) * p10.y +
               (1.0f - u) * v * p01.y +
               u * v * p11.y;

    return result;
}

static void rm_GenerateBilinearMesh(RM_Surface *surface, int cols, int rows){
    if (!surface) return;
    

    // VBertice number and triangles
    int vertexCount = (cols + 1) * (rows + 1);
    int triangleCount = cols * rows * 2; //2 triangles


    // Allouer Mesh
    Mesh mesh = {0};
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;

    mesh.vertices = (float *)RMCALLOC(vertexCount * 3, sizeof(float));
    mesh.texcoords = (float *)RMCALLOC(vertexCount * 2, sizeof(float));
    mesh.normals = (float *)RMCALLOC(vertexCount * 3, sizeof(float));
    mesh.indices = (unsigned short *)RMCALLOC(triangleCount * 3, sizeof(unsigned short));

    RM_Quad q = surface->quad;

    // Vertice
    int vIdx = 0;
    for (int y= 0; y <= rows; y++){
        for (int x = 0; x <= cols; x++){
            float u = (float)x / (float)cols;
            float v = (float)y / (float)rows;

            Vector2 pos;

            if (surface->mode == RM_MAP_PERSPECTIVE){
                // TODO : Utiliser rm_ApplyHomography() ici
                pos = rm_BilinearInterpolation(
                    q.topLeft, q.topRight,
                    q.bottomLeft, q.bottomRight,
                    u, v
                    );
            } else {
                // Mesh mode : interpolation ici
                pos = rm_BilinearInterpolation(
                    q.topLeft, q.topRight,
                    q.bottomLeft, q.bottomRight,
                    u, v
                    );
            }

            // Vertex pos 
            mesh.vertices[vIdx * 3 + 0] = pos.x;
            mesh.vertices[vIdx * 3 + 1] = pos.y;
            mesh.vertices[vIdx * 3 + 2] = 0.0f;

            // coordonnées tecture
            mesh.texcoords[vIdx * 2 + 0] = u;
            mesh.texcoords[vIdx * 2 + 1] = 1.0f - v;

            // Normal
            mesh.normals[vIdx * 3 + 0] = 0.0f;
            mesh.normals[vIdx * 3 + 1] = 0.0f;
            mesh.normals[vIdx * 3 + 2] = 1.0f;

            vIdx++;
        }
    }

    // Indices 2 triangle/(quad)
    int iIdx = 0;
    for (int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            // Indices coin quad
            int topLeft = y * (cols + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * (cols + 1) + x;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            mesh.indices[iIdx++] = topLeft;
            mesh.indices[iIdx++] = topRight;
            mesh.indices[iIdx++] = bottomLeft;

            // Triangle 2
            mesh.indices[iIdx++] = topRight;
            mesh.indices[iIdx++] = bottomRight;
            mesh.indices[iIdx++] = bottomLeft;
        }
    }

    // Upload in GPU
    UploadMesh(&mesh, false);

    // Replace mesh
    if (surface->mesh.vertices){
        UnloadMesh(surface->mesh);
    }

    surface->mesh = mesh;
    surface->meshNeedsUpdate = false;

    printf(" Mesh généré et uploadé\n");

}


static void rm_UpdateMesh(RM_Surface *surface){
    if (!surface || !surface->meshNeedsUpdate) return;

    rm_GenerateBilinearMesh(surface, surface->meshColumns, surface->meshRows);
}

RMAPI void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode){
    if (!surface) return;

    // Rien si mode d'ont change
    if (surface->mode == mode) return;

    //change
    surface->mode = mode;

    #warning "Verifier Quel la res change Vraiment "

    //Adjust res
    if (mode == RM_MAP_MESH){
        // Mode Mesh: Res medium
        surface->meshColumns = 4;
        surface->meshRows = 4;
    } else if (mode == RM_MAP_PERSPECTIVE) {
        //Mode Perspective : hight res
        //L'homographie sera implémùenter en phase 4
        //Pour l'instant utilise interpolation bilinéaire
        surface->meshColumns = 32;
        surface->meshRows = 32;
    }

    surface->meshNeedsUpdate = true;
}


RMAPI RM_MapMode RM_GetMapMode(const RM_Surface *surface){
    if (!surface) return RM_MAP_MESH; //default
        
    return surface->mode;
}

//---------------------------------------------------------------
// Implementation: Puiblic API
//---------------------------------------------------------------


//---------------------------------------------------------------
// Surface manage
//----------------------------------------------------------------


RMAPI RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode){
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

    rm_UpdateMesh(surface);

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


RMAPI void RM_SetQuad(RM_Surface *surface, RM_Quad quad){
    if (!surface) return;

    surface->quad = quad;
    surface->meshNeedsUpdate = true;
    
    bool isDegenerate = (
        quad.topLeft.x == quad.topRight.x &&
        quad.topLeft.y == quad.topRight.y &&
        quad.topLeft.x == quad.bottomLeft.x &&
        quad.topLeft.y == quad.bottomLeft.y &&
        quad.topLeft.x == quad.bottomRight.x &&
        quad.topLeft.y == quad.bottomRight.y
    );

    if (isDegenerate){
        printf("Quad dégénérer");
    };
}

RMAPI RM_Quad RM_GetQuad (const RM_Surface *surface){
    if (!surface){
        return (RM_Quad){ {0, 0}, {0, 0}, {0, 0}, {0, 0} };
    };

    return surface->quad;
}


RMAPI void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height){
    if (!surface) return;

    if (width) *width = surface->width;
    if (height) *height = surface->height;
}


RMAPI void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows){
    if (!surface) return;

    // Limite min/max
    if (columns < 4) columns = 4;
    if (columns > 64) columns = 64;
    if (rows < 4) rows = 4;
    if (rows > 64) rows = 64;

    // nothing if not change
    if (surface->meshColumns == columns && surface->meshRows == rows){
        return;
    }

    // update res
    surface->meshColumns = columns;
    surface->meshRows = rows;

    // Régénération
    surface->meshNeedsUpdate = true;
}

RMAPI void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows){
    if (!surface) return;

    if (columns) *columns = surface->meshColumns;
    if (rows) *rows = surface->meshRows;
}

//-----------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------

RMAPI void RM_BeginSurface(RM_Surface *surface){
    if (!surface) return; // contre NULL
    BeginTextureMode(surface->target);
}

RMAPI void RM_EndSurface(RM_Surface *surface){
    if (!surface) return;
    EndTextureMode();
}

RMAPI void RM_DrawSurface(const RM_Surface *surface){
    if (!surface) return;


    // Update if nessesary
    if (surface->meshNeedsUpdate){
        rm_UpdateMesh((RM_Surface * )surface);
    }
    
    Texture2D tex = surface->target.texture;


    // Draw mesh with texturte
    if (surface->mesh.vertices){
        Material mat = LoadMaterialDefault();
        SetMaterialTexture(&mat, MATERIAL_MAP_DIFFUSE, tex);

        rlDisableDepthTest();
        rlDisableBackfaceCulling();
        DrawMesh(surface->mesh, mat, MatrixIdentity());
        rlEnableBackfaceCulling();
        rlEnableDepthTest();
        // retourne static material
    } else{
        printf(" PAS DE VERTICES !!!!!!!!!!!!!!!!\n");
    }
}

#endif //RAYMAP_IMPLEMENTATION

