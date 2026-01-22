/**********************************************************************************************
*
*   RayMap v1.1.0 - Projection mapping and warping library for Raylib
*
*   DESCRIPTION:
*       RayMap provides tools for projection mapping, surface warping, and interactive
*       calibration. Perfect for video mapping installations, multi-projector setups,
*       and creative projection on non-flat surfaces.
*
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
    RM_MAP_BILINEAR = 0,
    RM_MAP_HOMOGRAPHY
} RM_MapMode;

// Surface structure (opaque)
typedef struct RM_Surface RM_Surface;


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


typedef struct RM_Calibration {
    RM_Surface *surface;           // Pointer to the surface being calibrated
    RM_CalibrationConfig config;   // Visual configuration (colors, grid, etc.)
    int activeCorner;              // Currently selected corner (-1 if none)
    Vector2 dragOffset;            // Offset for smooth dragging
    bool enabled;                  // Calibration mode enabled/disabled
} RM_Calibration;


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

//----------------------------------------------------------------
// Calibration
//----------------------------------------------------------------


// Init Calibration with default setting
// return calib struct ready to use (no malloc)
RMAPI RM_Calibration RM_CalibrationDefault(RM_Surface *surface);

// Update calib input and corner drag
// coll every frame when active
RMAPI void RM_UpdateCalibration(RM_Calibration *calibration);

// Update calib with auto toggle key
// toggle check + updatein one call 
// Example: RM_UpdateCalibrationInput(&calib, KEY_TAB);
// Pass 0 for togglekey to disable toggleing
RMAPI void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey);

//Toggle calibration on/off
RMAPI void RM_ToggleCalibration(RM_Calibration *calibration);

// Draw complete calib overlay
// nothing if calib disable
RMAPI void RM_DrawCalibration(RM_Calibration calibration);

// Draw only the corner handles
RMAPI void RM_DrawCalibrationCorners(RM_Calibration calibration);

// Draw only the quad border
RMAPI void RM_DrawCalibrationBorder(RM_Calibration calibration);

// Draw only the deformation grid
RMAPI void RM_DrawCalibrationGrid(RM_Calibration calibration);

// Reset quad to center 
RMAPI void RM_ResetCalibrationQuad(RM_Calibration *calibration, int screenWidth, int screenHeight);

// Reset quad
RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight);

// active corner index
RMAPI int RM_GetActiveCorner(RM_Calibration calibration);

// Check currently draging corner
RMAPI bool RM_IsCalibrating(RM_Calibration calibration);

//----------------------------------------------------------------
// IO
//----------------------------------------------------------------

// Save surface config to file
RMAPI bool RM_SaveConfig(const RM_Surface *surface, const char *filepath);

RMAPI bool RM_LoadConfig(RM_Surface *surface, const char *filepath);

//----------------------------------------------------------------
// Geometry utilities
//----------------------------------------------------------------

// chech if point inside a quad
RMAPI bool RM_PointInQuad(Vector2 point, RM_Quad quad);

// Axi aligned boundingbox quad
RMAPI Rectangle RM_GetQuadBounds(RM_Quad quad);

// Center point in quad 
RMAPI Vector2 RM_GetQuadCenter(RM_Quad quad);

// Get Area of a quad
RMAPI float RM_GetQuadArea(RM_Quad quad);


//----------------------------------------------------------------
// Point Mapping
//----------------------------------------------------------------

//  Map a point from texture space [0,1] to screen space
RMAPI Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint);

// Map a point from screen space to texture space [0,1]
RMAPI Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint);



//----------------------------------------------------------------
// Advanced/Debug
//----------------------------------------------------------------

#ifdef RAYMAP_DEBUG

// Get internal mesh for analysis/debug
// WARNING : Do not modify or free the returnes mesh
RMAPI Mesh *RM_GetSurfaceMesh(RM_Surface *surface);

#endif // RAYMAP_DEBUG

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

// Matrice 3x3 homographie 
typedef struct {
    float m[3][3];
} Matrix3x3;

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
    Matrix3x3 homography;
    bool homographyNeedsUpdate;
};


//---------------------------------------------------------------
// Implementation : Internal Functions
//---------------------------------------------------------------



static inline float rm_Cross2D(Vector2 a, Vector2 b){
    return a.x * b.y - a.y * b.x;
}

static inline bool rm_SameSide(Vector2 p, Vector2 a, Vector2 b, Vector2 ref){
    Vector2 ab = {b.x - a.x, b.y - a.y};
    Vector2 ap = {p.x - a.x, p.y - a.y};
    Vector2 ar = {ref.x - a.x, ref.y - a.y};

    float cross1 = rm_Cross2D(ab, ap);
    float cross2 = rm_Cross2D(ab, ar);

    return (cross1 * cross2) >= 0;
}

static void rm_GetDefaultResolutionForMode(RM_MapMode mode, int *cols, int *rows){
    switch (mode) {
        case RM_MAP_BILINEAR :
            // Bilinear interpolation : medium res
            *cols = 16;
            *rows = 16;
            break;
        case RM_MAP_HOMOGRAPHY :
            *cols = 32;
            *rows = 32;
            break;

        default :
            *cols = 16;
            *rows =16;
            break;
    }
}


// 3x3 matrix opération
// Ref : mathc (ferreyd), cglm, standard linear algebra

// identity matrix 
static Matrix3x3 rm_Matrix3x3Identity(void){
    Matrix3x3 result = {0};        
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    return result;
}

__attribute__((unused))
static Matrix3x3 rm_Matrix3x3Multiply(Matrix3x3 a, Matrix3x3 b){
    Matrix3x3 result = {0};

    for (int i = 0; i < 3; i ++){
        for (int j = 0; j < 3; j++){
            result.m[i][j] = 0;
            for (int k = 0; k < 3; k++){
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

    return result;
}


// Calcul determinant 3x3 matrix
// Ref : Sarrus rule for 3X3 determinants
static float rm_Matrix3x3Determinant(Matrix3x3 m){
    return m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])
        - m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])
        + m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
}

/*

[a b c]
[d e f]
[g h i]

a = m.m[0][0]
b = m.m[0][1]
c = m.m[0][2]

d = m.m[1][0]
e = m.m[1][1]
f = m.m[1][2]

g = m.m[2][0]
h = m.m[2][1]
i = m.m[2][2]

*/


// Calcul inverse 3x3 matrix using adjugate method
// Ref : Standard Linear algebra, similar to mathc mat3_inverse()
__attribute__((unused))
static Matrix3x3 rm_Matrix3x3Inverse(Matrix3x3 m){
    Matrix3x3 result = {0};
    // 0 0 0
    // 0 0 0
    // 0 0 0
    
    // if inverse existe
    float det = rm_Matrix3x3Determinant(m);

    if (fabsf(det) < 0.000001f) {
        printf("Matrix inverse, return identity");
        return rm_Matrix3x3Identity();
    }

    float invDet = 1.0f / det;

    // Calulcul cofacteur et transposer
    result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * invDet;
    result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) * invDet;
    result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * invDet;

    result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) * invDet;
    result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * invDet;
    result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;


    result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) * invDet;
    result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
    result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;

    return result;
}


static int rm_GaussSolve8x8(float A[8][8], float b[8], float x[8]){
    int i, j, k;
    float factor, temp, max_val;
    int max_row;

    // Copie a et b pour ne pas modifier les originaux
    float A_copy[8][8];
    float b_copy[8];

    for (i = 0; i < 8; i++){
        for (j = 0; j < 8; j++){
            A_copy[i][j] = A[i][j];
        }
        b_copy[i] = b[i];
    }

    // Triangulation
    for (k = 0; k < 7; k++){

        // Trouver le plus grand element dans k
        max_val = fabsf(A_copy[k][k]);
        max_row = k;


        for (i = k + 1; i < 8; i++){
            if (fabsf(A_copy[i][k]) > max_val){
                max_val = fabsf(A_copy[i][k]);
                max_row = i;
            }
        }

        // verifie singularité
        if (max_val < 1e-10f){
            return -1; // Matrix ginguliere
        }

        // Changer ligne si necessessaire 
        if (max_row != k){
            for (j =0; j < 8; j++){
                temp = A_copy[k][j];
                A_copy[k][j] = A_copy[max_row][j];
                A_copy[max_row][j] = temp;
            }
            temp = b_copy[k];
            b_copy[k] = b_copy[max_row];
            b_copy[max_row] = temp;
        }

        // Elimination -> Remmetre a zero elements sous pivot
        for (i = k + 1; i < 8; i++){
            factor = A_copy[i][k] / A_copy[k][k];

            for (j = k; j < 8; j++){
                A_copy[i][j] -= factor * A_copy[k][j];
            }
            b_copy[i] -= factor * b_copy[k];
        }

    }

    // Verifier le dernier pivot 
    if (fabsf(A_copy[7][7]) < 1e-10f){
        return -1;
    }


    // Substitution
    for (i = 7; i >= 0; i--){
        x[i] = b_copy[i];

        for (j = i + 1; j < 8; j++){
            x[i] -= A_copy[i][j] * x[j];
        }

        x[i] /= A_copy[i][i];
    }

    return 0;
}



// Calcul Matrice a partir de 4 points
// Utilise algo transformation linéaire direct (dlt)
static Matrix3x3 rm_ComputeHomography(RM_Quad dest){
    // Point de bases
    Vector2 src[4] = {
        {0, 0}, // top left
        {1, 0}, // top right
        {0, 1}, // bottom left
        {1, 1}, // bottom right
    };

    // Destination point
    Vector2 dst[4] = {
        dest.topLeft,
        dest.topRight,
        dest.bottomLeft,
        dest.bottomRight
    };

    // systeme d'équation
    // Pour chaque correspondance on obtien 2 equations

    float A[8][9];

    for (int i = 0; i < 4; i++){
        float x = src[i].x;
        float y = src[i].y;
        float u = dst[i].x;
        float v = dst[i].y;

        // Premiere equation 
        A[i*2][0] = x;
        A[i*2][1] = y;
        A[i*2][2] = 1;
        A[i*2][3] = 0;
        A[i*2][4] = 0;
        A[i*2][5] = 0;
        A[i*2][6] = -u * x;
        A[i*2][7] = -u * y;
        A[i*2][8] = -u;

        // Seconde equation
        A[i*2+1][0] = 0;
        A[i*2+1][1] = 0;
        A[i*2+1][2] = 0;
        A[i*2+1][3] = x;
        A[i*2+1][4] = y;
        A[i*2+1][5] = 1;
        A[i*2+1][6] = -v * x;
        A[i*2+1][7] = -v * y;
        A[i*2+1][8] = -v;

    }

    // Extraction 8x8
    float A_square[8][8];
    float b[8];

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            A_square[i][j] = A[i][j];
        }
        b[i] = -A[i][8]; // bouger la derniere colonne
    }

    float h[9];

    if (rm_GaussSolve8x8(A_square, b, h) != 0 ){
        // echec resolution -> renvoy la matrice 
         printf(" Homography computation failed, returning identity\n");
         return rm_Matrix3x3Identity();
    }

    h[8] = 1.0f; // normalisation
    
    // Construire 3x3 homography
    Matrix3x3 H;
    H.m[0][0] = h[0]; H.m[0][1] = h[1]; H.m[0][2] = h[2];
    H.m[1][0] = h[3]; H.m[1][1] = h[4]; H.m[1][2] = h[5];
    H.m[2][0] = h[6]; H.m[2][1] = h[7]; H.m[2][2] = h[8];

    return H;
}

static Vector2 rm_ApplyHomography(Matrix3x3 H, float u, float v){
    // Application de l'homographie 
    float x = H.m[0][0] * u + H.m[0][1] * v + H.m[0][2];
    float y = H.m[1][0] * u + H.m[1][1] * v + H.m[1][2];
    float w = H.m[2][0] * u + H.m[2][1] * v + H.m[2][2];

    if (fabsf(w) > 0.000001f){
        x /= w;
        y /= w;
    }

    return (Vector2){x, y};
}



// Bilinear 

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


    if (surface->mode == RM_MAP_HOMOGRAPHY && surface->homographyNeedsUpdate){
        surface->homography = rm_ComputeHomography(surface->quad);
        surface->homographyNeedsUpdate = false;
        printf("Homographie calculée (mode PERSPECTIVE)\n");
    }




    // Vertice
    int vIdx = 0;
    for (int y= 0; y <= rows; y++){
        for (int x = 0; x <= cols; x++){
            float u = (float)x / (float)cols;
            float v = (float)y / (float)rows;

            Vector2 pos;

            if (surface->mode == RM_MAP_HOMOGRAPHY){
                // persepctive mode utilise homographie
                pos = rm_ApplyHomography(surface->homography, u, v);
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
    if (surface->mode == mode) return;
    
    surface->mode = mode;
    
    rm_GetDefaultResolutionForMode(mode, &surface->meshColumns, &surface->meshRows);
    
    surface->meshNeedsUpdate = true;
    surface->homographyNeedsUpdate = true;
}

RMAPI RM_MapMode RM_GetMapMode(const RM_Surface *surface){
    if (!surface) return RM_MAP_BILINEAR; //default
        
    return surface->mode;
}

static RM_CalibrationConfig rm_GetDefaultCalibrationConfig(void){
    RM_CalibrationConfig config;

    config.showCorners = true;
    config.showGrid = true;
    config.showBorder = true;
    
    config.cornerColor = YELLOW;
    config.selectedCornerColor = GREEN;
    config.gridColor = ColorAlpha(WHITE, 0.3f);
    config.borderColor = RED;

    config.cornerRadius = 15.0f;
    config.gridResolutionX = 8;
    config.gridResolutionY = 8;

    return config;
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
    // Résolution selon le mode
    rm_GetDefaultResolutionForMode(mode, &surface->meshColumns, &surface->meshRows);
    surface->meshNeedsUpdate = true; 
    surface->homography = rm_Matrix3x3Identity();
    surface->homographyNeedsUpdate = true;
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
    surface->homographyNeedsUpdate = true;
    
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

//-------------------------------------------------------------
// Calibration 
//-------------------------------------------------------------

/*

RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight){
    if (!surface) return;

    int surfaceWidth = surface->width;
    int surfaceHeight = surface->height;

    int x = (screenWidth - surfaceWidth) / 2;
    int y = (screenHeight - surfaceHeight) / 2;

    RM_Quad CenterQuad = {
        { (float)x, (float)y },
        { (float)(x + surfaceWidth), (float)y },
        { (float)x, (float)(y + surfaceHeight) },
        { (float)(x + surfaceWidth), (float)(y + surfaceHeight) }
    };

    RM_SetQuad(surface, CenterQuad);
}

*/

//-------------------------------------------------------------
// Calibration Implementation
//-------------------------------------------------------------

RMAPI RM_Calibration RM_CalibrationDefault(RM_Surface *surface){
    RM_Calibration calib = {0};
    
    calib.surface = surface;
    calib.config = rm_GetDefaultCalibrationConfig();
    calib.activeCorner = -1;
    calib.dragOffset = (Vector2){0, 0};
    calib.enabled = true;  // Enabled by default
    
    return calib;
}

RMAPI void RM_ToggleCalibration(RM_Calibration *calibration){
    if (!calibration) return;
    
    calibration->enabled = !calibration->enabled;
    
    // Clean up when disabling during a drag
    if (!calibration->enabled) {
        calibration->activeCorner = -1;
    }
}

RMAPI void RM_UpdateCalibration(RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;
    if (!calibration->enabled) return;  // Skip if disabled

    RM_Quad quad = RM_GetQuad(calibration->surface);
    Vector2 mousePos = GetMousePosition();
    float cornerRadius = calibration->config.cornerRadius;
    
    // Click detection on corners
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Vector2 corners[4] = {
            quad.topLeft,
            quad.topRight,
            quad.bottomRight,
            quad.bottomLeft,
        };

        calibration->activeCorner = -1;

        for (int i = 0; i < 4; i++){
            float dist = Vector2Distance(mousePos, corners[i]);

            if (dist <= cornerRadius * 1.5f) {
                calibration->activeCorner = i;
                calibration->dragOffset = Vector2Subtract(corners[i], mousePos);
                break;
            }
        }
    }

    // Drag corner
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && calibration->activeCorner >= 0){
        Vector2 newPos = Vector2Add(mousePos, calibration->dragOffset);
        
        switch (calibration->activeCorner){
            case 0: quad.topLeft = newPos; break;
            case 1: quad.topRight = newPos; break;
            case 2: quad.bottomRight = newPos; break;
            case 3: quad.bottomLeft = newPos; break;
        }

        RM_SetQuad(calibration->surface, quad);
    }

    // End drag
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        calibration->activeCorner = -1;
    }
}

RMAPI void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey){
    if (!calibration) return;
    
    // Check toggle key (if provided)
    if (toggleKey != 0 && IsKeyPressed(toggleKey)) {
        RM_ToggleCalibration(calibration);
    }
    
    // Update calibration (does nothing if disabled)
    RM_UpdateCalibration(calibration);
}

RMAPI void RM_DrawCalibration(RM_Calibration calibration){
    if (!calibration.surface) return;
    if (!calibration.enabled) return;  // Skip if disabled

    RM_DrawCalibrationBorder(calibration);
    RM_DrawCalibrationGrid(calibration); 
    RM_DrawCalibrationCorners(calibration);
}

RMAPI void RM_DrawCalibrationCorners(RM_Calibration calibration){
    if (!calibration.surface) return;
    if (!calibration.enabled) return;
    if (!calibration.config.showCorners) return;

    RM_Quad quad = RM_GetQuad(calibration.surface);
    RM_CalibrationConfig cfg = calibration.config;

    Vector2 corners[4] = {
        quad.topLeft,
        quad.topRight,
        quad.bottomRight,
        quad.bottomLeft
    };

    for (int i = 0; i < 4; i++){
        Color cornerColor = (i == calibration.activeCorner)
            ? cfg.selectedCornerColor
            : cfg.cornerColor;

        // Draw circle
        DrawCircleV(corners[i], cfg.cornerRadius, cornerColor);
        
        // Draw border
        DrawCircleLines((int)corners[i].x, (int)corners[i].y, cfg.cornerRadius, WHITE);
            
        // Draw corner number
        DrawText(TextFormat("%d", i),
                (int)corners[i].x - 5,
                (int)corners[i].y - 10,
                20, BLACK);
   }
}

RMAPI void RM_DrawCalibrationGrid(RM_Calibration calibration){
    if (!calibration.surface) return;
    if (!calibration.enabled) return;
    if (!calibration.config.showGrid) return;

    RM_Quad quad = RM_GetQuad(calibration.surface);
    RM_CalibrationConfig cfg = calibration.config;

    // Horizontal lines
    for (int x = 0; x <= cfg.gridResolutionX; x++){
        float u = (float)x / (float)cfg.gridResolutionX;

        Vector2 top = Vector2Lerp(quad.topLeft, quad.topRight, u);
        Vector2 bottom = Vector2Lerp(quad.bottomLeft, quad.bottomRight, u);

        DrawLineV(top, bottom, cfg.gridColor);
    }

    // Vertical lines
    for (int y = 0; y <= cfg.gridResolutionY; y++){
        float v = (float)y / (float)cfg.gridResolutionY;

        Vector2 right = Vector2Lerp(quad.topRight, quad.bottomRight, v);
        Vector2 left = Vector2Lerp(quad.topLeft, quad.bottomLeft, v);

        DrawLineV(left, right, cfg.gridColor);
    }
}

RMAPI void RM_DrawCalibrationBorder(RM_Calibration calibration){
    if (!calibration.surface) return;
    if (!calibration.enabled) return;
    if (!calibration.config.showBorder) return;

    RM_Quad quad = RM_GetQuad(calibration.surface);
    Color borderColor = calibration.config.borderColor;

    DrawLineEx(quad.topLeft, quad.topRight, 2.0f, borderColor);
    DrawLineEx(quad.topRight, quad.bottomRight, 2.0f, borderColor);
    DrawLineEx(quad.bottomRight, quad.bottomLeft, 2.0f, borderColor);
    DrawLineEx(quad.bottomLeft, quad.topLeft, 2.0f, borderColor);
}

RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight){
    if (!surface) return;

    int surfaceWidth = surface->width;
    int surfaceHeight = surface->height;

    int x = (screenWidth - surfaceWidth) / 2;
    int y = (screenHeight - surfaceHeight) / 2;

    RM_Quad CenterQuad = {
        { (float)x, (float)y },
        { (float)(x + surfaceWidth), (float)y },
        { (float)x, (float)(y + surfaceHeight) },
        { (float)(x + surfaceWidth), (float)(y + surfaceHeight) }
    };

    RM_SetQuad(surface, CenterQuad);
}

RMAPI void RM_ResetCalibrationQuad(RM_Calibration *calibration, int screenWidth, int screenHeight){
    if (!calibration || !calibration->surface) return;

    RM_ResetQuad(calibration->surface, screenWidth, screenHeight);
}

RMAPI int RM_GetActiveCorner(RM_Calibration calibration){
    return calibration.activeCorner;
} 

RMAPI bool RM_IsCalibrating(RM_Calibration calibration){
    return (calibration.activeCorner >= 0 && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
}




//---------------------------------------------------------------
// IO
//---------------------------------------------------------------

RMAPI bool RM_SaveConfig(const RM_Surface *surface, const char *filepath){
    if (!surface || !filepath){
        printf("ERROR: RM_SaveConfig - Invalid parameters\n");
        return false;
    }

    FILE *file = fopen(filepath, "w");
    if (!file){
        printf("Error: RM_SaveConfig _ Connot open file '%s' for writing\n", filepath);
        return false;
    }



    // header
    fprintf(file, "# RAYMAP Config File\n");
    fprintf(file, "# Format: text/plain v1.0\n");
    fprintf(file, "\n");

    // Surface dim
    fprintf(file, "[Surface]\n");
    fprintf(file, "width=%d\n", surface->width);
    fprintf(file, "height=%d\n", surface->height);
    fprintf(file, "\n");

    // Mapping mode
    fprintf(file, "[Mode]\n");
    fprintf(file, "mode=%s\n", surface->mode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY");
    fprintf(file, "\n");

    // Mesh resolution
    fprintf(file, "[Mesh]\n");
    fprintf(file, "columns=%d\n", surface->meshColumns);
    fprintf(file, "rows=%d\n", surface->meshRows);
    fprintf(file, "\n");

    // Quad Corners
    fprintf(file, "[Quad]\n");
    fprintf(file, "topLeft=%.2f, %.2f\n", surface->quad.topLeft.x, surface->quad.topLeft.y);
    fprintf(file, "topRight=%.2f, %.2f\n", surface->quad.topRight.x, surface->quad.topRight.y);
    fprintf(file, "bottomLeft=%.2f, %.2f\n", surface->quad.bottomLeft.x, surface->quad.bottomLeft.y);
    fprintf(file, "bottomRight=%.2f, %.2f\n", surface->quad.bottomRight.x, surface->quad.bottomRight.y);
    fprintf(file, "\n");

    fclose(file);

    printf("INFO: Configuration saved to '%s'\n", filepath);
    return true;
}


RMAPI bool RM_LoadConfig(RM_Surface *surface, const char *filepath) {
    if (!surface || !filepath) {
        printf("ERROR: RM_LoadConfig - Invalid parameters\n");
        return false;
    }
    
    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("ERROR: RM_LoadConfig - Cannot open file '%s' for reading\n", filepath);
        return false;
    }
    
    char line[256];
    RM_Quad quad = surface->quad;
    int meshCols = surface->meshColumns;
    int meshRows = surface->meshRows;
    RM_MapMode mode = surface->mode;
    bool quadLoaded = false;
    
    while (fgets(line, sizeof(line), file)) {
        // supprime retour ligne 
        line[strcspn(line, "\n")] = 0;
        
        // Ignore comments, empty lines, section
        if (line[0] == '#' || line[0] == '\0' || line[0] == '[') {
            continue;
        }
        
        // Trouver le séparateur '='
        char *equals = strchr(line, '=');
        if (!equals) continue;
        
        // key et value
        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        
        // Mode
        if (strcmp(key, "mode") == 0) {
            if (strcmp(value, "BILINEAR") == 0) {
                mode = RM_MAP_BILINEAR;
            } else if (strcmp(value, "HOMOGRAPHY") == 0) {
                mode = RM_MAP_HOMOGRAPHY;
            }
        }
        
        // Mesh resolution
        else if (strcmp(key, "columns") == 0) {
            meshCols = atoi(value);
        }
        else if (strcmp(key, "rows") == 0) {
            meshRows = atoi(value);
        }
        
        // Quad corners
        else if (strcmp(key, "topLeft") == 0) {
            if (sscanf(value, "%f,%f", &quad.topLeft.x, &quad.topLeft.y) == 2) {
                quadLoaded = true;
            }
        }
        else if (strcmp(key, "topRight") == 0) {
            sscanf(value, "%f,%f", &quad.topRight.x, &quad.topRight.y);
        }
        else if (strcmp(key, "bottomLeft") == 0) {
            sscanf(value, "%f,%f", &quad.bottomLeft.x, &quad.bottomLeft.y);
        }
        else if (strcmp(key, "bottomRight") == 0) {
            sscanf(value, "%f,%f", &quad.bottomRight.x, &quad.bottomRight.y);
        }
    }
    
    fclose(file);
    
    if (!quadLoaded) {
        printf("WARNING: RM_LoadConfig - No quad data found in file\n");
        return false;
    }
    
    // applique config
    surface->mode = mode;
    surface->meshColumns = meshCols;
    surface->meshRows = meshRows;
    surface->meshNeedsUpdate = true;
    surface->homographyNeedsUpdate = true;
    
    // Applique le quad
    RM_SetQuad(surface, quad);
    
    printf("INFO: Configuration loaded from '%s'\n", filepath);
    return true;
}

//----------------------------------------------------------------
// Geometry Utilities
//----------------------------------------------------------------


RMAPI bool RM_PointInQuad(Vector2 point, RM_Quad quad){
    Vector2 center = {
        (quad.topLeft.x + quad.topRight.x + quad.bottomLeft.x + quad.bottomRight.x) / 4.0f,
        (quad.topLeft.y + quad.topRight.y + quad.bottomLeft.y + quad.bottomRight.y) / 4.0f
    };


    // Edges

    // topleft, topRight 
    if (!rm_SameSide(point, quad.topLeft, quad.topRight, center)){
        return false;
    }

    // topRight, bottomRight
    if (!rm_SameSide(point, quad.topRight, quad.bottomRight, center)){
        return false;
    }
    
    // bottomRight, bottomLeft
    if (!rm_SameSide(point, quad.bottomRight, quad.bottomLeft, center)){
        return false;
    }
    
    // bottomLeft -> topLeft
    if (!rm_SameSide(point, quad.bottomLeft, quad.topLeft, center)){
        return false;
    }

    return true;
}

RMAPI Rectangle RM_GetQuadBounds(RM_Quad quad){
    // min/max X,Y
    float minX = quad.topLeft.x;
    float maxX = quad.topLeft.x;
    float minY = quad.topLeft.y;
    float maxY = quad.topLeft.y;

    // top right
    if (quad.topRight.x < minX) minX = quad.topRight.x;
    if (quad.topRight.x > maxX) maxX = quad.topRight.x;
    if (quad.topRight.y < minY) minY = quad.topRight.y;
    if (quad.topRight.y > maxY) maxY = quad.topRight.y;

    // bottom Left
    if (quad.bottomLeft.x < minX) minX = quad.bottomLeft.x;
    if (quad.bottomLeft.x > maxX) maxX = quad.bottomLeft.x;
    if (quad.bottomLeft.y < minY) minY = quad.bottomLeft.y;
    if (quad.bottomLeft.y > maxY) maxY = quad.bottomLeft.y;

    // bottom Right
     if(quad.bottomRight.x < minX) minX = quad.bottomRight.x;
     if(quad.bottomRight.x > maxX) maxX = quad.bottomRight.x;
     if(quad.bottomRight.y < minY) minY = quad.bottomRight.y;
     if(quad.bottomRight.y < maxY) maxY = quad.bottomRight.y;

     return (Rectangle){
         minX,
         minY,
         maxX - minX,
         maxY - minY
     };
}

RMAPI Vector2 RM_GetQuadCenter(RM_Quad quad) {
    return (Vector2){
        (quad.topLeft.x + quad.topRight.x + quad.bottomLeft.x + quad.bottomRight.x) / 4.0f,
        (quad.topLeft.y + quad.topRight.y + quad.bottomLeft.y + quad.bottomRight.y) / 4.0f
    };
}

RMAPI float RM_GetQuadArea(RM_Quad quad){
    // Shoelace formule (aire d'un polygone)
    
    float x1 = quad.topLeft.x,  y1 = quad.topLeft.y;
    float x2 = quad.topRight.x, y2 = quad.topRight.y;
    float x3 = quad.bottomRight.x, y3 = quad.bottomRight.y;
    float x4 = quad.bottomLeft.x,  y4 = quad.bottomLeft.y;
    
    float area = 0.5f * fabsf(
            x1 * (y2 - y4) +
            x2 * (y3 - y1) +
            x3 * (y4 - y2) +
            x4 * (y1 - y3) 
            );

    return area;
}

//----------------------------------------------------------------
// Advanced/Debug
//----------------------------------------------------------------

RMAPI Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint) {
    if (!surface) {
        return (Vector2){-1, -1};
    }
    
    float u = fmaxf(0.0f, fminf(1.0f, texturePoint.x));
    float v = fmaxf(0.0f, fminf(1.0f, texturePoint.y));
    
    if (surface->mode == RM_MAP_HOMOGRAPHY) {
        // Recalculer si nécessaire
        if (surface->homographyNeedsUpdate) {
            surface->homography = rm_ComputeHomography(surface->quad);
            surface->homographyNeedsUpdate = false;
        }
        
        return rm_ApplyHomography(surface->homography, u, v);
    } else {
        return rm_BilinearInterpolation(
            surface->quad.topLeft,
            surface->quad.topRight,
            surface->quad.bottomLeft,
            surface->quad.bottomRight,
            u, v
        );
    }
}

RMAPI Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint){
    if (!surface){
        return (Vector2){-1, -1};
    }

    // Verifie que le point est dans le quad
    if (!RM_PointInQuad(screenPoint, surface->quad)){
        return (Vector2){-1, -1};
    }

    if (surface->mode == RM_MAP_HOMOGRAPHY){
        // use inverse homography
        Matrix3x3 invH = rm_Matrix3x3Inverse(surface->homography);
        return rm_ApplyHomography(invH, screenPoint.x, screenPoint.y);
    } else {
        // Bilinear res itérative -> Newton-Raphson
        // Approximation simple: recherche par dichotomie
        float u = 0.5f, v = 0.5f;
        const int MAX_ITERATIONS = 10;
        const float TOLERANCE = 0.5f;
        
        for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
            Vector2 mapped = rm_BilinearInterpolation(
                surface->quad.topLeft,
                surface->quad.topRight,
                surface->quad.bottomLeft,
                surface->quad.bottomRight,
                u, v
            );
            
            Vector2 error = {
                screenPoint.x - mapped.x,
                screenPoint.y - mapped.y
            };
            
            float errorMag = sqrtf(error.x * error.x + error.y * error.y);
            
            if (errorMag < TOLERANCE) {
                break;
            }
            
            // Gradient descent simple
            float step = 0.1f / (iter + 1);
            
            // Test dans 4 directions
            Vector2 testU1 = rm_BilinearInterpolation(
                surface->quad.topLeft, surface->quad.topRight,
                surface->quad.bottomLeft, surface->quad.bottomRight,
                u + step, v
            );
            
            Vector2 testV1 = rm_BilinearInterpolation(
                surface->quad.topLeft, surface->quad.topRight,
                surface->quad.bottomLeft, surface->quad.bottomRight,
                u, v + step
            );
            
            // Calculer gradients
            float gradU = (Vector2Distance(testU1, screenPoint) - errorMag) / step;
            float gradV = (Vector2Distance(testV1, screenPoint) - errorMag) / step;
            
            // Update
            u -= gradU * step * 0.5f;
            v -= gradV * step * 0.5f;
            
            // Clamp
            u = fmaxf(0.0f, fminf(1.0f, u));
            v = fmaxf(0.0f, fminf(1.0f, v));
        }

        return (Vector2){u, v};
    }
}





//----------------------------------------------------------------
// Advanced/Debug
//----------------------------------------------------------------

#ifdef RAYMAP_DEBUG

RMAPI Mesh *RM_GetSurfaceMesh(RM_Surface *surface){
    if (!surface) return NULL;
    return &surface->mesh;
}

#endif // RAYMAP_DEBUG

#endif //RAYMAP_IMPLEMENTATION

