#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

#include "Config.hpp"
#include "Resources/ResourceManager.hpp"
#include "Utils/Log.hpp"
#include "Types.hpp"
#include "Utils/Events.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

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
    class Camera;
    class IRenderSystem;

    class Texture;
    class ITextureArray;
    class ITextureSettings;
    class TextureManager;

    class RenderTarget;

    class ITextureSettings : public Copyable<ITextureSettings*> {
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
        MR::EventListener<ITextureSettings*, const float&> OnLodBiasChanged;
        MR::EventListener<ITextureSettings*, float*> OnBorderColorChanged; //as param used pointer to _border_color
        MR::EventListener<ITextureSettings*, const MinFilter&> OnMinFilterChanged;
        MR::EventListener<ITextureSettings*, const MagFilter&> OnMagFilterChanged;
        MR::EventListener<ITextureSettings*, const int&> OnMinLodChanged;
        MR::EventListener<ITextureSettings*, const int&> OnMaxLodChanged;
        MR::EventListener<ITextureSettings*, const Wrap&> OnWrapSChanged;
        MR::EventListener<ITextureSettings*, const Wrap&> OnWrapRChanged;
        MR::EventListener<ITextureSettings*, const Wrap&> OnWrapTChanged;
        MR::EventListener<ITextureSettings*, const CompareMode&> OnCompareModeChanged;
        MR::EventListener<ITextureSettings*, const CompareFunc&> OnCompareFuncChanged;

        virtual void SetLodBias(const float& v) = 0;
        virtual void SetBorderColor(float* rgba) = 0;
        virtual void SetBorderColor(const float& r, const float& g, const float& b, const float& a) = 0;
        virtual void SetMinFilter(const MinFilter& v) = 0;
        virtual void SetMagFilter(const MagFilter& v) = 0;
        virtual void SetMinLod(const int& v) = 0;
        virtual void SetMaxLod(const int& v) = 0;
        virtual void SetWrapS(const Wrap& v) = 0;
        virtual void SetWrapR(const Wrap& v) = 0;
        virtual void SetWrapT(const Wrap& v) = 0;
        virtual void SetCompareMode(const CompareMode& v) = 0;
        virtual void SetCompareFunc(const CompareFunc& v) = 0;

        virtual unsigned int GetGLSampler() = 0;
        virtual float GetLodBias() = 0;
        virtual float* GetBorderColor() = 0;
        virtual MinFilter GetMinFilter() = 0;
        virtual MagFilter GetMagFilter() = 0;
        virtual int GetMinLod() = 0;
        virtual int GetMaxLod() = 0;
        virtual Wrap GetWrapS() = 0;
        virtual Wrap GetWrapR() = 0;
        virtual Wrap GetWrapT() = 0;
        virtual CompareMode GetCompareMode() = 0;
        virtual CompareFunc GetCompareFunc() = 0;

        virtual ~ITextureSettings() {}
    };

    class TextureSettings : public IObject, public ITextureSettings {
    public:
        ITextureSettings* Copy() override;

        void SetLodBias(const float& v) override ;
        void SetBorderColor(float* rgba) override ;
        void SetBorderColor(const float& r, const float& g, const float& b, const float& a) override ;
        void SetMinFilter(const MinFilter& v) override ;
        void SetMagFilter(const MagFilter& v) override ;
        void SetMinLod(const int& v) override ;
        void SetMaxLod(const int& v) override ;
        void SetWrapS(const Wrap& v) override ;
        void SetWrapR(const Wrap& v) override ;
        void SetWrapT(const Wrap& v) override ;
        void SetCompareMode(const CompareMode& v) override ;
        void SetCompareFunc(const CompareFunc& v) override ;

        inline unsigned int GetGLSampler() override {return _sampler;}
        inline float GetLodBias() override {return _lod_bias;}
        inline float* GetBorderColor() override {return _border_color;}
        inline MinFilter GetMinFilter() override {return _min_filter;}
        inline MagFilter GetMagFilter() override {return _mag_filter;}
        inline int GetMinLod() override {return _min_lod;}
        inline int GetMaxLod() override {return _max_lod;}
        inline Wrap GetWrapS() override {return _wrap_s;}
        inline Wrap GetWrapR() override {return _wrap_r;}
        inline Wrap GetWrapT() override {return _wrap_t;}
        inline CompareMode GetCompareMode() override {return _compare_mode;}
        inline CompareFunc GetCompareFunc() override {return _compare_func;}

        TextureSettings();
        virtual ~TextureSettings();
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

    class ITexture {
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

        enum class Target : unsigned int {
            Base1D = 0x0DE0,
            Base2D = 0x0DE1,
            Base3D = 0x806F,
            Rectangle = 0x84F5,
            Buffer = 0x8C2A,
            CubeMap = 0x8513,
            Array1D = 0x8C18,
            Array2D = 0x8C1A,
            CubeMapArray = 0x9009,
            Multisample2D = 0x9100,
            Multisample2DArray = 0x9102
        };

        enum class CompressionMode : unsigned int {
            NoCompression = 0,
            Default,
            S3TC,
            ETC2
        };

        /* SENDER - Texture pointer */
        MR::EventListener<ITexture*, void*> OnInfoReset;

        /* mipMapLevel, internalFormat, width, height, format, type, data
         *  invokes from SetData method */
        MR::EventListener<ITexture*, const int&, const InternalFormat&, const int&, const int&, const Format&, const Type&, void*> OnDataChanged;

        /* mipMapLevel, xOffset, yOffset, width, height, format, type, data
         *  invokes form UpdateData method */
        MR::EventListener<ITexture*, const int&, const int&, const int&, const int&, const int&, const Format&, const Type&, void*> OnDataUpdated;

        MR::EventListener<ITexture*, ITextureSettings*> OnSettingsChanged;

        virtual ITextureSettings* GetSettings() = 0;
        virtual void SetSettings(ITextureSettings* ts) = 0;

        virtual unsigned short GetWidth() = 0;
        virtual unsigned short GetHeight() = 0;
        virtual unsigned short GetDepth() = 0;
        virtual InternalFormat GetInternalFormat() = 0;
        virtual unsigned int GetImageSize() = 0;
        virtual unsigned char GetRedBitsNum() = 0;
        virtual unsigned char GetGreenBitsNum() = 0;
        virtual unsigned char GetBlueBitsNum() = 0;
        virtual unsigned char GetDepthBitsNum() = 0;
        virtual unsigned char GetAlphaBitsNum() = 0;
        virtual bool IsCompressed() = 0;
        virtual unsigned int GetGPUHandle() = 0;
        virtual unsigned int GetTextureArrayIndex() = 0;
        virtual ITextureArray* GetTextureArray() = 0;
        virtual Target GetTarget() = 0;
        virtual void ResetInfo() = 0;
        virtual void GetData(const int& mipMapLevel, const Format& format, const Type& type, void* dstBuffer) = 0;
        virtual void SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data) = 0;
        virtual void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data) = 0;
        virtual ITexture::CompressionMode GetCompressionMode() = 0;

        virtual ~ITexture() {}
    };

    class Texture : public IObject, public virtual ITexture, public virtual Resource {
    public:
        inline ITextureSettings* GetSettings() override { return _settings; } //May be nullptr
        inline unsigned short GetWidth() override { return gl_width; }
        inline unsigned short GetHeight() override { return gl_height; }
        inline unsigned short GetDepth() override { return gl_depth; }
        inline InternalFormat GetInternalFormat() override { return (InternalFormat)gl_internal_format; }
        inline unsigned int GetImageSize() override { return gl_mem_image_size; }
        inline unsigned char GetRedBitsNum() override { return gl_red_bits_num; }
        inline unsigned char GetGreenBitsNum() override { return gl_green_bits_num; }
        inline unsigned char GetBlueBitsNum() override { return gl_blue_bits_num; }
        inline unsigned char GetDepthBitsNum() override { return gl_depth_bits_num; }
        inline unsigned char GetAlphaBitsNum() override { return gl_alpha_bits_num; }
        inline bool IsCompressed() override { return gl_compressed; }
        inline unsigned int GetGPUHandle() override { return this->gl_texture; }
        inline Target GetTarget() override { return _target; }
        inline ITexture::CompressionMode GetCompressionMode() { return _compression_mode; }

        inline unsigned int GetTextureArrayIndex() override { return this->_textureArrayIndex; }
        inline ITextureArray* GetTextureArray() override { return this->_texArray; }
        inline bool InTextureArray() { return _inTextureArray; }

        inline void SetSettings(ITextureSettings* ts) {
            if(_settings != ts){
                _settings = ts;
                OnSettingsChanged(this, ts);
            }
        }

        void ResetInfo(); //Get fresh info about texture

        void GetData(const int& mipMapLevel, const Format& format, const Type& type, void* dstBuffer);
        void SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data);
        void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data);

        Texture(MR::ResourceManager* m, const std::string& name, const std::string& source);
        virtual ~Texture();

        static Texture* FromFile(MR::ResourceManager* m, const std::string& file, std::string name = "Auto");
        static void CreateOpenGLTexture(unsigned int* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type);
        static void CreateOpenGLTexture(const unsigned char *const data, const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);
        static void CreateOpenGLTexture(const unsigned char *const data, const char face_order[6], const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const GLTextureLoadFlags& flags = GLTextureLoadFlags::MipMaps);

    protected:

        unsigned short gl_width = 0, gl_height = 0, gl_depth = 0;
        unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
        unsigned int gl_mem_image_size = 0, gl_internal_format = 0;
        bool gl_compressed = false;
        unsigned int gl_texture; //OpenGL texture
        ITextureSettings* _settings = nullptr;

        ITexture::CompressionMode _compression_mode;

        Target _target = Target::Base2D;

        bool _inTextureArray = false;
        unsigned int _textureArrayIndex = 0;
        ITextureArray* _texArray = nullptr;

        bool _Loading() override;
        void _UnLoading() override;

        unsigned char* _async_data = 0;
        int _async_iform = 0;
        int _async_form = 0;

    private:
        friend class IRenderSystem;
        friend class ITextureArray;
        friend class TextureManager;
        Texture(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned int & glTexture);
    };

    class CubeMap : public virtual Texture {
    public:
        MR::EventListener<CubeMap*, const glm::vec3&> OnCapturePointChanged;

        void SetCapturePoint(const glm::vec3& p);
        inline glm::vec3 GetCapturePoint();

        void Capture(IRenderSystem* context);

        MR::RenderTarget* GetRenderTarget();

        CubeMap(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned short & Width, const unsigned short & Height);
        virtual ~CubeMap();
    protected:
        glm::vec3 _world_point_pos;
        MR::Camera* _cam;
        MR::RenderTarget* _rtarget;
    };

    class ITextureArray {
    public:
        virtual unsigned int GetGPUHandle() = 0;
        virtual Texture::InternalFormat GetInternalFormat() = 0;
        virtual Texture::Format GetFormat() = 0;
        virtual Texture::Type GetType() = 0;
        virtual unsigned short GetWidth() = 0;
        virtual unsigned short GetHeight() = 0;
        virtual unsigned short GetDepth() = 0;
        virtual int GetTexturesNum() = 0;

        virtual unsigned int Add(void* newData) = 0; //returns texture index in array
        virtual void Update(const unsigned int& textureIndex, void* newData) = 0;

        virtual bool CanStore(const ITexture::InternalFormat& iformat, const ITexture::Format& format, const ITexture::Type& type, const unsigned short& width, const unsigned short& height) = 0;
    };

    class TextureArray : public IObject, public ITextureArray {
        friend class Texture;
        friend class TextureManager;
    public:
        inline unsigned int GetGPUHandle() override {return _gl_texture;}
        inline Texture::InternalFormat GetInternalFormat() override {return _internal_format;}
        inline Texture::Format GetFormat() override {return _format;}
        inline Texture::Type GetType() override {return _type;}
        inline unsigned short GetWidth() override {return _width;}
        inline unsigned short GetHeight() override {return _height;}
        inline unsigned short GetDepth() override {return _depth;}
        inline int GetTexturesNum() override {return _storedTexNum;}

        /*  Should be with current TextureArray.GetWidth/Height/Format/Type/InternalFormat
            textureIndex - index of texture in array */

        virtual unsigned int Add(void* newData) override; //returns texture index in array
        virtual void Update(const unsigned int& textureIndex, void* newData) override;

        inline bool CanStore(const ITexture::InternalFormat& iformat, const ITexture::Format& format, const ITexture::Type& type, const unsigned short& width, const unsigned short& height) override {
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
        inline ITexture* WhiteTexture();

        inline void SetCompressionMode(const ITexture::CompressionMode& cm) { _defaultCompressionMode = cm; }
        inline ITexture::CompressionMode GetCompressionMode() { return _defaultCompressionMode; }

        TextureManager();
        virtual ~TextureManager();

        static TextureManager* Instance();
        static void DestroyInstance();
    protected:
        MR::ITexture* _white;

        bool _useTexArrays = false; //experimental
        unsigned int _maxTexturesInArray = 50;
        std::vector<TextureArray*> _tex_arrays;

        ITexture::CompressionMode _defaultCompressionMode;

        TextureArray* _StoreTexture(unsigned int* texArrayIndex, void* data, const ITexture::InternalFormat& iformat, const ITexture::Format& format, const ITexture::Type& type, const unsigned short& width, const unsigned short& height);
    };
}

glm::vec3 MR::CubeMap::GetCapturePoint(){
    return _world_point_pos;
}

MR::ITexture* MR::TextureManager::WhiteTexture(){
    return dynamic_cast<ITexture*>(_white);
}

#endif // _MR_TEXTURE_H_
