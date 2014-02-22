#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

#include "Config.hpp"
#include "ResourceManager.hpp"
#include "Log.hpp"
#include "Types.hpp"
#include "Events.hpp"

namespace MR{
    enum class GLTextureLoadFormat : int {
        Auto = 0,
        Luminous = 1, //(greyscale)
        LuminousAplha = 2, //Luminous with Alpha
        RGB = 3,
        RGBA = 4
    };

    /**
        SOIL FLAGS

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
    enum class GLTextureLoadFlags : unsigned int {
        None = 0,
        PowerOfTwo = 1,
        MipMaps = 2,
        TextureRepeats = 4,
        MultiplyAplha = 8,
        InvertY = 16,
        CompressToDXT = 32,
        DDS_LoadDirect = 64,
        NTSC_SafeRGB = 128,
        CoCg_Y = 256,
        TextureRectangle = 512
    };

    enum class GLTextureHDRFake : int {
        HDR_RGBE = 0, //RGB * pow( 2.0, A - 128.0 )
        HDR_RGBdivA = 1, //RGB / A
        HDR_RGBdivA2 = 2 //RGB / (A*A)
    };

    /** Loads OpenGL texture from file to specific OpenGL texture id (if t is non zero) or create new GL texture **/

    unsigned int LoadGLTexture(const std::string& file, const unsigned int & t = 0, const GLTextureLoadFormat& = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    unsigned int LoadGLCubemap(const std::string& x_pos_file, const std::string& x_neg_file,
                               const std::string& y_pos_file, const std::string& y_neg_file,
                               const std::string& z_pos_file, const std::string& z_neg_file,
                               const unsigned int & t = 0,
                               const GLTextureLoadFormat& format = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    unsigned int LoadGLCubemap(const std::string& file, const char faceOrder[6], const unsigned int & t = 0, const GLTextureLoadFormat& format = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    unsigned int LoadGLHDR(const std::string& file, const int& rescale_to_max, const unsigned int & t = 0, const GLTextureHDRFake& hdrFake = GLTextureHDRFake::HDR_RGBE, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    /** Loads OpenGL texture from memory **/

    unsigned int LoadGLTexture(const unsigned char *const buffer, const unsigned int& buffer_length, const unsigned int & t = 0, const GLTextureLoadFormat& = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    unsigned int LoadGLCubemap(const unsigned char *const x_pos_buffer, const unsigned int& x_pos_buffer_length,
                               const unsigned char *const x_neg_buffer, const unsigned int& x_neg_buffer_length,
                               const unsigned char *const y_pos_buffer, const unsigned int& y_pos_buffer_length,
                               const unsigned char *const y_neg_buffer, const unsigned int& y_neg_buffer_length,
                               const unsigned char *const z_pos_buffer, const unsigned int& z_pos_buffer_length,
                               const unsigned char *const z_neg_buffer, const unsigned int& z_neg_buffer_length,
                               const unsigned int & t = 0,
                               const GLTextureLoadFormat& format = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    unsigned int LoadGLCubemap(const unsigned char *const buffer, const unsigned int& buffer_length, const char faceOrder[6], const unsigned int & t = 0, const GLTextureLoadFormat& format = GLTextureLoadFormat::Auto, const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    /** Save screenshot from OpenGL window in RGB format **/
    enum class GLImageSaveType : int {
        TGA = 0,
        BMP = 1,
        DDS = 2
    };

    bool SaveScreenshot(const std::string& file, const GLImageSaveType& image_type, const int& x, const int& y, const int& width, const int& height);

    /** Load raw image **/
    unsigned char* LoadImage(const std::string& file, int* width, int* height, GLTextureLoadFormat* format, const GLTextureLoadFormat& force_format);

    /** **/

    class RenderContext;

    class Texture;
    class TextureArray;
    class TextureSettings;
    class TextureManager;

    class _TS_WithPtr : public WithPtr<TextureSettings> {};

    class TextureSettings : public _TS_WithPtr, Copyable<_TS_WithPtr::Ptr> {
    public:
        enum class MinFilter : int {
            NEAREST_MIPMAP_NEAREST = 0x2700,
            LINEAR_MIPMAP_NEAREST = 0x2701,
            NEAREST_MIPMAP_LINEAR = 0x2702,
            LINEAR_MIPMAP_LINEAR = 0x2703,
            NEAREST = 0x2600,
            LINEAR = 0x2601
        };

        enum class MagFilter : int {
            NEAREST = 0x2600,
            LINEAR = 0x2601
        };

        enum class Wrap : int {
            CLAMP = 0x812F,
            REPEAT = 0x2901,
            MIRRORED_REPEAT = 0x8370,
            DECAL = 0x2101
        };

        enum class CompareMode : int {
            COMPARE_REF_TO_TEXTURE = 0x884E,
            NONE = 0
        };

        enum class CompareFunc : int {
            LEQUAL = 0x0203,
            GEQUAL = 0x0206,
            LESS = 0x0201,
            GREATHER = 0x0204,
            EQUAL = 0x0202,
            NOTEQUAL = 0x0205,
            ALWAYS = 0x0207,
            NEVER = 0x0200
        };

        /* SENDER - TextureSettings pointer */
        MR::Event<const float&> OnLodBiasChanged;
        MR::Event<float*> OnBorderColorChanged; //as param used pointer to _border_color
        MR::Event<const MinFilter&> OnMinFilterChanged;
        MR::Event<const MagFilter&> OnMagFilterChanged;
        MR::Event<const int&> OnMinLodChanged;
        MR::Event<const int&> OnMaxLodChanged;
        MR::Event<const Wrap&> OnWrapSChanged;
        MR::Event<const Wrap&> OnWrapRChanged;
        MR::Event<const Wrap&> OnWrapTChanged;
        MR::Event<const CompareMode&> OnCompareModeChanged;
        MR::Event<const CompareFunc&> OnCompareFuncChanged;

        TextureSettings::Ptr Copy();

        void SetLodBias(const float& v);
        void SetBorderColor(float* rgba);
        void SetBorderColor(const float& r, const float& g, const float& b, const float& a);
        void SetMinFilter(const MinFilter& v);
        void SetMagFilter(const MagFilter& v);
        void SetMinLod(const int& v);
        void SetMaxLod(const int& v);
        void SetWrapS(const Wrap& v);
        void SetWrapR(const Wrap& v);
        void SetWrapT(const Wrap& v);
        void SetCompareMode(const CompareMode& v);
        void SetCompareFunc(const CompareFunc& v);

        inline unsigned int GetGLSampler(){return _sampler;}
        inline float GetLodBias(){return _lod_bias;}
        inline float* GetBorderColor(){return _border_color;}
        inline MinFilter GetMinFilter(){return _min_filter;}
        inline MagFilter GetMagFilter(){return _mag_filter;}
        inline int GetMinLod(){return _min_lod;}
        inline int GetMaxLod(){return _max_lod;}
        inline Wrap GetWrapS(){return _wrap_s;}
        inline Wrap GetWrapR(){return _wrap_r;}
        inline Wrap GetWrapT(){return _wrap_t;}
        inline CompareMode GetCompareMode(){return _compare_mode;}
        inline CompareFunc GetCompareFunc(){return _compare_func;}

        TextureSettings();
        virtual ~TextureSettings();

        inline static TextureSettings::Ptr Create(){ return TextureSettings::Ptr(new TextureSettings()); }
    protected:
        unsigned int _sampler;
        float _lod_bias;
        float _border_color[4];
        MinFilter _min_filter;
        MagFilter _mag_filter;
        int _min_lod, _max_lod;
        Wrap _wrap_s, _wrap_r, _wrap_t;
        CompareMode _compare_mode;
        CompareFunc _compare_func;
    };

    class Texture : public virtual Resource {
    public:
        enum class InternalFormat : int {
            ALPHA = 0x1906,
            ALPHA4 = 0x803B,
            ALPHA8 = 0x803C,
            ALPHA12 = 0x803D,
            ALPHA16 = 0x803E,
            LUMINANCE = 0x1909,
            LUMINANCE4 = 0x803F,
            LUMINANCE8 = 0x8040,
            LUMINANCE12 = 0x8041,
            LUMINANCE16 = 0x8042,
            LUMINANCE_ALPHA = 0x190A,
            LUMINANCE4_ALPHA4 = 0x8043,
            LUMINANCE6_ALPHA2 = 0x8044,
            LUMINANCE8_ALPHA8 = 0x8045,
            LUMINANCE12_ALPHA4 = 0x8046,
            LUMINANCE12_ALPHA12 = 0x8047,
            LUMINANCE16_ALPHA16 = 0x8048,
            INTENSITY = 0x8049,
            INTENSITY4 = 0x804A,
            INTENSITY8 = 0x804B,
            INTENSITY12 = 0x804C,
            INTENSITY16 = 0x804D,
            R3_G3_B2 = 0x2A10,
            RGB = 0x1907,
            RGB4 = 0x804F,
            RGB5 = 0x8050,
            RGB8 = 0x8051,
            RGB10 = 0x8052,
            RGB12 = 0x8053,
            RGB16 = 0x8054,
            RGBA = 0x1908,
            RGBA2 = 0x8055,
            RGBA4 = 0x8056,
            RGB5_A1 = 0x8057,
            RGBA8 = 0x8058,
            RGB10_A2 = 0x8059,
            RGBA12 = 0x805A,
            RGBA16 = 0x805B
        };

        enum class Format : int {
            COLOR_INDEX = 0x1900,
            RED = 0x1903,
            GREEN = 0x1904,
            BLUE = 0x1905,
            ALPHA = 0x1906,
            RGB = 0x1907,
            RGBA = 0x1908,
            BGR_EXT = 0x80E0,
            BGRA_EXT = 0x80E1,
            LUMINANCE = 0x1909,
            LUMINANCE_ALPHA = 0x190A
        };

        enum class Type : int {
            UNSIGNED_BYTE = 0x1401,
            BYTE = 0x1400,
            UNSIGNED_SHORT = 0x1403,
            SHORT = 0x1402,
            UNSIGNED_INT = 0x1405,
            INT = 0x1404,
            FLOAT = 0x1406,
            UNSIGNED_BYTE_3_3_2 = 0x8032,
            UNSIGNED_BYTE_2_3_3_REV = 0x8362,
            UNSIGNED_SHORT_5_6_5 = 0x8363,
            UNSIGNED_SHORT_5_6_5_REV = 0x8364,
            UNSIGNED_SHORT_4_4_4_4 = 0x8033,
            UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
            UNSIGNED_SHORT_5_5_5_1 = 0x8034,
            UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
            UNSIGNED_INT_8_8_8_8 = 0x8035,
            UNSIGNED_INT_8_8_8_8_REV = 0x8367,
            UNSIGNED_INT_10_10_10_2 = 0x8036,
            UNSIGNED_INT_2_10_10_10_REV = 0x8368
        };

        /* SENDER - Texture pointer */
        MR::Event<void*> OnInfoReset;

        /* mipMapLevel, internalFormat, width, height, format, type, data
         *  invokes from SetData method */
        MR::Event<const int&, const InternalFormat&, const int&, const int&, const Format&, const Type&, void*> OnDataChanged;

        /* mipMapLevel, xOffset, yOffset, width, height, format, type, data
         *  invokes form UpdateData method */
        MR::Event<const int&, const int&, const int&, const int&, const int&, const Format&, const Type&, void*> OnDataUpdated;

        MR::Event<TextureSettings::Ptr> OnSettingsChanged;

        inline TextureSettings::Ptr GetSettings() const { return _settings; } //May be nullptr
        inline unsigned short GetWidth() const { return gl_width; }
        inline unsigned short GetHeight() const { return gl_height; }
        inline unsigned short GetDepth() const { return gl_depth; }
        inline InternalFormat GetInternalFormat() const { return (InternalFormat)gl_internal_format; }
        inline unsigned int GetImageSize() const { return gl_mem_image_size; }
        inline unsigned int GetCompressedImageSize() const { return gl_mem_compressed_img_size; }
        inline unsigned char GetRedBitsNum() const { return gl_red_bits_num; }
        inline unsigned char GetGreenBitsNum() const { return gl_green_bits_num; }
        inline unsigned char GetBlueBitsNum() const { return gl_blue_bits_num; }
        inline unsigned char GetDepthBitsNum() const { return gl_depth_bits_num; }
        inline unsigned char GetAlphaBitsNum() const { return gl_alpha_bits_num; }
        inline bool IsCompressed() const { return gl_compressed; }
        inline unsigned int GetGLTexture() const { return this->gl_texture; }

        inline unsigned int GetTextureArrayIndex() const { return this->_textureArrayIndex; }
        inline TextureArray* GetTextureArray() const { return this->_texArray; }

        inline void SetSettings(TextureSettings::Ptr ts) {
            if(_settings != ts){
                _settings = ts;
                OnSettingsChanged(this, ts);
            }
        }

        void ResetInfo(); //Get fresh info about texture

        void GetData(const int& mipMapLevel, const Format& format, const Type& type, void* dstBuffer);
        void SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data);
        void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data);

        virtual bool Load();
        virtual void UnLoad();

        Texture(MR::ResourceManager* m, const std::string& name, const std::string& source);
        virtual ~Texture();

        static Texture* FromFile(MR::ResourceManager* m, const std::string& file, std::string name = "Auto");
        static void CreateOpenGLTexture(unsigned int* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type);
        static void CreateOpenGLTexture(const unsigned char *const data, const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);
        static void CreateOpenGLTexture(const unsigned char *const data, const char face_order[6], const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);
    protected:
        unsigned short gl_width = 0, gl_height = 0, gl_depth = 0;
        unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
        unsigned int gl_mem_image_size = 0, gl_mem_compressed_img_size = 0, gl_internal_format = 0;
        bool gl_compressed = false;
        unsigned int gl_texture; //OpenGL texture
        TextureSettings::Ptr _settings;

        bool _inTextureArray = false;
        unsigned int _textureArrayIndex = 0;
        TextureArray* _texArray = nullptr;
    private:
        friend class RenderContext;
        friend class TextureArray;
        friend class TextureManager;
        Texture(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned int & glTexture);
    };

    class TextureArray {
        friend class Texture;
        friend class TextureManager;
    public:
        inline unsigned int GetGLTexture(){return _gl_texture;}
        inline Texture::InternalFormat GetInternalFormat(){return _internal_format;}
        inline Texture::Format GetFormat(){return _format;}
        inline Texture::Type GetType(){return _type;}
        inline unsigned short GetWidth(){return _width;}
        inline unsigned short GetHeight(){return _height;}
        inline unsigned short GetDepth(){return _depth;}
        inline int GetTexturesNum(){return _storedTexNum;}

        /*  Should be with current TextureArray.GetWidth/Height/Format/Type/InternalFormat
            textureIndex - index of texture in array */

        virtual unsigned int Add(void* newData); //returns texture index in array
        virtual void Update(const unsigned int& textureIndex, void* newData);

        inline bool CanStore(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height){
            return ((iformat == _internal_format)&&(format == _format)&&(type == _type)&&(width == _width)&&(height == _height)&&(_storedTexNum < _maxTextures));
        }

        TextureArray(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height, const unsigned int& maxTextures);
        virtual ~TextureArray();

        static bool Support(const Texture::InternalFormat& iformat);
    protected:
        unsigned int _storedTexNum = 0;
        unsigned int _maxTextures;
        unsigned int _gl_texture;
        Texture::InternalFormat _internal_format;
        Texture::Format _format;
        Texture::Type _type;
        unsigned short _width, _height, _depth;
    };

    class TextureManager : public virtual MR::ResourceManager {
        friend class Texture;
    public:
        virtual Resource* Create(const std::string& name, const std::string& source);
        virtual Resource* Create(const std::string& name, const unsigned int & gl_texture);
        inline Texture* NeedTexture(const std::string& source) { return dynamic_cast<Texture*>(Need(source)); }

        TextureManager() : ResourceManager() {}
        virtual ~TextureManager();

        static TextureManager* Instance(){
            static TextureManager* m = new TextureManager();
            return m;
        }
    protected:
        bool _useTexArrays = false; //experimental
        unsigned int _maxTexturesInArray = 50;
        std::vector<TextureArray*> _tex_arrays;

        TextureArray* _StoreTexture(unsigned int* texArrayIndex, void* data, const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height);
    };
}

#endif // _MR_TEXTURE_H_
