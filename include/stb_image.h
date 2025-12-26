/**
 * =============================================================================
 * STB Image - Single-Header Image Loading Library
 * =============================================================================
 * This is a minimal implementation inspired by the stb_image library.
 * In a real project, you would use the full stb_image.h from:
 * https://github.com/nothings/stb
 * 
 * This simplified version demonstrates the concepts of image loading for
 * texture mapping in OpenGL. It supports loading raw image data that can
 * be uploaded to the GPU as a texture.
 * =============================================================================
 */

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace stbi {

/**
 * Load an image from a file.
 * 
 * @param filename Path to the image file
 * @param width Output: image width in pixels
 * @param height Output: image height in pixels  
 * @param channels Output: number of color channels (1=gray, 3=RGB, 4=RGBA)
 * @param desired_channels Requested channels (0 = use file's channels)
 * @return Pointer to image data, or nullptr on failure. Caller must free with stbi_image_free()
 * 
 * The returned data is in row-major order, starting from the top-left pixel.
 * Each pixel has 'channels' bytes (e.g., 4 bytes for RGBA: R, G, B, A).
 */
inline unsigned char* stbi_load(const char* filename, int* width, int* height, 
                                int* channels, int desired_channels) {
    // For this educational example, we'll generate procedural textures
    // instead of loading from files. This avoids the complexity of
    // parsing various image formats (PNG, JPG, BMP, etc.)
    
    (void)filename; // Unused in this simplified version
    
    // Generate a default 64x64 texture with a checkerboard pattern
    const int w = 64;
    const int h = 64;
    const int c = (desired_channels > 0) ? desired_channels : 4;
    
    *width = w;
    *height = h;
    *channels = c;
    
    unsigned char* data = (unsigned char*)malloc(w * h * c);
    if (!data) return nullptr;
    
    // Create a checkerboard pattern - useful for debugging UV coordinates
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * c;
            // Alternate colors in 8x8 blocks
            bool isLight = ((x / 8) + (y / 8)) % 2 == 0;
            unsigned char value = isLight ? 200 : 100;
            
            data[idx] = value;     // R
            if (c > 1) data[idx + 1] = value; // G
            if (c > 2) data[idx + 2] = value; // B
            if (c > 3) data[idx + 3] = 255;   // A (fully opaque)
        }
    }
    
    return data;
}

/**
 * Free image data allocated by stbi_load.
 */
inline void stbi_image_free(void* data) {
    free(data);
}

/**
 * Set whether to flip images vertically on load.
 * OpenGL expects textures with origin at bottom-left, but most image
 * formats store pixels starting from top-left.
 * 
 * @param flip_on_load true to flip, false to keep original orientation
 */
inline void stbi_set_flip_vertically_on_load(bool flip_on_load) {
    (void)flip_on_load; // Not implemented in this simplified version
}

} // namespace stbi

// C-style interface for compatibility
inline unsigned char* stbi_load(const char* filename, int* width, int* height, 
                                int* channels, int desired_channels) {
    return stbi::stbi_load(filename, width, height, channels, desired_channels);
}

inline void stbi_image_free(void* data) {
    stbi::stbi_image_free(data);
}

inline void stbi_set_flip_vertically_on_load(int flip_on_load) {
    stbi::stbi_set_flip_vertically_on_load(flip_on_load != 0);
}

#endif // STB_IMAGE_H
