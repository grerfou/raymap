# RayMap

> **Professional projection mapping and surface warping library for Raylib**

[![License](https://img.shields.io/badge/license-zlib%2Flibpng-blue.svg)](LICENSE)
[![Raylib](https://img.shields.io/badge/raylib-5.0%2B-red.svg)](https://www.raylib.com)
[![Version](https://img.shields.io/badge/version-1.1.0-green.svg)](https://github.com/grerfou/raymap/releases)

RayMap is a powerful, header-only C library for real-time projection mapping, surface warping, and interactive calibration. Perfect for video mapping installations, multi-projector setups, and creative projection on non-flat surfaces.

##  Features

### Core Capabilities
-  **Bilinear & Homography Warping** - Perspective-correct transformations
-  **Real-time Mesh Deformation** - Dynamic mesh generation with configurable resolution
-  **Interactive Calibration** - Drag-and-drop corner adjustment with visual feedback
-  **Configuration Save/Load** - Persistent calibration storage
-  **Point Mapping Utilities** - Bidirectional coordinate transformation

### Video Extension (RayMapVid)
-  **FFmpeg Integration** - Professional video decoding (H.264, H.265, VP9, etc.)
-  **Hardware Acceleration Ready** - NVDEC, VAAPI, VideoToolbox support (coming soon)
-  **Playback Control** - Play, pause, loop, and state management
-  **Seamless Integration** - Direct-to-texture rendering

### Architecture
-  **Header-Only** - Single file, zero build dependencies
-  **Opaque Types** - Clean API with hidden implementation
-  **Customizable Memory** - Override allocators for embedded systems
-  **Lazy Evaluation** - Optimized mesh updates only when needed

##  Quick Start

### Installation

RayMap is header-only. Just copy the files to your project:

```bash
# Core library
curl -O https://raw.githubusercontent.com/grerfou/raymap/main/raymap.h

# Video extension (optional)
curl -O https://raw.githubusercontent.com/grerfou/raymap/main/raymapvid.h
```

### Minimal Example

```c
#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

int main(void)
{
    InitWindow(1920, 1080, "RayMap - Minimal");
    
    // Create a 800x600 surface with homography warping
    RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);
    
    // Setup calibration
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    while (!WindowShouldClose())
    {
        // Update calibration (toggle with C key)
        RM_UpdateCalibrationInput(&calib, KEY_C);
        
        // Draw content to surface
        RM_BeginSurface(surface);
            ClearBackground(DARKBLUE);
            DrawText("Hello RayMap!", 250, 250, 60, WHITE);
        RM_EndSurface(surface);
        
        // Render warped surface
        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(surface);
            RM_DrawCalibration(calib);  // Show calibration overlay
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    return 0;
}
```

**Compile:**
```bash
gcc minimal.c -o minimal -lraylib -lm
```

**Controls:**
- `C` - Toggle calibration mode
- `Mouse` - Drag corners to adjust warping

## Documentation

- **[API Reference](docs/API.md)** - Complete function documentation
- **[Examples](examples/)** - examples from basic to advanced

##  Use Cases

### Video Mapping Installations
```c
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
#include "raymapvid.h"

RM_Surface *surface = RM_CreateSurface(1920, 1080, RM_MAP_HOMOGRAPHY);
RMV_Video *video = RMV_LoadVideo("content.mp4");

RMV_SetVideoLoop(video, true);
RMV_PlayVideo(video);

// In main loop:
RMV_UpdateVideo(video, GetFrameTime());

RM_BeginSurface(surface);
    DrawTexture(RMV_GetVideoTexture(video), 0, 0, WHITE);
RM_EndSurface(surface);

RM_DrawSurface(surface);
```

### Multi-Projector Setup
```c
// Create 4 surfaces for quad projector setup
RM_Surface *projectors[4];
for (int i = 0; i < 4; i++) {
    projectors[i] = RM_CreateSurface(1920, 1080, RM_MAP_HOMOGRAPHY);
    RM_LoadConfig(projectors[i], TextFormat("projector_%d.cfg", i));
}

// Render each with edge blending
for (int i = 0; i < 4; i++) {
    RM_DrawSurface(projectors[i]);
}
```

### Interactive Touch Surface
```c
// Detect clicks on warped surface
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    Vector2 texCoords = RM_UnmapPoint(surface, mousePos);
    
    if (texCoords.x >= 0) {
        // Click is inside warped quad
        TraceLog(LOG_INFO, "Clicked at UV: %.2f, %.2f", 
                 texCoords.x, texCoords.y);
    }
}
```

##  Dependencies

### Core Library (raymap.h)
- **Raylib 5.0+** - Graphics library
- **Standard C Library** - `math.h`, `stdlib.h`, `string.h`

### Video Extension (raymapvid.h)
- **FFmpeg 4.4+** - Video decoding
  - `libavcodec` (required)
  - `libavformat` (required)
  - `libavutil` (required)
  - `libswscale` (required)

### Installation (Ubuntu/Debian)
```bash
# Raylib
sudo apt install libraylib-dev

# FFmpeg (for video support)
sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
```

### Installation (macOS)
```bash
# Raylib
brew install raylib

# FFmpeg
brew install ffmpeg
```

##  Advanced Configuration

### Custom Memory Allocators
```c
#define RMMALLOC(sz)     my_malloc(sz)
#define RMCALLOC(n,sz)   my_calloc(n,sz)
#define RMFREE(p)        my_free(p)

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
```

### Debug Mode
```c
#define RAYMAP_DEBUG
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

// Enables RM_GetSurfaceMesh() for debugging
Mesh *mesh = RM_GetSurfaceMesh(surface);
```

### Standalone Video Library
```c
// Use raymapvid without raymap
#define RAYMAPVID_IMPLEMENTATION
#include "raymapvid.h"
```

##  Building Examples

```bash
cd examples
make

# Or individually:
gcc 01_minimal_surface.c -o 01_minimal_surface -lraylib -lm
gcc 09_basic_video_playback.c -o 09_basic_video_playback \
    -lraylib -lm -lavcodec -lavformat -lavutil -lswscale
```

##  Testing

```bash
# Run test suite (requires Unity test framework)
cd tests
make test

# Memory leak check
valgrind --leak-check=full ./tests/test_raymap
```

##  Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

**Areas for contribution:**
-  Unit tests
-  Documentation improvements
-  Example projects
-  Bug reports
-  Feature requests

## License

RayMap is licensed under the **zlib/libpng license** - same as Raylib.

```
Copyright (c) 2026 grerfou

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
```

**FFmpeg Notice (raymapvid.h):**
- FFmpeg libraries are licensed under **LGPL 2.1+**
- Dynamic linking is used (no source code modifications)
- FFmpeg source available at [ffmpeg.org](https://ffmpeg.org)

## Acknowledgments

- **Raylib** - raysan5 and contributors
- **FFmpeg** - FFmpeg team
- **stb libraries** - Sean Barrett (header-only inspiration)
- **OpenCV** - Homography implementation reference

## Contact & Support

- **Issues**: [GitHub Issues](https://github.com/grerfou/raymap/issues)
- **Discussions**: [GitHub Discussions](https://github.com/grerfou/raymap/discussions)

