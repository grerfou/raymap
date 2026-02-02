# Changelog

All notable changes to RayMap will be documented in this file.

## [1.0.0] - 2025-01-XX

### Added
- Initial public release
- Core projection mapping features (bilinear and homography)
- Interactive calibration system
- Multi-surface support
- Configuration save/load (text format)
- 16 comprehensive memory tests
- Examples: basic, calibration, multi-mapping
- Full API documentation

### Features
- Real-time mesh warping
- GPU-accelerated rendering
- Memory-safe implementation (Valgrind + AddressSanitizer verified)
- Single-header library design
- Cross-platform support (Linux, Windows, macOS)

### Performance
- Typical 60+ FPS with 3 surfaces @ 1920×1080
- Lazy mesh regeneration (only when needed)
- Efficient GPU upload strategy

---

## [Unreleased]

### Planned
- Error handling system (RM_GetLastError)
- Binary config format
- Homography coordinate normalization
- SIMD optimizations (optional)
- Additional examples

---

[1.0.0]: https://github.com/grerfou/raymap/releases/tag/v1.0.0
