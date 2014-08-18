/**
        TextureLoadFlags - SOIL FLAGS

        Note that if DDS_LoadDirect is used
        the rest of the flags with the exception of
        TextureRepeats will be ignored while
        loading already-compressed DDS files.

        PowerOfTwo: force the image to be POT
        MipMaps: generate mipmaps for the texture
        TextureRepeats: otherwise will clamp
        MultiplyAplha: for using (GL_ONE,GL_ONE_MINUS_SRC_ALPHA) blending
        InvertY: flip the image vertically
        CompressToDXT: if the card can display them, will convert RGB to DXT1, RGBA to DXT5
        DDS_LoadDirect: will load DDS files directly without _ANY_ additional processing
        NTSC_SafeRGB: clamps RGB components to the range [16,235]
        CoCg_Y: Google YCoCg; RGB=>CoYCg, RGBA=>CoCgAY
        TextureRectangle: uses ARB_texture_rectangle ; pixel indexed & no repeat or MIPmaps or cubemaps
**/

#pragma once

#ifndef _MR_TEXTURE_LOADER_H_
#define _MR_TEXTURE_LOADER_H_

#include "../Utils/Singleton.hpp"

#include <string>

namespace MR {

class TextureLoader : public Singleton<TextureLoader> {
public:

    enum TextureLoadFormat {
        LFO_Auto = 0,
        LFO_Luminous = 1, //(greyscale)
        LFO_LuminousAplha = 2, //Luminous with Alpha
        LFO_RGB = 3,
        LFO_RGBA = 4
    };

    enum TextureLoadFlags {
        LFL_None = 0,
        LFL_PowerOfTwo = 1,
        LFL_MipMaps = 2,
        LFL_TextureRepeats = 4,
        LFL_MultiplyAplha = 8,
        LFL_InvertY = 16,
        LFL_CompressToDXT = 32,
        LFL_DDS_LoadDirect = 64,
        LFL_NTSC_SafeRGB = 128,
        LFL_CoCg_Y = 256,
        LFL_TextureRectangle = 512
    };

    enum TextureHDRFake {
        HDR_RGBE = 0, //RGB * pow( 2.0, A - 128.0 )
        HDR_RGBdivA = 1, //RGB / A
        HDR_RGBdivA2 = 2 //RGB / (A*A)
    };

    /// Loads OpenGL texture from file to specific OpenGL texture id (if t is non zero) or create new GL texture

    unsigned int LoadGLTexture(const std::string& file, const unsigned int & t = 0, const TextureLoadFormat& = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    unsigned int LoadGLCubemap(const std::string& x_pos_file, const std::string& x_neg_file,
                               const std::string& y_pos_file, const std::string& y_neg_file,
                               const std::string& z_pos_file, const std::string& z_neg_file,
                               const unsigned int & t = 0,
                               const TextureLoadFormat& format = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    unsigned int LoadGLCubemap(const std::string& file, const char faceOrder[6], const unsigned int & t = 0, const TextureLoadFormat& = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    unsigned int LoadGLHDR(const std::string& file, const int& rescale_to_max, const unsigned int & t = 0, const TextureHDRFake& hdrFake = HDR_RGBE, const TextureLoadFlags& flags = LFL_MipMaps);

    /// Loads OpenGL texture from memory

    unsigned int LoadGLTexture(const unsigned char *const buffer, const unsigned int& buffer_length, const unsigned int & t = 0, const TextureLoadFormat& = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    unsigned int LoadGLCubemap(const unsigned char *const x_pos_buffer, const unsigned int& x_pos_buffer_length,
                               const unsigned char *const x_neg_buffer, const unsigned int& x_neg_buffer_length,
                               const unsigned char *const y_pos_buffer, const unsigned int& y_pos_buffer_length,
                               const unsigned char *const y_neg_buffer, const unsigned int& y_neg_buffer_length,
                               const unsigned char *const z_pos_buffer, const unsigned int& z_pos_buffer_length,
                               const unsigned char *const z_neg_buffer, const unsigned int& z_neg_buffer_length,
                               const unsigned int & t = 0,
                               const TextureLoadFormat& format = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    unsigned int LoadGLCubemap(const unsigned char *const buffer, const unsigned int& buffer_length, const char faceOrder[6], const unsigned int & t = 0, const TextureLoadFormat& format = LFO_Auto, const TextureLoadFlags& flags = LFL_MipMaps);

    /// Save screenshot from OpenGL window in RGB format
    enum class ImageSaveType : int {
        IMG_ST_TGA = 0,
        IMG_ST_BMP = 1,
        IMG_ST_DDS = 2
    };

    bool SaveScreenshot(const std::string& file, const ImageSaveType& image_type, const int& x, const int& y, const int& width, const int& height);

    /** Load raw image **/
    unsigned char* LoadImage(const std::string& file, int* width, int* height, TextureLoadFormat* format, const TextureLoadFormat& force_format);

    TextureLoader();
    virtual ~TextureLoader();
};

}

#endif // _MR_TEXTURE_LOADER_H_
