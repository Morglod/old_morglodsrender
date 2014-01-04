#pragma once

#include "ResourceManager.h"
#include "Texture.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

namespace MR {
class Font;
class FontManager;

struct CharDescr {
public:
    const unsigned short x, y;
    const unsigned short Width, Height;
    const short XOffset, YOffset;
    const unsigned short XAdvance;
    const unsigned short Page;

    const float TextureX1, TextureX2;
    const float TextureY1, TextureY2;

    CharDescr() : x( 0 ), y( 0 ), Width( 0 ), Height( 0 ), XOffset( 0 ), YOffset( 0 ),
        XAdvance( 0 ), Page( 0 ), TextureX1(0), TextureX2(0), TextureY1(0), TextureY2(0) {}
    CharDescr(unsigned short _x, unsigned short _y, unsigned short _w, unsigned short _h, short _xo, short _yo, unsigned short _xa, unsigned short _p, float _tx1, float _tx2, float _ty1, float _ty2) :
        x( _x ), y( _y ), Width( _w ), Height( _h ), XOffset( _xo ), YOffset( _yo ),
        XAdvance( _xa ), Page( _p ), TextureX1(_tx1), TextureX2(_tx2), TextureY1(_ty1), TextureY2(_ty2) {}
};

struct Charset {
public:
    const unsigned short TextureSize;
    unsigned int BadCharCodeReplace;
    MR::Texture* _texture;
    const Font* font;

    std::vector<unsigned int> char_codes;
    std::vector<CharDescr*> char_desrcs;

	float NewLineHight;

    inline CharDescr* Get(unsigned int i) {
        auto it = std::find(char_codes.begin(), char_codes.end(), i);
        if(it == char_codes.end()) return this->Get(BadCharCodeReplace);
        else return char_desrcs[std::distance(char_codes.begin(), it)];
    }

    inline void Add(unsigned int code, CharDescr* cd) {
		if(cd->Height > this->NewLineHight) this->NewLineHight = cd->Height;
        char_codes.push_back(code);
        char_desrcs.push_back(cd);
    }

    //CharDescr* Chars[256];
    Charset(unsigned short _ts, MR::Texture* _tx) : TextureSize(_ts), _texture(_tx), BadCharCodeReplace('?'), NewLineHight(0.0f) {}
};

class FontText {
protected:
    const unsigned int _verts_num;
    GLuint _gl_buffer = 0;
    Font* _font;
    std::wstring _text;
public:
	float color_r = 0.0f, color_b = 0.0f, color_g = 0.0f, color_a = 1.0f;

    GLenum glTextureStage = GL_TEXTURE0;

    inline std::wstring GetText() {
        return this->_text;
    }

    void Draw(float x, float y, float rescale = 1);

    FontText(Font* _f, std::wstring text);
};

class Font : public virtual Resource {
protected:
    std::string _name;
    std::string _source;
    bool _loaded = false;
    FontManager* _manager;
    Charset* _char_set = nullptr;
public:
    GLenum glTextureStage = GL_TEXTURE0;

    virtual inline std::string GetName() {
        return this->_name;
    }

    virtual inline std::string GetSource() {
        return this->_source;
    }

    inline Charset* GetCharset() {
        return this->_char_set;
    }

    virtual void PrintLine(std::wstring text, float x, float y, float rescale = 1.0f, float color_r = 0.0f, float color_b = 0.0f, float color_g = 0.0f, float color_a = 1.0f, wchar_t maskChar = '\0');
	virtual float CalcLineSize(std::wstring text, float rescale = 1.0f);

    bool Load();
    virtual void UnLoad();
    virtual bool ReLoad();

    virtual inline bool IsLoaded() {
        return this->_loaded;
    }

    virtual inline ResourceManager* GetManager() {
        return reinterpret_cast<ResourceManager*>(this->_manager);
    }

    Font(MR::ResourceManager* manager, std::string name, std::string source);
    virtual ~Font();
};

class FontManager : public virtual ResourceManager {
protected:
    std::vector<Font*> _resources;
public:
    bool debugMessages;

    virtual inline void Add(Resource* res) {
        this->_resources.push_back( dynamic_cast<Font*>(res) );
    }
    virtual inline void Remove(Resource* res) {
        auto it = std::find(this->_resources.begin(), this->_resources.end(), res);
        if(it != this->_resources.end()) {
            delete (*it);
            this->_resources.erase(it);
        }
    }

    virtual Resource* Create(std::string name, std::string source);
    virtual Resource* CreateAndLoad(std::string name, std::string source);
    virtual Resource* Get(std::string source);
    virtual Resource* Find(std::string name);
    virtual Resource* Need(std::string source);
    virtual Resource** Need(std::string* sources, const unsigned int num){return nullptr;}
    virtual void Remove(std::string name, std::string source);

    virtual void RemoveAll();

    FontManager() : debugMessages(false) {}
    virtual ~FontManager() {}

    static FontManager* Instance() {
        static FontManager* m = new FontManager();
        return m;
    }
};
}
