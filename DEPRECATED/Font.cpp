#include "Font.h"
#include <fstream>
#include <stdlib.h>

namespace MR {
void FontText::Draw(float x, float y, float rescale) {
	glColor4f(color_r, color_g, color_b, color_a);

	glPushMatrix();
	glTranslatef(-x, -y, 0);

	glPushMatrix();
    glScalef(rescale, -rescale, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, this->_gl_buffer);
    glVertexPointer(3, GL_FLOAT, sizeof(float)*5, 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));

    this->_font->GetCharset()->_texture->Bind(this->glTextureStage);
    glDrawArrays(GL_QUADS, 0, this->_verts_num);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopMatrix();
    glPopMatrix();
}

FontText::FontText(Font* _f, std::wstring text) : _verts_num( ((unsigned int)text.size())*4), _font(_f), _text(text) {
	const unsigned int varray_size = ((unsigned int)text.size())*20;
	float* varray = new float[varray_size];

	float CurX = 0;
	float CurY = 0;
	for(unsigned int i = 0; i < varray_size; ) {
		unsigned int current_char = i/20;
		CharDescr* cd;

		if(text[current_char] != '\n') cd = this->_font->GetCharset()->Get((int)text[current_char]);
		else cd = this->_font->GetCharset()->Get((int)(' '));

			//Vertex1
			//pos
			varray[i] = (float) CurX + cd->XOffset;
			varray[i+1] = (float) cd->YOffset + CurY;
			varray[i+2] = 0;
			//tex
			varray[i+3] = cd->TextureX1;
			varray[i+4] = cd->TextureY1;

			//Vertex2
			//pos
			varray[i+5] = (float) cd->Width + CurX + cd->XOffset;
			varray[i+6] = (float) cd->YOffset + CurY;
			varray[i+7] = 0;
			//tex
			varray[i+8] = cd->TextureX2;
			varray[i+9] = cd->TextureY1;

			//Vertex3
			//pos
			varray[i+10] = (float) cd->Width + CurX + cd->XOffset;
			varray[i+11] = (float) cd->Height + cd->YOffset + CurY;
			varray[i+12] = 0;
			//tex
			varray[i+13] = cd->TextureX2;
			varray[i+14] = cd->TextureY2;

			//Vertex4
			//pos
			varray[i+15] = (float) CurX + cd->XOffset;
			varray[i+16] = (float) cd->Height + cd->YOffset + CurY;
			varray[i+17] = 0;
			//tex
			varray[i+18] = cd->TextureX1;
			varray[i+19] = cd->TextureY2;

		if(text[current_char] == '\n'){
			CurX = 0;
			CurY += this->_font->GetCharset()->NewLineHight;
		}
		else CurX += cd->XAdvance;

		i += 20;
	}

	glGenBuffers(1, &this->_gl_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->_gl_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*varray_size, &varray[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Font::PrintLine(std::wstring text, float x, float y, float rescale, float color_r, float color_b, float color_g, float color_a, wchar_t maskChar) {
	this->_char_set->_texture->Bind(this->glTextureStage);
	glColor4f(color_r, color_b, color_g, color_a);

	glPushMatrix();
	glTranslatef(-x, y, 0);

	glPushMatrix();
	glScalef(rescale, -rescale, 0);

	float CurX = 0;
	float CurY = 0;
	for( unsigned int i = 0; i < text.size(); ++i ) {
		int ichar = (int)text[i];
		if(text[i] == '\n'){
			CurX = 0;
			CurY += this->GetCharset()->NewLineHight;
			continue;
		}

		if(maskChar != '\0') ichar = (int)maskChar;
		glBegin(GL_QUADS);
		glTexCoord2f( this->_char_set->Get(ichar)->TextureX1, this->_char_set->Get(ichar)->TextureY1 );
		glVertex2f( (float) CurX + this->_char_set->Get(ichar)->XOffset, (float) this->_char_set->Get(ichar)->YOffset + CurY);

		glTexCoord2f( this->_char_set->Get(ichar)->TextureX2, this->_char_set->Get(ichar)->TextureY1);
		glVertex2f((float) this->_char_set->Get(ichar)->Width + CurX + this->_char_set->Get(ichar)->XOffset, (float) this->_char_set->Get(ichar)->YOffset + CurY);

		glTexCoord2f( this->_char_set->Get(ichar)->TextureX2, this->_char_set->Get(ichar)->TextureY2);
		glVertex2f((float) this->_char_set->Get(ichar)->Width + CurX + this->_char_set->Get(ichar)->XOffset, (float) this->_char_set->Get(ichar)->Height + this->_char_set->Get(ichar)->YOffset + CurY);

		glTexCoord2f( this->_char_set->Get(ichar)->TextureX1, this->_char_set->Get(ichar)->TextureY2);
		glVertex2f((float) CurX + this->_char_set->Get(ichar)->XOffset, (float) this->_char_set->Get(ichar)->Height + this->_char_set->Get(ichar)->YOffset + CurY);
		glEnd();

		CurX += this->_char_set->Get(ichar)->XAdvance;
	}

	glPopMatrix();
	glPopMatrix();
}

float Font::CalcLineSize(std::wstring text, float rescale){
	float sizeX = 0.0f;
	for( unsigned int i = 0; i < text.size(); ++i ) {
		int ichar = (int)text[i];
		if(text[i] == '\n'){
			continue;
		}
		sizeX += (float)(this->_char_set->Get(ichar)->Width + this->_char_set->Get(ichar)->XOffset);
	}
	return sizeX*rescale;
}

bool Font::Load() {
    if(this->_manager->debugMessages) MR::Log::LogString("Font "+this->_name+" ("+this->_source+") loading");
    if(this->_source == "") {
        if(this->_manager->debugMessages)
            MR::Log::LogString("Font "+this->_name+" ("+this->_source+") load failed. Source is null");
        this->_loaded = false;
        return false;
    }

    //PARSING FILE
    //if(!IO::File::Exists( gcnew String(this->_source.c_str()) ) )
    //    if(this->_manager->debugMessages)
    //        MR::Log::LogString("Font "+this->_name+" ("+this->_source+") load failed. Source is not exist");
    std::ifstream fs(this->_source, std::ios::in);
    if(!fs){
        if(this->_manager->debugMessages) MR::Log::LogString("Font "+this->_name+" ("+this->_source+") load failed. Source is not exist");
        return false;
    }

    std::string fstring( (std::istreambuf_iterator<char>(fs)), (std::istreambuf_iterator<char>()) );

    //array<Byte>^ abuffer = System::IO::File::ReadAllBytes(gcnew String(this->_source.c_str()));
    //String^ file = System::Text::Encoding::Default->GetString(System::Text::Encoding::Convert(System::Text::Encoding::ASCII, System::Text::Encoding::Default, abuffer));

    //file = file->Remove(0, file->IndexOf("scaleW=")+7);
    fstring = fstring.erase(0, fstring.find_first_of("scaleW=")+7);
    unsigned short TextureSize = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
    //file = file->Remove(0, file->IndexOf("file=\"")+6);
    fstring = fstring.erase(0, fstring.find_first_of("file=\"")+6);

    std::string texture_file = fstring.substr(0, fstring.find_first_of('\"'));
    MR::Log::LogString("Font CharsetTexture(\""+texture_file+"\")");
    this->_char_set = new Charset(TextureSize, dynamic_cast<MR::Texture*>(MR::TextureManager::Instance()->Need( texture_file )) );
    /*std::string texture_file = (wchar_t*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(
		System::IO::Path::Combine(
		System::IO::Path::GetDirectoryName( gcnew System::String(this->_source.c_str()) ),
		file->Substring(0, file->IndexOf("\"")) )
	);*/
	//size_t pos = this->_source.find_last_of("\\/");
	//std::string texture_file = ((std::string::npos == pos) ? "" : this->_source.substr(0, pos))+fstring.substr(0, fstring.find_first_of("\""));

    //file = file->Remove(0, file->IndexOf("char id=")+8);
    fstring = fstring.erase(0, fstring.find_first_of("char id=")+8);

    //At first pass we have "char id=" from 0 index
    bool ok = true;
    while(ok) {
        unsigned short id = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("x=")+2);//file = file->Remove(0, file->IndexOf("x=")+2);
        unsigned short x = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("y=")+2);//file = file->Remove(0, file->IndexOf("y=")+2);
        unsigned short y = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("width=")+6);//file = file->Remove(0, file->IndexOf("width=")+6);
        unsigned short width = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("height=")+7);//file = file->Remove(0, file->IndexOf("height=")+7);
        unsigned short height = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("xoffset=")+8);//file = file->Remove(0, file->IndexOf("xoffset=")+8);
        short xoffset = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::Int16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("yoffset=")+8);//file = file->Remove(0, file->IndexOf("yoffset=")+8);
        short yoffset = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::Int16::Parse(file->Substring(0, file->IndexOf(" ")));
        fstring = fstring.erase(0, fstring.find_first_of("xadvance=")+9);//file = file->Remove(0, file->IndexOf("xadvance=")+9);
        unsigned short xadvance = (unsigned short)std::strtoul(fstring.substr(0, fstring.find_first_of(" ")).c_str(), NULL, 0);//System::UInt16::Parse(file->Substring(0, file->IndexOf(" ")));

        unsigned short page = 0;
        float x1 = (x/ (float)TextureSize);
        float x2 = ((x+width) / (float)TextureSize);
        float y1 = 1.0f-(y/ (float)TextureSize);
        float y2 = 1.0f-((y+height)/ (float)TextureSize);

        this->_char_set->Add(id, new CharDescr(x,y,width,height,xoffset,yoffset,xadvance,page, x1, x2, y1, y2) );
        if(fstring.find("char id=") == std::string::npos) ok = false;//if(file->Contains("char id=") == false) ok = false;
        else fstring = fstring.erase(0, fstring.find_first_of("char id=")+8);//else file = file->Remove(0, file->IndexOf("char id=")+8);
    }

    if(this->_char_set->_texture == nullptr) return false;
    this->_loaded = true;
    return true;
}
void Font::UnLoad() {
    if(this->_manager->debugMessages) MR::Log::LogString("Font "+this->_name+" ("+this->_source+") unloading");
    delete this->_char_set;
    this->_char_set = nullptr;
    this->_loaded = false;
}
bool Font::ReLoad() {
    if(this->_manager->debugMessages) MR::Log::LogString("Font "+this->_name+" ("+this->_source+") reloading");
    this->UnLoad();
    return this->Load();
}
Font::Font(MR::ResourceManager* manager,
     std::string name, std::string source) : Resource(manager, name, source), _name(name), _source(source) {
    _manager = dynamic_cast<MR::FontManager*>(manager);
}
Font::~Font() {
    if(this->_manager->debugMessages) MR::Log::LogString("Font "+this->_name+" ("+this->_source+") deleting");
    this->UnLoad();
}

Resource* FontManager::Create(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("FontManager "+name+" ("+source+") creating");
    Font * t = new Font(this, name, source);
    this->_resources.push_back(t);
    return t;
}

Resource* FontManager::CreateAndLoad(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("FontManager "+name+" ("+source+") creating and loading");
    Resource* t = this->Create(name, source);
    t->Load();
    return t;
}
Resource* FontManager::Get(std::string source) {
    if(this->debugMessages) MR::Log::LogString("FontManager ("+source+") getting");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++) {
        if( (*it)->GetSource() == source ) return *it;
    }
    return nullptr;
}
Resource* FontManager::Find(std::string name) {
    if(this->debugMessages) MR::Log::LogString("FontManager "+name+" searching");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++) {
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}
Resource* FontManager::Need(std::string source) {
    if(this->debugMessages) MR::Log::LogString("FontManager ("+source+") needed");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++) {
        if( (*it)->GetSource() == source ) {
            if( (*it)->IsLoaded() == false) (*it)->Load();
            return *it;
        }
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

void FontManager::Remove(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("FontManager "+name+" ("+source+") removing");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++) {
        if( (*it)->GetName() == name && (*it)->GetSource() == source) {
            delete *it;
            this->_resources.erase(it);
        }
    }
}

void FontManager::RemoveAll() {
    if(this->debugMessages) MR::Log::LogString("FontManager removing all");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++) {
        delete *it;
        this->_resources.erase(it);
        it--;
    }
}
}
