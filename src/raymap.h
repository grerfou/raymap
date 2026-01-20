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
*   MATHEMATICAL REFERENCES:
*       - Homography computation based on Direct Linear Transform (DLT)
*         Reference: OpenCV cv::findHomography() implementation
*         https://github.com/opencv/opencv/blob/master/modules/calib3d/src/fundam.cpp
*
*       - Matrix operations inspired by:
*         * mathc by Ferreyd: https://github.com/felselva/mathc
*         * cglm - OpenGL Mathematics (glm) for C: https://github.com/recp/cglm
*
*       - Theoretical foundation:
*         "Multiple View Geometry in Computer Vision"
*         by Richard Hartley and Andrew Zisserman
*         Chapter 4: Estimation - 2D Homographies
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

//----------------------------------------------------------------
// Calibration
//----------------------------------------------------------------

// Create Calibration session
RMAPI RM_Calibration *RM_CreateCalibration(RM_Surface *surface);

// Destroy colibration session
RMAPI void RM_DestroyCalibration(RM_Calibration *calibration);

// Begin calibration
RMAPI void RM_UpdateCalibration(RM_Calibration *calibration);

// End Calibration 
RMAPI void RM_EndCalibration(RM_Calibration *calibration);

// Draw complete  Calibration
RMAPI void RM_DrawCalibration(const RM_Calibration *calibration);

// Draw only corner
RMAPI void RM_DrawCalibrationCorners(const RM_Calibration *calibration);

// Draw only quad border
RMAPI void RM_DrawCalibrationBorder(const RM_Calibration *calibration);

// Draw only grid
RMAPI void RM_DrawCalibrationGrid(const RM_Calibration *calibration);

// Calibration Config
RMAPI RM_CalibrationConfig *RM_GetCalibrationConfig(RM_Calibration *calibration);

// Reset quad
RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight);

// Active corner 
RMAPI int RM_GetActiveCorner(const RM_Calibration *calibration);

// Drag corner
RMAPI bool RM_IsCalibrate(const RM_Calibration *calibration);

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

struct RM_Calibration {
    RM_Surface *surface;
    RM_CalibrationConfig config;
    int activeCorner;
    Vector2 dragOffset;
};


//---------------------------------------------------------------
// Implementation : Internal Functions
//---------------------------------------------------------------

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

RMAPI RM_Calibration *RM_CreateCalibration(RM_Surface *surface){
    if (!surface) return NULL;

    RM_Calibration *calib = (RM_Calibration *)RMMALLOC(sizeof(RM_Calibration));
    if (!calib) return NULL;

    calib->surface = surface;
    calib->config = rm_GetDefaultCalibrationConfig();
    calib->activeCorner = -1;
    calib->dragOffset = (Vector2){ 0, 0 };

    return calib;
}

RMAPI void RM_DestroyCalibration(RM_Calibration *calibration){
    if (!calibration) return;

    RMFREE(calibration);
}

RMAPI void RM_BeginCalibration(RM_Calibration *calibration){
    if (!calibration) return;

    // Placeholder - 
    // MODE special ? Desactiver normal render ? etc ..
}

RMAPI void RM_EndCalibration(RM_Calibration *calibration){
    if (!calibration) return;

    // Placeholder
}

RMAPI void RM_UpdateCalibration(RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;

    RM_Quad quad = RM_GetQuad(calibration->surface);
    Vector2 mousePos = GetMousePosition();
    float cornerRadius = calibration->config.cornerRadius;
    
    // Clic detection
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        // check coin
        Vector2 corners[4] = {
            quad.topLeft,
            quad.topRight,
            quad.bottomRight,
            quad.bottomLeft,
        };

        calibration->activeCorner = -1;

        for (int i = 0; i < 4; i++){
            float dist = Vector2Distance(mousePos, corners[i]);

            if (dist <= cornerRadius *1.5f) {
                calibration->activeCorner = i;

                // Calculer offset -> drag fluide
                calibration->dragOffset = Vector2Subtract(corners[i], mousePos);
                break;
            }
        }
    }


    // Drag
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        // Calcul posision avec offset
        Vector2 newPos = Vector2Add(mousePos, calibration->dragOffset);
        // Update coin 
        switch (calibration->activeCorner){
            case 0: quad.topLeft = newPos; break;
            case 1: quad.topRight = newPos; break;
            case 2: quad.bottomRight = newPos; break;
            case 3: quad.bottomLeft = newPos; break;
        }

        // apply new quad
        RM_SetQuad(calibration->surface, quad);
    }

    // End drag
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        calibration->activeCorner = -1; // deselectionner
    }
}

RMAPI void RM_DrawCalibration(const RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;

    RM_DrawCalibrationBorder(calibration);
    RM_DrawCalibrationGrid(calibration); 
    RM_DrawCalibrationCorners(calibration);
}

RMAPI void RM_DrawCalibrationCorners(const RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;
    if (!calibration->config.showCorners) return;

    RM_Quad quad = RM_GetQuad(calibration->surface);
    RM_CalibrationConfig cfg = calibration->config;

    Vector2 corners[4] = {
        quad.topLeft,
        quad.topRight,
        quad.bottomRight,
        quad.bottomLeft
    };

    for (int i = 0; i < 4; i++){

        Color cornerColor = (i == calibration->activeCorner)
            ? cfg.selectedCornerColor
            : cfg.cornerColor;

        // cercle
        DrawCircleV(corners[i], cfg.cornerRadius, cornerColor);
        
        // Bordure 
        DrawCircleLines((int)corners[i].x, (int)corners[i].y, cfg.cornerRadius, WHITE);
            
        // num Coin
        DrawText(TextFormat("%d", i),
                (int)corners[i].x - 5,
                (int)corners[i].y -10,
                20, BLACK);
   }
}

RMAPI void RM_DrawCalibrationGrid(const RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;
    if (!calibration->config.showGrid) return;

    RM_Quad quad = RM_GetQuad(calibration->surface);
    RM_CalibrationConfig cfg = calibration->config;

    // Horizontal
    for (int x = 0; x < cfg.gridResolutionX; x++){
        float u = (float)x / (float)cfg.gridResolutionX;

        // Vector top
        Vector2 top = Vector2Lerp(quad.topLeft, quad.topRight, u);

        // Vector bottom
        Vector2 bottom = Vector2Lerp(quad.bottomLeft, quad.bottomRight, u);

        DrawLineV(top, bottom, cfg.gridColor);
    }

    // Vertical
    for (int y = 0; y < cfg.gridResolutionY; y++){
        float v = (float)y/ (float)cfg.gridResolutionY;

        // Vector Right
        Vector2 right = Vector2Lerp(quad.topRight, quad.bottomRight, v);
        // Vector Left
        Vector2 left = Vector2Lerp(quad.topLeft, quad.bottomLeft, v);

        DrawLineV(left, right, cfg.gridColor);
    }
}

RMAPI void RM_DrawCalibrationBorder(const RM_Calibration *calibration){
    if (!calibration || !calibration->surface) return;
    if (!calibration->config.showBorder) return;

    RM_Quad quad = RM_GetQuad(calibration->surface);
    Color borderColor = calibration->config.borderColor;

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

RMAPI int RM_GetActiveCorner(const RM_Calibration *calibration){
    if (!calibration) return -1;

    return calibration->activeCorner;
} 

RMAPI bool RM_IsCalibrate(const RM_Calibration *calibration){
    if (!calibration) return false;

    return (calibration->activeCorner >= 0 && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
}


RMAPI RM_CalibrationConfig *RM_GetCalibrationConfig(RM_Calibration *calibration){
    if (!calibration) return NULL;

    return &calibration->config;
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

