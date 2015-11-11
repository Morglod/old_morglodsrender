#pragma once

mr::Texture2DPtr floor_texture, wall_texture;

void _Texture_Do_Load(std::string const& file, mr::Texture2DPtr& out_tex) {
    using namespace mr;

    auto textureData = TextureData::FromFile(file);

    TextureParams textureParams;
    textureParams.minFilter = TextureMinFilter::LinearMipmapLinear;
    textureParams.magFilter = TextureMagFilter::Linear;

    out_tex = Texture2D::Create(textureParams);
    out_tex->Storage();
    out_tex->WriteImage(textureData);
    out_tex->BuildMipmaps();
    out_tex->MakeResident();
}

void InitTexture() {
    _Texture_Do_Load("res_doom/floor.bmp", floor_texture);
    _Texture_Do_Load("res_doom/wall.bmp", wall_texture);
}
