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
*       - Bilinear and homography-based surface warping
*       - Interactive corner-based calibration
*       - Real-time mesh deformation
*       - Configuration save/load
*       - Point mapping utilities
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
// Defines and Macros
//--------------------------------------------------------------------------------------------
#ifndef RMAPI
    #define RMAPI extern
#endif

//--------------------------------------------------------------------------------------------
// Types and Structures (Public API)
//--------------------------------------------------------------------------------------------

// Quad structure - defines a 4-point quadrilateral
typedef struct {
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomRight;
    Vector2 bottomLeft;
} RM_Quad;

// Map mode enumeration
typedef enum {
    RM_MAP_BILINEAR = 0,    // Simple bilinear interpolation
    RM_MAP_HOMOGRAPHY       // Perspective-correct homography
} RM_MapMode;

// Surface structure (opaque pointer pattern)
typedef struct RM_Surface RM_Surface;

// Calibration visual configuration
typedef struct {
    bool showCorners;               // Display corner handles
    bool showGrid;                  // Display deformation grid
    bool showBorder;                // Display quad border
    Color cornerColor;              // Default corner color
    Color selectedCornerColor;      // Active corner color
    Color gridColor;                // Grid line color
    Color borderColor;              // Border color
    float cornerRadius;             // Corner handle radius
    int gridResolutionX;            // Horizontal grid subdivisions
    int gridResolutionY;            // Vertical grid subdivisions
} RM_CalibrationConfig;

// Calibration state
typedef struct {
    RM_Surface *surface;            // Target surface
    RM_CalibrationConfig config;    // Visual settings
    int activeCorner;               // Currently selected corner (-1 if none)
    Vector2 dragOffset;             // Mouse drag offset for smooth interaction
    bool enabled;                   // Calibration mode active/inactive
} RM_Calibration;

//--------------------------------------------------------------------------------------------
// Surface Management
//--------------------------------------------------------------------------------------------

// Create a new mappable surface
RMAPI RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode);

// Destroy surface and free resources
RMAPI void RM_DestroySurface(RM_Surface *surface);

// Set quad corner positions (returns false if invalid)
RMAPI bool RM_SetQuad(RM_Surface *surface, RM_Quad quad);

// Get current quad corner positions
RMAPI RM_Quad RM_GetQuad(const RM_Surface *surface);

// Get surface render texture dimensions
RMAPI void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height);

// Set mesh resolution (columns x rows)
RMAPI void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows);

// Get current mesh resolution
RMAPI void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows);

//--------------------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------------------

// Begin drawing to surface render texture
RMAPI void RM_BeginSurface(RM_Surface *surface);

// End drawing to surface render texture
RMAPI void RM_EndSurface(RM_Surface *surface);

// Draw the warped surface to screen
RMAPI void RM_DrawSurface(const RM_Surface *surface);

// Set mapping mode (bilinear/homography)
RMAPI void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode);

// Get current mapping mode
RMAPI RM_MapMode RM_GetMapMode(const RM_Surface *surface);

//--------------------------------------------------------------------------------------------
// Calibration
//--------------------------------------------------------------------------------------------

// Initialize calibration with default settings
RMAPI RM_Calibration RM_CalibrationDefault(RM_Surface *surface);

// Update calibration input and corner dragging (call every frame when active)
RMAPI void RM_UpdateCalibration(RM_Calibration *calibration);

// Update calibration with auto-toggle key (pass 0 to disable toggling)
RMAPI void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey);

// Toggle calibration on/off
RMAPI void RM_ToggleCalibration(RM_Calibration *calibration);

// Draw complete calibration overlay (corners + grid + border)
RMAPI void RM_DrawCalibration(RM_Calibration calibration);

// Draw only corner handles
RMAPI void RM_DrawCalibrationCorners(RM_Calibration calibration);

// Draw only quad border
RMAPI void RM_DrawCalibrationBorder(RM_Calibration calibration);

// Draw only deformation grid
RMAPI void RM_DrawCalibrationGrid(RM_Calibration calibration);

// Reset quad to centered rectangle
RMAPI void RM_ResetCalibrationQuad(RM_Calibration *calibration, int screenWidth, int screenHeight);

// Reset surface quad to centered rectangle
RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight);

// Get active corner index (-1 if none)
RMAPI int RM_GetActiveCorner(RM_Calibration calibration);

// Check if currently dragging a corner
RMAPI bool RM_IsCalibrating(RM_Calibration calibration);

//--------------------------------------------------------------------------------------------
// Configuration I/O
//--------------------------------------------------------------------------------------------

// Save surface configuration to text file
RMAPI bool RM_SaveConfig(const RM_Surface *surface, const char *filepath);

// Load surface configuration from text file
RMAPI bool RM_LoadConfig(RM_Surface *surface, const char *filepath);

//--------------------------------------------------------------------------------------------
// Geometry Utilities
//--------------------------------------------------------------------------------------------

// Check if point is inside quad
RMAPI bool RM_PointInQuad(Vector2 point, RM_Quad quad);

// Get axis-aligned bounding box of quad
RMAPI Rectangle RM_GetQuadBounds(RM_Quad quad);

// Get center point of quad
RMAPI Vector2 RM_GetQuadCenter(RM_Quad quad);

// Calculate area of quad
RMAPI float RM_GetQuadArea(RM_Quad quad);

//--------------------------------------------------------------------------------------------
// Point Mapping
//--------------------------------------------------------------------------------------------

// Map point from texture space [0,1] to screen space
RMAPI Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint);

// Map point from screen space to texture space [0,1]
RMAPI Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint);

//--------------------------------------------------------------------------------------------
// Advanced/Debug
//--------------------------------------------------------------------------------------------

#ifdef RAYMAP_DEBUG
// Get internal mesh for debugging (do NOT modify or free)
RMAPI Mesh *RM_GetSurfaceMesh(RM_Surface *surface);
#endif

#endif // RAYMAP_H


/***********************************************************************************
*
*   RAYMAP IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYMAP_IMPLEMENTATION)

#undef RMAPI
#define RMAPI

//--------------------------------------------------------------------------------------------
// Implementation Includes
//--------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

//--------------------------------------------------------------------------------------------
// Memory Management Macros
//--------------------------------------------------------------------------------------------
#ifndef RMMALLOC
    #define RMMALLOC(sz)        malloc(sz)
#endif
#ifndef RMCALLOC
    #define RMCALLOC(n, sz)     calloc(n, sz)
#endif
#ifndef RMFREE
    #define RMFREE(p)           free(p)
#endif

//--------------------------------------------------------------------------------------------
// Internal Types and Structures
//--------------------------------------------------------------------------------------------

// 3x3 Matrix for homography transformations
typedef struct {
    float m[3][3];
} Matrix3x3;

// Surface structure (internal definition)
struct RM_Surface {
    int width;                      // Render texture width
    int height;                     // Render texture height
    RM_Quad quad;                   // Corner positions
    RM_MapMode mode;                // Mapping algorithm
    RenderTexture2D target;         // Render target
    Material material;              // Material with texture
    Mesh mesh;                      // Deformed mesh
    int meshColumns;                // Mesh horizontal resolution
    int meshRows;                   // Mesh vertical resolution
    bool meshNeedsUpdate;           // Dirty flag for mesh
    Matrix3x3 homography;           // Cached homography matrix
    bool homographyNeedsUpdate;     // Dirty flag for homography
};

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Geometry
//--------------------------------------------------------------------------------------------

// Calculate 2D cross product
static inline float rm_Cross2D(Vector2 a, Vector2 b)
{
    return a.x * b.y - a.y * b.x;
}

// Check if point p and ref are on same side of line ab
static inline bool rm_SameSide(Vector2 p, Vector2 a, Vector2 b, Vector2 ref)
{
    Vector2 ab = { b.x - a.x, b.y - a.y };
    Vector2 ap = { p.x - a.x, p.y - a.y };
    Vector2 ar = { ref.x - a.x, ref.y - a.y };

    float cross1 = rm_Cross2D(ab, ap);
    float cross2 = rm_Cross2D(ab, ar);

    return (cross1 * cross2) >= 0.0f;
}

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Configuration
//--------------------------------------------------------------------------------------------

// Get default mesh resolution based on mapping mode
static void rm_GetDefaultResolutionForMode(RM_MapMode mode, int *cols, int *rows)
{
    switch (mode) {
        case RM_MAP_BILINEAR:
            *cols = 16;
            *rows = 16;
            break;
            
        case RM_MAP_HOMOGRAPHY:
            *cols = 32;
            *rows = 32;
            break;
            
        default:
            *cols = 16;
            *rows = 16;
            break;
    }
}

// Get default calibration configuration
static RM_CalibrationConfig rm_GetDefaultCalibrationConfig(void)
{
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

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Matrix 3x3 Operations
//--------------------------------------------------------------------------------------------

// Create 3x3 identity matrix
static Matrix3x3 rm_Matrix3x3Identity(void)
{
    Matrix3x3 result = { 0 };
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    return result;
}

// Multiply two 3x3 matrices
__attribute__((unused))
static Matrix3x3 rm_Matrix3x3Multiply(Matrix3x3 a, Matrix3x3 b)
{
    Matrix3x3 result = { 0 };
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    
    return result;
}

// Calculate 3x3 matrix determinant (Sarrus rule)
static float rm_Matrix3x3Determinant(Matrix3x3 m)
{
    return m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])
         - m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])
         + m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
}

// Invert 3x3 matrix using adjugate method
__attribute__((unused))
static Matrix3x3 rm_Matrix3x3Inverse(Matrix3x3 m)
{
    Matrix3x3 result = { 0 };
    
    float det = rm_Matrix3x3Determinant(m);
    if (fabsf(det) < 1e-6f) {
        TraceLog(LOG_WARNING, "RAYMAP: Matrix singular, returning identity");
        return rm_Matrix3x3Identity();
    }
    
    float invDet = 1.0f / det;
    
    // Compute cofactors and transpose
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

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Linear Algebra
//--------------------------------------------------------------------------------------------

// Solve 8x8 linear system using Gaussian elimination with partial pivoting
static int rm_GaussSolve8x8(float A[8][8], float b[8], float x[8])
{
    float A_copy[8][8];
    float b_copy[8];
    
    // Copy input to avoid modifying originals
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            A_copy[i][j] = A[i][j];
        }
        b_copy[i] = b[i];
    }
    
    // Forward elimination with partial pivoting
    for (int k = 0; k < 7; k++) {
        // Find pivot
        float max_val = fabsf(A_copy[k][k]);
        int max_row = k;
        
        for (int i = k + 1; i < 8; i++) {
            if (fabsf(A_copy[i][k]) > max_val) {
                max_val = fabsf(A_copy[i][k]);
                max_row = i;
            }
        }
        
        // Check for singularity
        if (max_val < 1e-10f) {
            return -1;
        }
        
        // Swap rows if necessary
        if (max_row != k) {
            for (int j = 0; j < 8; j++) {
                float temp = A_copy[k][j];
                A_copy[k][j] = A_copy[max_row][j];
                A_copy[max_row][j] = temp;
            }
            float temp = b_copy[k];
            b_copy[k] = b_copy[max_row];
            b_copy[max_row] = temp;
        }
        
        // Eliminate column
        for (int i = k + 1; i < 8; i++) {
            float factor = A_copy[i][k] / A_copy[k][k];
            for (int j = k; j < 8; j++) {
                A_copy[i][j] -= factor * A_copy[k][j];
            }
            b_copy[i] -= factor * b_copy[k];
        }
    }
    
    // Check final pivot
    if (fabsf(A_copy[7][7]) < 1e-10f) {
        return -1;
    }
    
    // Back substitution
    for (int i = 7; i >= 0; i--) {
        x[i] = b_copy[i];
        for (int j = i + 1; j < 8; j++) {
            x[i] -= A_copy[i][j] * x[j];
        }
        x[i] /= A_copy[i][i];
    }
    
    return 0;
}

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Homography
//--------------------------------------------------------------------------------------------

// Compute homography matrix from quad using DLT (Direct Linear Transform)
static Matrix3x3 rm_ComputeHomography(RM_Quad dest)
{
    // Source points (normalized texture coordinates)
    Vector2 src[4] = {
        { 0.0f, 0.0f },  // Top-left
        { 1.0f, 0.0f },  // Top-right
        { 1.0f, 1.0f },  // Bottom-right
        { 0.0f, 1.0f }   // Bottom-left
    };
    
    // Destination points (screen coordinates)
    Vector2 dst[4] = {
        dest.topLeft,
        dest.topRight,
        dest.bottomRight,
        dest.bottomLeft
    };
    
    // Build 8x9 system of equations (2 equations per correspondence)
    float A[8][9];
    
    for (int i = 0; i < 4; i++) {
        float x = src[i].x;
        float y = src[i].y;
        float u = dst[i].x;
        float v = dst[i].y;
        
        // First equation
        A[i*2][0] = x;
        A[i*2][1] = y;
        A[i*2][2] = 1.0f;
        A[i*2][3] = 0.0f;
        A[i*2][4] = 0.0f;
        A[i*2][5] = 0.0f;
        A[i*2][6] = -u * x;
        A[i*2][7] = -u * y;
        A[i*2][8] = -u;
        
        // Second equation
        A[i*2+1][0] = 0.0f;
        A[i*2+1][1] = 0.0f;
        A[i*2+1][2] = 0.0f;
        A[i*2+1][3] = x;
        A[i*2+1][4] = y;
        A[i*2+1][5] = 1.0f;
        A[i*2+1][6] = -v * x;
        A[i*2+1][7] = -v * y;
        A[i*2+1][8] = -v;
    }
    
    // Extract 8x8 coefficient matrix and right-hand side
    float A_square[8][8];
    float b[8];
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            A_square[i][j] = A[i][j];
        }
        b[i] = -A[i][8];
    }
    
    // Solve for homography parameters
    float h[9];
    if (rm_GaussSolve8x8(A_square, b, h) != 0) {
        TraceLog(LOG_WARNING, "RAYMAP: Homography computation failed, returning identity");
        return rm_Matrix3x3Identity();
    }
    
    h[8] = 1.0f;  // Normalization
    
    // Build 3x3 homography matrix
    Matrix3x3 H;
    H.m[0][0] = h[0]; H.m[0][1] = h[1]; H.m[0][2] = h[2];
    H.m[1][0] = h[3]; H.m[1][1] = h[4]; H.m[1][2] = h[5];
    H.m[2][0] = h[6]; H.m[2][1] = h[7]; H.m[2][2] = h[8];
    
    return H;
}

// Apply homography transformation to point
static Vector2 rm_ApplyHomography(Matrix3x3 H, float u, float v)
{
    float x = H.m[0][0] * u + H.m[0][1] * v + H.m[0][2];
    float y = H.m[1][0] * u + H.m[1][1] * v + H.m[1][2];
    float w = H.m[2][0] * u + H.m[2][1] * v + H.m[2][2];
    
    if (fabsf(w) > 1e-6f) {
        x /= w;
        y /= w;
    }
    
    return (Vector2){ x, y };
}

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Bilinear Interpolation
//--------------------------------------------------------------------------------------------

// Perform bilinear interpolation between four corner points
static Vector2 rm_BilinearInterpolation(Vector2 p00, Vector2 p10, Vector2 p01, Vector2 p11,
                                        float u, float v)
{
    // p00 = top-left, p10 = top-right
    // p01 = bottom-left, p11 = bottom-right
    // u = horizontal [0, 1], v = vertical [0, 1]
    
    Vector2 result;
    
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

//--------------------------------------------------------------------------------------------
// Internal Helper Functions - Mesh Generation
//--------------------------------------------------------------------------------------------

// Generate deformed mesh based on current quad and mapping mode
static void rm_GenerateBilinearMesh(RM_Surface *surface, int cols, int rows)
{
    if (!surface) {
        TraceLog(LOG_ERROR, "RAYMAP: Cannot generate mesh for NULL surface");
        return;
    }
    
    // Calculate dimensions
    int vertexCount = (cols + 1) * (rows + 1);
    int triangleCount = cols * rows * 2;
    
    TraceLog(LOG_DEBUG, "RAYMAP: Generating mesh [%dx%d = %d vertices, %d triangles]",
             cols, rows, vertexCount, triangleCount);
    
    // Allocate mesh
    Mesh mesh = { 0 };
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;
    
    mesh.vertices = (float *)RMCALLOC(vertexCount * 3, sizeof(float));
    mesh.texcoords = (float *)RMCALLOC(vertexCount * 2, sizeof(float));
    mesh.normals = (float *)RMCALLOC(vertexCount * 3, sizeof(float));
    mesh.indices = (unsigned short *)RMCALLOC(triangleCount * 3, sizeof(unsigned short));
    
    // Verify allocation
    if (!mesh.vertices || !mesh.texcoords || !mesh.normals || !mesh.indices) {
        TraceLog(LOG_ERROR, "RAYMAP: Failed to allocate mesh memory");
        if (mesh.vertices) RMFREE(mesh.vertices);
        if (mesh.texcoords) RMFREE(mesh.texcoords);
        if (mesh.normals) RMFREE(mesh.normals);
        if (mesh.indices) RMFREE(mesh.indices);
        return;
    }
    
    // Compute homography if needed
    RM_Quad q = surface->quad;
    if (surface->mode == RM_MAP_HOMOGRAPHY && surface->homographyNeedsUpdate) {
        surface->homography = rm_ComputeHomography(surface->quad);
        surface->homographyNeedsUpdate = false;
        TraceLog(LOG_DEBUG, "RAYMAP: Homography computed");
    }
    
    // Generate vertices
    int vIdx = 0;
    for (int y = 0; y <= rows; y++) {
        for (int x = 0; x <= cols; x++) {
            float u = (float)x / (float)cols;
            float v = (float)y / (float)rows;
            
            Vector2 pos;
            if (surface->mode == RM_MAP_HOMOGRAPHY) {
                pos = rm_ApplyHomography(surface->homography, u, v);
            } else {
                pos = rm_BilinearInterpolation(
                    q.topLeft, q.topRight,
                    q.bottomLeft, q.bottomRight,
                    u, v
                );
            }
            
            // Vertex position
            mesh.vertices[vIdx * 3 + 0] = pos.x;
            mesh.vertices[vIdx * 3 + 1] = pos.y;
            mesh.vertices[vIdx * 3 + 2] = 0.0f;
            
            // Texture coordinates
            mesh.texcoords[vIdx * 2 + 0] = u;
            mesh.texcoords[vIdx * 2 + 1] = 1.0f - v;  // Flip V for raylib
            
            // Normals (all pointing towards +Z)
            mesh.normals[vIdx * 3 + 0] = 0.0f;
            mesh.normals[vIdx * 3 + 1] = 0.0f;
            mesh.normals[vIdx * 3 + 2] = 1.0f;
            
            vIdx++;
        }
    }
    
    // Generate indices
    int iIdx = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int topLeft = y * (cols + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * (cols + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            mesh.indices[iIdx++] = topLeft;
            mesh.indices[iIdx++] = topRight;
            mesh.indices[iIdx++] = bottomLeft;
            
            // Second triangle
            mesh.indices[iIdx++] = topRight;
            mesh.indices[iIdx++] = bottomRight;
            mesh.indices[iIdx++] = bottomLeft;
        }
    }
    
    // Upload to GPU
    UploadMesh(&mesh, false);
    
    if (mesh.vboId[0] == 0) {
        TraceLog(LOG_ERROR, "RAYMAP: Failed to upload mesh to GPU");
        RMFREE(mesh.vertices);
        RMFREE(mesh.texcoords);
        RMFREE(mesh.normals);
        RMFREE(mesh.indices);
        return;
    }
    
    // Replace existing mesh
    if (surface->mesh.vertices) {
        UnloadMesh(surface->mesh);
        TraceLog(LOG_DEBUG, "RAYMAP: Old mesh unloaded");
    }
    
    surface->mesh = mesh;
    surface->meshNeedsUpdate = false;
    
    TraceLog(LOG_INFO, "RAYMAP: Mesh generated successfully [%d vertices, %d triangles]",
             vertexCount, triangleCount);
}

// Update mesh if dirty flag is set
static void rm_UpdateMesh(RM_Surface *surface)
{
    if (!surface) {
        TraceLog(LOG_WARNING, "RAYMAP: Cannot update NULL surface mesh");
        return;
    }
    
    if (!surface->meshNeedsUpdate) {
        return;
    }
    
    TraceLog(LOG_DEBUG, "RAYMAP: Updating mesh...");
    rm_GenerateBilinearMesh(surface, surface->meshColumns, surface->meshRows);
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Surface Management
//--------------------------------------------------------------------------------------------

RMAPI RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode)
{
    // Validate input
    if (width <= 0 || width > 8192) {
        TraceLog(LOG_ERROR, "RAYMAP: Invalid width %d (must be 1-8192)", width);
        return NULL;
    }
    if (height <= 0 || height > 8192) {
        TraceLog(LOG_ERROR, "RAYMAP: Invalid height %d (must be 1-8192)", height);
        return NULL;
    }
    
    // Allocate surface
    RM_Surface *surface = (RM_Surface *)RMMALLOC(sizeof(RM_Surface));
    if (!surface) {
        TraceLog(LOG_ERROR, "RAYMAP: Failed to allocate surface memory");
        return NULL;
    }
    
    // Initialize fields
    surface->width = width;
    surface->height = height;
    surface->mode = mode;
    surface->mesh = (Mesh){ 0 };
    surface->mesh.vertices = NULL;
    surface->material = (Material){ 0 };
    surface->homography = rm_Matrix3x3Identity();
    surface->homographyNeedsUpdate = true;
    
    // Set default quad (full rectangle)
    surface->quad = (RM_Quad){
        { 0.0f, 0.0f },
        { (float)width, 0.0f },
        { (float)width, (float)height },
        { 0.0f, (float)height }
    };
    
    // Set mesh resolution
    rm_GetDefaultResolutionForMode(mode, &surface->meshColumns, &surface->meshRows);
    surface->meshNeedsUpdate = true;
    
    // Create render texture
    surface->target = LoadRenderTexture(width, height);
    if (surface->target.id == 0) {
        TraceLog(LOG_ERROR, "RAYMAP: Failed to create %dx%d render texture", width, height);
        RMFREE(surface);
        return NULL;
    }
    
    // Create material
    surface->material = LoadMaterialDefault();
    if (surface->material.shader.id == 0) {
        TraceLog(LOG_WARNING, "RAYMAP: Material shader not properly loaded");
    }
    SetMaterialTexture(&surface->material, MATERIAL_MAP_DIFFUSE, surface->target.texture);
    
    // Generate mesh
    rm_UpdateMesh(surface);
    
    // Final verification
    if (surface->mesh.vertices == NULL) {
        TraceLog(LOG_ERROR, "RAYMAP: Failed to generate mesh");
        UnloadMaterial(surface->material);
        UnloadRenderTexture(surface->target);
        RMFREE(surface);
        return NULL;
    }
    
    TraceLog(LOG_INFO, "RAYMAP: Surface created [%dx%d, mode=%s, mesh=%dx%d]",
             width, height,
             mode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY",
             surface->meshColumns, surface->meshRows);
    
    return surface;
}

RMAPI void RM_DestroySurface(RM_Surface *surface)
{
    if (!surface) {
        TraceLog(LOG_WARNING, "RAYMAP: Attempted to destroy NULL surface");
        return;
    }
    
    // Unload GPU resources
    if (surface->material.shader.id > 0) {
        UnloadMaterial(surface->material);
        TraceLog(LOG_DEBUG, "RAYMAP: Material unloaded");
    }
    
    if (surface->target.id > 0) {
        UnloadRenderTexture(surface->target);
        TraceLog(LOG_DEBUG, "RAYMAP: RenderTexture unloaded");
    }
    
    if (surface->mesh.vertices != NULL) {
        UnloadMesh(surface->mesh);
        TraceLog(LOG_DEBUG, "RAYMAP: Mesh unloaded");
    }
    
    // Free CPU memory
    RMFREE(surface);
    
    TraceLog(LOG_INFO, "RAYMAP: Surface destroyed");
}

RMAPI bool RM_SetQuad(RM_Surface *surface, RM_Quad quad)
{
    if (!surface) {
        TraceLog(LOG_WARNING, "RAYMAP: Cannot set quad on NULL surface");
        return false;
    }
    
    // Validate minimum area
    float area = RM_GetQuadArea(quad);
    if (area < 100.0f) {
        TraceLog(LOG_WARNING, "RAYMAP: Quad too small (area=%.2f), rejected", area);
        return false;
    }
    
    // Validate distinct corners
    Vector2 corners[4] = { quad.topLeft, quad.topRight, quad.bottomRight, quad.bottomLeft };
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            float dist = Vector2Distance(corners[i], corners[j]);
            if (dist < 1.0f) {
                TraceLog(LOG_WARNING, "RAYMAP: Degenerate quad (corners too close), rejected");
                return false;
            }
        }
    }
    
    // Apply quad
    surface->quad = quad;
    surface->meshNeedsUpdate = true;
    surface->homographyNeedsUpdate = true;
    
    return true;
}

RMAPI RM_Quad RM_GetQuad(const RM_Surface *surface)
{
    if (!surface) {
        return (RM_Quad){ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    }
    return surface->quad;
}

RMAPI void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height)
{
    if (!surface) return;
    if (width) *width = surface->width;
    if (height) *height = surface->height;
}

RMAPI void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows)
{
    if (!surface) return;
    
    // Clamp to valid range
    if (columns < 4) columns = 4;
    if (columns > 64) columns = 64;
    if (rows < 4) rows = 4;
    if (rows > 64) rows = 64;
    
    // Skip if no change
    if (surface->meshColumns == columns && surface->meshRows == rows) {
        return;
    }
    
    // Update resolution
    surface->meshColumns = columns;
    surface->meshRows = rows;
    surface->meshNeedsUpdate = true;
}

RMAPI void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows)
{
    if (!surface) return;
    if (columns) *columns = surface->meshColumns;
    if (rows) *rows = surface->meshRows;
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Rendering
//--------------------------------------------------------------------------------------------

RMAPI void RM_BeginSurface(RM_Surface *surface)
{
    if (!surface) return;
    BeginTextureMode(surface->target);
}

RMAPI void RM_EndSurface(RM_Surface *surface)
{
    if (!surface) return;
    EndTextureMode();
}

RMAPI void RM_DrawSurface(const RM_Surface *surface)
{
    if (!surface) {
        TraceLog(LOG_WARNING, "RAYMAP: Attempted to draw NULL surface");
        return;
    }
    
    // Update mesh if needed
    if (surface->meshNeedsUpdate) {
        rm_UpdateMesh((RM_Surface *)surface);
    }
    
    // Validate mesh
    if (!surface->mesh.vertices) {
        TraceLog(LOG_ERROR, "RAYMAP: Surface has no mesh vertices");
        return;
    }
    if (surface->mesh.vboId[0] == 0) {
        TraceLog(LOG_ERROR, "RAYMAP: Mesh not uploaded to GPU");
        return;
    }
    if (surface->target.texture.id == 0) {
        TraceLog(LOG_ERROR, "RAYMAP: Surface render texture is invalid");
        return;
    }
    if (surface->material.shader.id == 0) {
        TraceLog(LOG_ERROR, "RAYMAP: Surface material shader is invalid");
        return;
    }
    
    // Draw mesh
    rlDisableDepthTest();
    rlDisableBackfaceCulling();
    
    DrawMesh(surface->mesh, surface->material, MatrixIdentity());
    
    rlEnableBackfaceCulling();
    rlEnableDepthTest();
}

RMAPI void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode)
{
    if (!surface) return;
    if (surface->mode == mode) return;
    
    surface->mode = mode;
    rm_GetDefaultResolutionForMode(mode, &surface->meshColumns, &surface->meshRows);
    surface->meshNeedsUpdate = true;
    surface->homographyNeedsUpdate = true;
}

RMAPI RM_MapMode RM_GetMapMode(const RM_Surface *surface)
{
    if (!surface) return RM_MAP_BILINEAR;
    return surface->mode;
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Calibration
//--------------------------------------------------------------------------------------------

RMAPI RM_Calibration RM_CalibrationDefault(RM_Surface *surface)
{
    RM_Calibration calib = { 0 };
    calib.surface = surface;
    calib.config = rm_GetDefaultCalibrationConfig();
    calib.activeCorner = -1;
    calib.dragOffset = (Vector2){ 0, 0 };
    calib.enabled = true;
    return calib;
}

RMAPI void RM_ToggleCalibration(RM_Calibration *calibration)
{
    if (!calibration) return;
    calibration->enabled = !calibration->enabled;
    if (!calibration->enabled) {
        calibration->activeCorner = -1;
    }
}

RMAPI void RM_UpdateCalibration(RM_Calibration *calibration)
{
    if (!calibration || !calibration->surface || !calibration->enabled) return;
    
    RM_Quad quad = RM_GetQuad(calibration->surface);
    Vector2 mousePos = GetMousePosition();
    float cornerRadius = calibration->config.cornerRadius;
    
    // Detect corner selection
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 corners[4] = {
            quad.topLeft,
            quad.topRight,
            quad.bottomRight,
            quad.bottomLeft
        };
        
        calibration->activeCorner = -1;
        
        for (int i = 0; i < 4; i++) {
            float dist = Vector2Distance(mousePos, corners[i]);
            if (dist <= cornerRadius * 1.5f) {
                calibration->activeCorner = i;
                calibration->dragOffset = Vector2Subtract(corners[i], mousePos);
                break;
            }
        }
    }
    
    // Drag active corner
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && calibration->activeCorner >= 0) {
        Vector2 newPos = Vector2Add(mousePos, calibration->dragOffset);
        
        switch (calibration->activeCorner) {
            case 0: quad.topLeft = newPos; break;
            case 1: quad.topRight = newPos; break;
            case 2: quad.bottomRight = newPos; break;
            case 3: quad.bottomLeft = newPos; break;
        }
        
        RM_SetQuad(calibration->surface, quad);
    }
    
    // Release corner
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        calibration->activeCorner = -1;
    }
}

RMAPI void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey)
{
    if (!calibration) return;
    
    if (toggleKey != 0 && IsKeyPressed(toggleKey)) {
        RM_ToggleCalibration(calibration);
    }
    
    RM_UpdateCalibration(calibration);
}

RMAPI void RM_DrawCalibration(RM_Calibration calibration)
{
    if (!calibration.surface || !calibration.enabled) return;
    
    RM_DrawCalibrationBorder(calibration);
    RM_DrawCalibrationGrid(calibration);
    RM_DrawCalibrationCorners(calibration);
}

RMAPI void RM_DrawCalibrationCorners(RM_Calibration calibration)
{
    if (!calibration.surface || !calibration.enabled || !calibration.config.showCorners) {
        return;
    }
    
    RM_Quad quad = RM_GetQuad(calibration.surface);
    RM_CalibrationConfig cfg = calibration.config;
    
    Vector2 corners[4] = {
        quad.topLeft,
        quad.topRight,
        quad.bottomRight,
        quad.bottomLeft
    };
    
    for (int i = 0; i < 4; i++) {
        Color color = (i == calibration.activeCorner) 
            ? cfg.selectedCornerColor 
            : cfg.cornerColor;
        
        DrawCircleV(corners[i], cfg.cornerRadius, color);
        DrawCircleLines((int)corners[i].x, (int)corners[i].y, cfg.cornerRadius, WHITE);
        DrawText(TextFormat("%d", i),
                 (int)corners[i].x - 5,
                 (int)corners[i].y - 10,
                 20, BLACK);
    }
}

RMAPI void RM_DrawCalibrationGrid(RM_Calibration calibration)
{
    if (!calibration.surface || !calibration.enabled || !calibration.config.showGrid) {
        return;
    }
    
    RM_Quad quad = RM_GetQuad(calibration.surface);
    RM_CalibrationConfig cfg = calibration.config;
    
    // Horizontal lines
    for (int x = 0; x <= cfg.gridResolutionX; x++) {
        float u = (float)x / (float)cfg.gridResolutionX;
        Vector2 top = Vector2Lerp(quad.topLeft, quad.topRight, u);
        Vector2 bottom = Vector2Lerp(quad.bottomLeft, quad.bottomRight, u);
        DrawLineV(top, bottom, cfg.gridColor);
    }
    
    // Vertical lines
    for (int y = 0; y <= cfg.gridResolutionY; y++) {
        float v = (float)y / (float)cfg.gridResolutionY;
        Vector2 left = Vector2Lerp(quad.topLeft, quad.bottomLeft, v);
        Vector2 right = Vector2Lerp(quad.topRight, quad.bottomRight, v);
        DrawLineV(left, right, cfg.gridColor);
    }
}

RMAPI void RM_DrawCalibrationBorder(RM_Calibration calibration)
{
    if (!calibration.surface || !calibration.enabled || !calibration.config.showBorder) {
        return;
    }
    
    RM_Quad quad = RM_GetQuad(calibration.surface);
    Color color = calibration.config.borderColor;
    
    DrawLineEx(quad.topLeft, quad.topRight, 2.0f, color);
    DrawLineEx(quad.topRight, quad.bottomRight, 2.0f, color);
    DrawLineEx(quad.bottomRight, quad.bottomLeft, 2.0f, color);
    DrawLineEx(quad.bottomLeft, quad.topLeft, 2.0f, color);
}

RMAPI void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight)
{
    if (!surface) return;
    
    int x = (screenWidth - surface->width) / 2;
    int y = (screenHeight - surface->height) / 2;
    
    RM_Quad centered = {
        { (float)x, (float)y },
        { (float)(x + surface->width), (float)y },
        { (float)(x + surface->width), (float)(y + surface->height) },
        { (float)x, (float)(y + surface->height) }
    };
    
    RM_SetQuad(surface, centered);
}

RMAPI void RM_ResetCalibrationQuad(RM_Calibration *calibration, int screenWidth, int screenHeight)
{
    if (!calibration || !calibration->surface) return;
    RM_ResetQuad(calibration->surface, screenWidth, screenHeight);
}

RMAPI int RM_GetActiveCorner(RM_Calibration calibration)
{
    return calibration.activeCorner;
}

RMAPI bool RM_IsCalibrating(RM_Calibration calibration)
{
    return (calibration.activeCorner >= 0 && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Configuration I/O
//--------------------------------------------------------------------------------------------

RMAPI bool RM_SaveConfig(const RM_Surface *surface, const char *filepath)
{
    if (!surface || !filepath) {
        TraceLog(LOG_ERROR, "RAYMAP: SaveConfig - Invalid parameters");
        return false;
    }
    
    FILE *file = fopen(filepath, "w");
    if (!file) {
        TraceLog(LOG_ERROR, "RAYMAP: SaveConfig - Cannot open '%s' for writing", filepath);
        return false;
    }
    
    // Write header
    fprintf(file, "# RAYMAP Config File\n");
    fprintf(file, "# Format: text/plain v1.0\n\n");
    
    // Surface dimensions
    fprintf(file, "[Surface]\n");
    fprintf(file, "width=%d\n", surface->width);
    fprintf(file, "height=%d\n\n", surface->height);
    
    // Mapping mode
    fprintf(file, "[Mode]\n");
    fprintf(file, "mode=%s\n\n", 
            surface->mode == RM_MAP_BILINEAR ? "BILINEAR" : "HOMOGRAPHY");
    
    // Mesh resolution
    fprintf(file, "[Mesh]\n");
    fprintf(file, "columns=%d\n", surface->meshColumns);
    fprintf(file, "rows=%d\n\n", surface->meshRows);
    
    // Quad corners
    fprintf(file, "[Quad]\n");
    fprintf(file, "topLeft=%.2f,%.2f\n", surface->quad.topLeft.x, surface->quad.topLeft.y);
    fprintf(file, "topRight=%.2f,%.2f\n", surface->quad.topRight.x, surface->quad.topRight.y);
    fprintf(file, "bottomRight=%.2f,%.2f\n", surface->quad.bottomRight.x, surface->quad.bottomRight.y);
    fprintf(file, "bottomLeft=%.2f,%.2f\n", surface->quad.bottomLeft.x, surface->quad.bottomLeft.y);
    
    fclose(file);
    
    TraceLog(LOG_INFO, "RAYMAP: Configuration saved to '%s'", filepath);
    return true;
}

RMAPI bool RM_LoadConfig(RM_Surface *surface, const char *filepath)
{
    if (!surface || !filepath) {
        TraceLog(LOG_ERROR, "RAYMAP: LoadConfig - Invalid parameters");
        return false;
    }
    
    FILE *file = fopen(filepath, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "RAYMAP: LoadConfig - Cannot open '%s' for reading", filepath);
        return false;
    }
    
    char line[256];
    RM_Quad quad = surface->quad;
    int meshCols = surface->meshColumns;
    int meshRows = surface->meshRows;
    RM_MapMode mode = surface->mode;
    bool quadLoaded = false;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        // Skip comments, empty lines, sections
        if (line[0] == '#' || line[0] == '\0' || line[0] == '[') {
            continue;
        }
        
        // Find separator
        char *equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        
        // Parse key-value pairs
        if (strcmp(key, "mode") == 0) {
            if (strcmp(value, "BILINEAR") == 0) {
                mode = RM_MAP_BILINEAR;
            } else if (strcmp(value, "HOMOGRAPHY") == 0) {
                mode = RM_MAP_HOMOGRAPHY;
            }
        }
        else if (strcmp(key, "columns") == 0) {
            meshCols = atoi(value);
        }
        else if (strcmp(key, "rows") == 0) {
            meshRows = atoi(value);
        }
        else if (strcmp(key, "topLeft") == 0) {
            if (sscanf(value, "%f,%f", &quad.topLeft.x, &quad.topLeft.y) == 2) {
                quadLoaded = true;
            }
        }
        else if (strcmp(key, "topRight") == 0) {
            sscanf(value, "%f,%f", &quad.topRight.x, &quad.topRight.y);
        }
        else if (strcmp(key, "bottomRight") == 0) {
            sscanf(value, "%f,%f", &quad.bottomRight.x, &quad.bottomRight.y);
        }
        else if (strcmp(key, "bottomLeft") == 0) {
            sscanf(value, "%f,%f", &quad.bottomLeft.x, &quad.bottomLeft.y);
        }
    }
    
    fclose(file);
    
    if (!quadLoaded) {
        TraceLog(LOG_WARNING, "RAYMAP: LoadConfig - No quad data found");
        return false;
    }
    
    // Apply configuration
    surface->mode = mode;
    surface->meshColumns = meshCols;
    surface->meshRows = meshRows;
    surface->meshNeedsUpdate = true;
    surface->homographyNeedsUpdate = true;
    
    RM_SetQuad(surface, quad);
    
    TraceLog(LOG_INFO, "RAYMAP: Configuration loaded from '%s'", filepath);
    return true;
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Geometry Utilities
//--------------------------------------------------------------------------------------------

RMAPI bool RM_PointInQuad(Vector2 point, RM_Quad quad)
{
    Vector2 center = {
        (quad.topLeft.x + quad.topRight.x + quad.bottomLeft.x + quad.bottomRight.x) / 4.0f,
        (quad.topLeft.y + quad.topRight.y + quad.bottomLeft.y + quad.bottomRight.y) / 4.0f
    };
    
    // Check if point is on same side of all edges as center
    if (!rm_SameSide(point, quad.topLeft, quad.topRight, center)) return false;
    if (!rm_SameSide(point, quad.topRight, quad.bottomRight, center)) return false;
    if (!rm_SameSide(point, quad.bottomRight, quad.bottomLeft, center)) return false;
    if (!rm_SameSide(point, quad.bottomLeft, quad.topLeft, center)) return false;
    
    return true;
}

RMAPI Rectangle RM_GetQuadBounds(RM_Quad quad)
{
    float minX = quad.topLeft.x;
    float maxX = quad.topLeft.x;
    float minY = quad.topLeft.y;
    float maxY = quad.topLeft.y;
    
    Vector2 corners[3] = { quad.topRight, quad.bottomLeft, quad.bottomRight };
    for (int i = 0; i < 3; i++) {
        if (corners[i].x < minX) minX = corners[i].x;
        if (corners[i].x > maxX) maxX = corners[i].x;
        if (corners[i].y < minY) minY = corners[i].y;
        if (corners[i].y > maxY) maxY = corners[i].y;
    }
    
    return (Rectangle){ minX, minY, maxX - minX, maxY - minY };
}

RMAPI Vector2 RM_GetQuadCenter(RM_Quad quad)
{
    return (Vector2){
        (quad.topLeft.x + quad.topRight.x + quad.bottomLeft.x + quad.bottomRight.x) / 4.0f,
        (quad.topLeft.y + quad.topRight.y + quad.bottomLeft.y + quad.bottomRight.y) / 4.0f
    };
}

RMAPI float RM_GetQuadArea(RM_Quad quad)
{
    // Shoelace formula for polygon area
    float x1 = quad.topLeft.x, y1 = quad.topLeft.y;
    float x2 = quad.topRight.x, y2 = quad.topRight.y;
    float x3 = quad.bottomRight.x, y3 = quad.bottomRight.y;
    float x4 = quad.bottomLeft.x, y4 = quad.bottomLeft.y;
    
    float area = 0.5f * fabsf(
        x1 * (y2 - y4) +
        x2 * (y3 - y1) +
        x3 * (y4 - y2) +
        x4 * (y1 - y3)
    );
    
    return area;
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Point Mapping
//--------------------------------------------------------------------------------------------

RMAPI Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint)
{
    if (!surface) {
        return (Vector2){ -1.0f, -1.0f };
    }
    
    // Clamp to [0,1]
    float u = fmaxf(0.0f, fminf(1.0f, texturePoint.x));
    float v = fmaxf(0.0f, fminf(1.0f, texturePoint.y));
    
    if (surface->mode == RM_MAP_HOMOGRAPHY) {
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

RMAPI Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint)
{
    if (!surface) {
        return (Vector2){ -1.0f, -1.0f };
    }
    
    // Check if point is inside quad
    if (!RM_PointInQuad(screenPoint, surface->quad)) {
        return (Vector2){ -1.0f, -1.0f };
    }
    
    // Use inverse homography (works for both modes)
    if (surface->homographyNeedsUpdate) {
        surface->homography = rm_ComputeHomography(surface->quad);
        surface->homographyNeedsUpdate = false;
    }
    
    Matrix3x3 invH = rm_Matrix3x3Inverse(surface->homography);
    Vector2 uv = rm_ApplyHomography(invH, screenPoint.x, screenPoint.y);
    
    // Clamp result
    uv.x = fmaxf(0.0f, fminf(1.0f, uv.x));
    uv.y = fmaxf(0.0f, fminf(1.0f, uv.y));
    
    return uv;
}

//--------------------------------------------------------------------------------------------
// Public API Implementation - Advanced/Debug
//--------------------------------------------------------------------------------------------

#ifdef RAYMAP_DEBUG

RMAPI Mesh *RM_GetSurfaceMesh(RM_Surface *surface)
{
    if (!surface) return NULL;
    return &surface->mesh;
}

#endif // RAYMAP_DEBUG

#endif // RAYMAP_IMPLEMENTATION
