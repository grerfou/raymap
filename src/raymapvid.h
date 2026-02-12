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
// Function Declarations (API)
//--------------------------------------------------------------------------------------------

// Load/Unload 
RMVAPI RMV_Video *RMV_LoadVideo(const char *filepath);
RMVAPI void RMV_UnloadVideo(RMV_Video *video);

// Metadata
RMVAPI RMV_VideoInfo RMV_GetVideoInfo(const RMV_Video *video);

// Texture access
RMVAPI Texture2D RMV_GetVideoTexture(const RMV_Video *video);

// Playback control
RMVAPI void RMV_UpdateVideo(RMV_Video *video, float deltaTime);
RMVAPI void RMV_PlayVideo(RMV_Video *video);
RMVAPI void RMV_PauseVideo(RMV_Video *video);
RMVAPI void RMV_StopVideo(RMV_Video *video);
RMVAPI void RMV_ToggleVideoPause(RMV_Video *video);

// State query
RMVAPI RMV_PlaybackState RMV_GetVideoState(const RMV_Video *video);
RMVAPI bool RMV_IsVideoPlaying(const RMV_Video *video);
RMVAPI bool RMV_IsVideoLoaded(const RMV_Video *video);

// Settings
RMVAPI void RMV_SetVideoLoop(RMV_Video *video, bool loop);

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

// FFMPEG includes
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

//--------------------------------------------------------------------------------------------
// Memory Management
//--------------------------------------------------------------------------------------------

#ifndef RMVMALLOC
    #define RMVMALLOC(size) malloc(size)
#endif
#ifndef RMVCALLOC
    #define RMVCALLOC(n, size) calloc(n, size)
#endif
#ifndef RMVFREE
    #define RMVFREE(ptr) free(ptr)
#endif

//--------------------------------------------------------------------------------------------
// Internal Structure
//--------------------------------------------------------------------------------------------

struct RMV_Video {
    // FFMPEG context
    AVFormatContext *formatCtx;
    AVCodecContext *codecCtx;
    const AVCodec *codec;
    AVFrame *frame;
    AVFrame *frameRGB;
    AVPacket *packet;
    struct SwsContext *swsCtx;

    // Stream info
    int videoStreamIndex;

    // raylib texture
    Texture2D texture;
    uint8_t *rgbBuffer;
    bool textureCreated;

    // Video Metadata
    int width;
    int height;
    float duration;
    float fps;
    const char *codecName;
    const char *formatName;
    bool hasAudio;

    // Playback state 
    RMV_PlaybackState state;
    float currentTime;
    bool loop;
    float frameAccumulator;

    // State flags
    bool isLoaded;
};


//--------------------------------------------------------------------------------------------
// Internal Helper Functions
//--------------------------------------------------------------------------------------------

// Cleanup function -> free resources 
// Cleanup function - frees all resources in proper order
static void rmv_CleanupVideo(RMV_Video *video) {
    if (!video) return;
    
    // Mark as not loaded first
    video->isLoaded = false;
    
    // Free RGB buffer
    if (video->rgbBuffer) {
        av_free(video->rgbBuffer);
        video->rgbBuffer = NULL;
    }
    
    // Unload Raylib texture
    if (video->texture.id > 0) {
        UnloadTexture(video->texture);
        video->texture.id = 0;
    }
    
    // Free swscale context
    if (video->swsCtx) {
        sws_freeContext(video->swsCtx);
        video->swsCtx = NULL;
    }
    
    // Free frames
    if (video->frameRGB) {
        av_frame_free(&video->frameRGB);
        // av_frame_free() sets pointer to NULL automatically
    }
    
    if (video->frame) {
        av_frame_free(&video->frame);
        // av_frame_free() sets pointer to NULL automatically
    }
    
    // Free packet
    if (video->packet) {
        av_packet_free(&video->packet);
        // av_packet_free() sets pointer to NULL automatically
    }
    
    // Free codec context
    if (video->codecCtx) {
        avcodec_free_context(&video->codecCtx);
        // avcodec_free_context() sets pointer to NULL automatically
    }
    
    // Close format context
    if (video->formatCtx) {
        avformat_close_input(&video->formatCtx);
        // avformat_close_input() sets pointer to NULL automatically
    }
}

//--------------------------------------------------------------------------------------------
// Public API
//--------------------------------------------------------------------------------------------

RMVAPI RMV_Video *RMV_LoadVideo(const char *filepath) {
    // Validate input
    if (!filepath) {
        TraceLog(LOG_ERROR, "RAYMAPVID: NULL filepath provided");
        return NULL;
    }

    // Allocate video structure
    RMV_Video *video = (RMV_Video *)RMVCALLOC(1, sizeof(RMV_Video));
    if (!video) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to allocate video structure");
        return NULL;
    }

    // Open video file
    video->formatCtx = NULL;
    if (avformat_open_input(&video->formatCtx, filepath, NULL, NULL) != 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Could not open file '%s'", filepath);
        RMVFREE(video);
        return NULL;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(video->formatCtx, NULL) < 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Could not find stream information");
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Find video stream
    video->videoStreamIndex = -1;
    for (unsigned int i = 0; i < video->formatCtx->nb_streams; i++) {
        if (video->formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video->videoStreamIndex = i;
            break;
        }
    }

    if (video->videoStreamIndex == -1) {
        TraceLog(LOG_ERROR, "RAYMAPVID: No video stream found");
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Get codec parameters
    AVCodecParameters *codecParams = video->formatCtx->streams[video->videoStreamIndex]->codecpar;

    // Find decoder
    video->codec = avcodec_find_decoder(codecParams->codec_id);
    if (!video->codec) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Unsupported codec");
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Allocate codec context
    video->codecCtx = avcodec_alloc_context3(video->codec);
    if (!video->codecCtx) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to allocate codec context");
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Copy codec parameters to context
    if (avcodec_parameters_to_context(video->codecCtx, codecParams) < 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to copy codec parameters");
        avcodec_free_context(&video->codecCtx);
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Open codec
    if (avcodec_open2(video->codecCtx, video->codec, NULL) < 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Could not open codec");
        avcodec_free_context(&video->codecCtx);
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Log codec info
    TraceLog(LOG_INFO, "RAYMAPVID: Codec opened: %s", avcodec_get_name(codecParams->codec_id));
    TraceLog(LOG_INFO, "RAYMAPVID: Dimensions: %dx%d", video->codecCtx->width, video->codecCtx->height);
    TraceLog(LOG_INFO, "RAYMAPVID: Pixel format: %s (%d)",
             av_get_pix_fmt_name(video->codecCtx->pix_fmt),
             video->codecCtx->pix_fmt);

    // Extract video info
    video->width = video->codecCtx->width;
    video->height = video->codecCtx->height;

    // Validate dimensions
    if (video->width <= 0 || video->height <= 0 ||
        video->width > 8192 || video->height > 8192) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Invalid dimensions: %dx%d", video->width, video->height);
        avcodec_free_context(&video->codecCtx);
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Validate pixel format
    if (video->codecCtx->pix_fmt == AV_PIX_FMT_NONE) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Invalid pixel format");
        avcodec_free_context(&video->codecCtx);
        avformat_close_input(&video->formatCtx);
        RMVFREE(video);
        return NULL;
    }

    // Calculate FPS
    AVRational frameRate = video->formatCtx->streams[video->videoStreamIndex]->r_frame_rate;
    video->fps = av_q2d(frameRate);

    // Calculate duration
    int64_t duration = video->formatCtx->duration;
    video->duration = (duration != AV_NOPTS_VALUE) ? (float)duration / AV_TIME_BASE : 0.0f;

    // Store codec and format names
    video->codecName = avcodec_get_name(codecParams->codec_id);
    video->formatName = video->formatCtx->iformat->name;

    // Check for audio stream
    video->hasAudio = false;
    for (unsigned int i = 0; i < video->formatCtx->nb_streams; i++) {
        if (video->formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            video->hasAudio = true;
            break;
        }
    }

    // Allocate frames
    video->frame = av_frame_alloc();
    video->frameRGB = av_frame_alloc();
    if (!video->frame || !video->frameRGB) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to allocate frames");
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    // Allocate packet
    video->packet = av_packet_alloc();
    if (!video->packet) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to allocate packet");
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    // Determine buffer size for RGB frame
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, video->width, video->height, 1);
    if (numBytes <= 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Invalid buffer size: %d", numBytes);
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    video->rgbBuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    if (!video->rgbBuffer) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to allocate RGB buffer (%d bytes)", numBytes);
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    TraceLog(LOG_INFO, "RAYMAPVID: RGB buffer allocated (%d bytes)", numBytes);

    // Assign buffer to frameRGB with proper alignment
    int ret = av_image_fill_arrays(
        video->frameRGB->data,
        video->frameRGB->linesize,
        video->rgbBuffer,
        AV_PIX_FMT_RGB24,
        video->width,
        video->height,
        1  // alignment
    );

    if (ret < 0) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to fill image arrays (error %d)", ret);
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    // Initialize swscale context (YUV to RGB conversion)
    int swsFlags = SWS_BILINEAR | SWS_FULL_CHR_H_INT;

    TraceLog(LOG_INFO, "RAYMAPVID: Creating swscale context");
    TraceLog(LOG_INFO, "  Source: %dx%d format=%s",
             video->codecCtx->width,
             video->codecCtx->height,
             av_get_pix_fmt_name(video->codecCtx->pix_fmt));
    TraceLog(LOG_INFO, "  Dest: %dx%d format=RGB24", video->width, video->height);

    video->swsCtx = sws_getContext(
        video->codecCtx->width,      // src width
        video->codecCtx->height,     // src height
        video->codecCtx->pix_fmt,    // src format
        video->codecCtx->width,      // dst width
        video->codecCtx->height,     // dst height
        AV_PIX_FMT_RGB24,            // dst format
        swsFlags,                    // flags
        NULL,                        // src filter
        NULL,                        // dst filter
        NULL                         // param
    );

    if (!video->swsCtx) {
        TraceLog(LOG_ERROR, "RAYMAPVID: Failed to initialize swscale context");
        TraceLog(LOG_ERROR, "  Format: %s (%d)",
                 av_get_pix_fmt_name(video->codecCtx->pix_fmt),
                 video->codecCtx->pix_fmt);
        rmv_CleanupVideo(video);
        RMVFREE(video);
        return NULL;
    }

    TraceLog(LOG_INFO, "RAYMAPVID: Swscale context created successfully");

    // Initialize texture as empty (will be created on first GetVideoTexture call)
    video->texture = (Texture2D){0};
    video->textureCreated = false;   
    video->isLoaded = true;

    // Init video playback state
    video->state = RMV_STATE_STOPPED;
    video->currentTime = 0.0f;
    video->loop = false;
    video->frameAccumulator = 0.0f;

    TraceLog(LOG_INFO, "RAYMAPVID: Video loaded successfully: %dx%d @ %.2f fps",
             video->width, video->height, video->fps);

    return video;
}

RMVAPI void RMV_UnloadVideo(RMV_Video *video) {
    if (!video) return;
    
    rmv_CleanupVideo(video);
    RMVFREE(video);
    
    TraceLog(LOG_INFO, "RAYMAPVID: Video unloaded");
}


RMVAPI RMV_VideoInfo RMV_GetVideoInfo(const RMV_Video *video) {
    RMV_VideoInfo info = {0};

    if (!video){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_GetVideoInfo() called with NULL video");
        return info;   
    }

    info.width = video->width;
    info.height = video->height;
    info.duration = video->duration;
    info.fps = video->fps;
    info.codec = video->codecName;
    info.format = video->formatName;
    info.hasAudio = video->hasAudio;
    info.hwaccel = RMV_HWACCEL_NONE;

    return info;
}

RMVAPI Texture2D RMV_GetVideoTexture(const RMV_Video *video) {
    if (!video){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_GetVideoTexture() called with NULL video");
        return (Texture2D){0};
    }
    
    // Create texture on first access (lazy initialization)
    if (!video->textureCreated) {
        // Need to cast away const to modify texture (safe here)
        RMV_Video *v = (RMV_Video *)video;
        
        Image img = {
            .data = v->rgbBuffer,
            .width = v->width,
            .height = v->height,
            .mipmaps = 1,
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8
        };
        v->texture = LoadTextureFromImage(img);
        //UnloadImage(img);
        
        if (v->texture.id == 0) {
            TraceLog(LOG_ERROR, "RAYMAPVID: Failed to create texture");
            return (Texture2D){0};
        }
        
        v->textureCreated = true;
        TraceLog(LOG_INFO, "RAYMAPVID: Texture created on first access");
    }

    return video->texture;
}

RMVAPI void RMV_UpdateVideo(RMV_Video *video, float deltaTime) {

    if (!video || !video->isLoaded){
        return;
    }

    // Only Update if playing
    if (video->state != RMV_STATE_PLAYING){
        return;
    }

    // Create texture on first update if not already created (lazy init)
    if (!video->textureCreated) {
        // Create image with correct format pointing to our RGB buffer
        Image img = {
            .data = video->rgbBuffer,
            .width = video->width,
            .height = video->height,
            .mipmaps = 1,
            .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8
        };
        video->texture = LoadTextureFromImage(img);
        //UnloadImage(img);
        
        if (video->texture.id == 0) {
            TraceLog(LOG_ERROR, "RAYMAPVID: Failed to create texture");
            video->state = RMV_STATE_ERROR;
            return;
        }
        
        video->textureCreated = true;
        TraceLog(LOG_INFO, "RAYMAPVID: Texture created on first update");
    }

    // Accumulate time
    video->frameAccumulator += deltaTime;
    video->currentTime += deltaTime;

    // Calculate frame time based on FPS
    float frameTime = 1.0f / video->fps;

    // Decode frames if enough time has accumulated
    while (video->frameAccumulator >= frameTime){
        video->frameAccumulator -= frameTime;

        // Try decode one frame
        bool frameDecoded = false;

        while (!frameDecoded){
            // Read packet from stream
            int ret = av_read_frame(video->formatCtx, video->packet);

            if (ret < 0){
                // End of file or error
                if (ret == AVERROR_EOF){
                    TraceLog(LOG_INFO, "RAYMAPVID: End of Video reached");

                    if (video->loop){
                        // Loop seek back restart
                        av_seek_frame(video->formatCtx, video->videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
                        avcodec_flush_buffers(video->codecCtx);
                        video->currentTime = 0.0f;
                        video->frameAccumulator = 0.0f;
                        continue;
                    } else {
                        // stop playback
                        video->state = RMV_STATE_STOPPED;
                        video->currentTime = 0.0f;
                        video->frameAccumulator = 0.0f;
                        return;
                    }
                } else {
                    TraceLog(LOG_ERROR, "RAYMAPVID: RAYMAPVID: Error reading frame: %d", ret);
                    return;
                }
            }

            // check packet is from video stream
            if (video->packet->stream_index == video->videoStreamIndex){
                // Send packet to decoder
                ret = avcodec_send_packet(video->codecCtx, video->packet);

                if (ret < 0){
                    TraceLog(LOG_ERROR, "RAYMAPVID: RAYMAPVID: Error reading frame: %d", ret);
                    av_packet_unref(video->packet);
                    return;
                }

                // Receive decoded frame
                ret = avcodec_receive_frame(video->codecCtx, video->frame);

                if (ret == 0){
                    // Frame decoded successfull

                    // ConvertYUV top RGB
                    sws_scale(
                            video->swsCtx,
                            (const uint8_t *const *)video->frame->data,
                            video->frame->linesize,
                            0,
                            video->codecCtx->height,
                            video->frameRGB->data,
                            video->frameRGB->linesize
                    );

                    // Update texture with new frame
                    UpdateTexture(video->texture, video->rgbBuffer);

                    frameDecoded = true;

                } else if (ret == AVERROR(EAGAIN)){
                    // Need more packets
                    // Continue to readnext packet
                } else if (ret == AVERROR_EOF){
                    // Decoder has been fully flushed
                    frameDecoded = true;
                } else {
                    TraceLog(LOG_ERROR, "RAYMAPVID: Error receiving frame: %d", ret);
                    av_packet_unref(video->packet);
                    return;
                }
            }

            av_packet_unref(video->packet);

            if (frameDecoded){
                break;
            }
        }
    }
}

RMVAPI void RMV_SetVideoLoop(RMV_Video *video, bool loop){
    if (!video || !video->isLoaded){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_SetVideoLoop() called with invalid video");
        return;
    }

    video->loop = loop;
    TraceLog(LOG_INFO, "RAYMAPVID: Video loop %s", loop ? "enabled" : "disabled");
}

RMVAPI void RMV_PlayVideo(RMV_Video *video) {

    if (!video || !video->isLoaded){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_PlayVideo() called with invalid video");
        return;
    }

    video->state = RMV_STATE_PLAYING;
    TraceLog(LOG_INFO, "RAYMAPVID: Video Playing");
}

RMVAPI void RMV_PauseVideo(RMV_Video *video) {

    if (!video || !video->isLoaded){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_PausedVideo called with invalid video");
        return;
    }

    video->state = RMV_STATE_PAUSED;
    TraceLog(LOG_INFO, "RAYMAPVID: Video Paused");
}

RMVAPI void RMV_StopVideo(RMV_Video *video) {
    
    if (!video || !video->isLoaded){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_StopVideo called with invalid video");
        return;
    }

    video->state = RMV_STATE_STOPPED;
    video->currentTime = 0.0f;
    TraceLog(LOG_INFO, "RAYMAPVID: Video stopped");
}

RMVAPI void RMV_ToggleVideoPause(RMV_Video *video){
    
    if (!video || !video->isLoaded){
        TraceLog(LOG_WARNING, "RAYMAPVID: RMV_StopVideo called with invalid video");
        return;
    }

    if (video->state == RMV_STATE_PLAYING){
        RMV_PauseVideo(video);
    }else if (video->state == RMV_STATE_PAUSED){
        RMV_PlayVideo(video);
    }
}

RMVAPI RMV_PlaybackState RMV_GetVideoState(const RMV_Video *video) {

    if (!video || !video->isLoaded){
        return RMV_STATE_ERROR;
    }

    return video->state;
}

RMVAPI bool RMV_IsVideoPlaying(const RMV_Video *video) {

    if (!video || !video->isLoaded){
        return false;
    }

    return (video->state == RMV_STATE_PLAYING);
}

RMVAPI bool RMV_IsVideoLoaded(const RMV_Video *video) {
    return (video != NULL && video->isLoaded);
}

#endif // RAYMAPVID_IMPLEMENTATION
