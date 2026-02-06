/**********************************************************************************************
*
*   raymapvid v0.1.0 - Video decoding library for RayMap (FFmpeg-based)
*
*   DESCRIPTION:
*       Single-header library for professional video decoding.
*       Designed for RayMap projection mapping but usable standalone.
*
*   FEATURES (v0.1.0 - Foundation):
*       - Header-only architecture
*       - Opaque types (RMV_Video)
*       - Auto-detection RAYMAP_IMPLEMENTATION
*       - Clean API with proper namespacing (RMV_ prefix)
*
*   CONFIGURATION:
*       Standard usage with RayMap:
*           #define RAYMAP_IMPLEMENTATION
*           #include "raymap.h"
*           #include "raymapvid.h"  // Auto-implemented!
*
*       Standalone usage:
*           #define RAYMAPVID_IMPLEMENTATION
*           #include "raymapvid.h"
*
*   DEPENDENCIES:
*       - raylib 5.0+ (zlib/libpng license)
*       - FFmpeg 4.4+ LGPL (libavcodec, libavformat, libavutil, libswscale)
*
*   LICENSING:
*       raymapvid code: zlib/libpng (permissive, commercial use OK)
*       FFmpeg runtime: LGPL 2.1+ (dynamic linking, source available)
*       
*       See LICENSE file for full details.
*
*   CONTRIBUTORS:
*       grerfou - Initial implementation
*
**********************************************************************************************/

#ifndef RAYMAPVID_H
#define RAYMAPVID_H

//--------------------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------------------
#include "raylib.h"
#include <stdbool.h>

//--------------------------------------------------------------------------------------------
// Defines and Macros
//--------------------------------------------------------------------------------------------
#ifndef RMVAPI
    #define RMVAPI extern
#endif

// Version
#define RAYMAPVID_VERSION_MAJOR 0
#define RAYMAPVID_VERSION_MINOR 1
#define RAYMAPVID_VERSION_PATCH 0

//--------------------------------------------------------------------------------------------
// Types and Structures (OPAQUE)
//--------------------------------------------------------------------------------------------

// Opaque video handle (implementation hidden)
typedef struct RMV_Video RMV_Video;

//--------------------------------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------------------------------

// Hardware acceleration type
typedef enum {
    RMV_HWACCEL_NONE = 0,           // Software decoding
    RMV_HWACCEL_AUTO,               // Auto-detect best available
    RMV_HWACCEL_NVDEC,              // NVIDIA (CUDA/NVDEC)
    RMV_HWACCEL_VAAPI,              // Intel/AMD (Linux)
    RMV_HWACCEL_VIDEOTOOLBOX,       // Apple (macOS/iOS)
    RMV_HWACCEL_D3D11,              // Windows (Direct3D 11)
    RMV_HWACCEL_DXVA2               // Windows (DirectX Video Acceleration)
} RMV_HWAccelType;

// Playback state
typedef enum {
    RMV_STATE_STOPPED = 0,
    RMV_STATE_PLAYING,
    RMV_STATE_PAUSED,
    RMV_STATE_ERROR
} RMV_PlaybackState;

//--------------------------------------------------------------------------------------------
// Public Structures
//--------------------------------------------------------------------------------------------

// Video information (metadata)
typedef struct {
    int width;                      // Video width in pixels
    int height;                     // Video height in pixels
    float duration;                 // Duration in seconds
    float fps;                      // Frames per second
    const char *codec;              // Codec name (e.g., "h264")
    const char *format;             // Container format (e.g., "mp4")
    bool hasAudio;                  // Audio stream present
    RMV_HWAccelType hwaccel;        // Active hardware acceleration
} RMV_VideoInfo;

//--------------------------------------------------------------------------------------------
// Function Declarations (API - NOT IMPLEMENTED YET)
//--------------------------------------------------------------------------------------------

// NOTE: These are declarations only for Étape 1.1
// Implementation will come in future steps

// Load/Unload (Étape 1.2)
RMVAPI RMV_Video *RMV_LoadVideo(const char *filepath);
RMVAPI void RMV_UnloadVideo(RMV_Video *video);

// Metadata (Étape 1.3)
RMVAPI RMV_VideoInfo RMV_GetVideoInfo(const RMV_Video *video);

// Texture access (Étape 1.4)
RMVAPI Texture2D RMV_GetVideoTexture(const RMV_Video *video);

// Playback control (Phase 2)
RMVAPI void RMV_UpdateVideo(RMV_Video *video, float deltaTime);
RMVAPI void RMV_PlayVideo(RMV_Video *video);
RMVAPI void RMV_PauseVideo(RMV_Video *video);
RMVAPI void RMV_StopVideo(RMV_Video *video);

// State query (Phase 2)
RMVAPI RMV_PlaybackState RMV_GetVideoState(const RMV_Video *video);
RMVAPI bool RMV_IsVideoPlaying(const RMV_Video *video);
RMVAPI bool RMV_IsVideoLoaded(const RMV_Video *video);

#endif // RAYMAPVID_H

/***********************************************************************************
*
*   RAYMAPVID IMPLEMENTATION
*
************************************************************************************/

// Auto-detect: If RAYMAP_IMPLEMENTATION is defined, enable raymapvid too
#if defined(RAYMAP_IMPLEMENTATION) && !defined(RAYMAPVID_IMPLEMENTATION)
    #define RAYMAPVID_IMPLEMENTATION
#endif

#if defined(RAYMAPVID_IMPLEMENTATION)

#undef RMVAPI
#define RMVAPI

//--------------------------------------------------------------------------------------------
// Implementation Includes
//--------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------------------------------------------------------------------
// Internal Structure (HIDDEN FROM USER)
//--------------------------------------------------------------------------------------------

struct RMV_Video {
    // This is just a placeholder structure for Étape 1.1
    // Real implementation will come in Étape 1.2
    int dummy;  // Prevents empty struct warning
};

//--------------------------------------------------------------------------------------------
// Stub Implementations (NOT FUNCTIONAL YET)
//--------------------------------------------------------------------------------------------

// These are stubs that return NULL/defaults for compilation testing only
// Real implementation comes in later steps

RMVAPI RMV_Video *RMV_LoadVideo(const char *filepath) {
    (void)filepath;  // Unused parameter
    TraceLog(LOG_WARNING, "RAYMAPVID: RMV_LoadVideo() not implemented yet (Étape 1.2)");
    return NULL;
}

RMVAPI void RMV_UnloadVideo(RMV_Video *video) {
    (void)video;  // Unused parameter
    TraceLog(LOG_WARNING, "RAYMAPVID: RMV_UnloadVideo() not implemented yet (Étape 1.2)");
}

RMVAPI RMV_VideoInfo RMV_GetVideoInfo(const RMV_Video *video) {
    (void)video;  // Unused parameter
    RMV_VideoInfo info = {0};
    TraceLog(LOG_WARNING, "RAYMAPVID: RMV_GetVideoInfo() not implemented yet (Étape 1.3)");
    return info;
}

RMVAPI Texture2D RMV_GetVideoTexture(const RMV_Video *video) {
    (void)video;  // Unused parameter
    TraceLog(LOG_WARNING, "RAYMAPVID: RMV_GetVideoTexture() not implemented yet (Étape 1.4)");
    return (Texture2D){0};
}

RMVAPI void RMV_UpdateVideo(RMV_Video *video, float deltaTime) {
    (void)video;
    (void)deltaTime;
    // Stub - no implementation yet (Phase 2)
}

RMVAPI void RMV_PlayVideo(RMV_Video *video) {
    (void)video;
    // Stub - no implementation yet (Phase 2)
}

RMVAPI void RMV_PauseVideo(RMV_Video *video) {
    (void)video;
    // Stub - no implementation yet (Phase 2)
}

RMVAPI void RMV_StopVideo(RMV_Video *video) {
    (void)video;
    // Stub - no implementation yet (Phase 2)
}

RMVAPI RMV_PlaybackState RMV_GetVideoState(const RMV_Video *video) {
    (void)video;
    return RMV_STATE_STOPPED;
}

RMVAPI bool RMV_IsVideoPlaying(const RMV_Video *video) {
    (void)video;
    return false;
}

RMVAPI bool RMV_IsVideoLoaded(const RMV_Video *video) {
    (void)video;
    return false;
}

#endif // RAYMAPVID_IMPLEMENTATION
