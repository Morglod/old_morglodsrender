/*#include "RenderTarget.h"

MR::RenderTarget::RenderTarget(int w, int h, int gl_format, int gl_data_type){
    this->Width = w;
    this->Height = h;

    glGenFramebuffers( 1, &this->framebuffer );

    glGenTextures( 1, &this->gl_TEXTURE );
    glBindTexture( GL_TEXTURE_2D, this->gl_TEXTURE );

    glTexImage2D(
        GL_TEXTURE_2D, 0, gl_format, w, h, 0, gl_format, gl_data_type, NULL
    );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gl_TEXTURE, 0
    );
}

MR::RenderTarget::~RenderTarget(){
    glDeleteTextures( 1, &this->gl_TEXTURE );
    glDeleteFramebuffers( 1, &this->framebuffer );
}

void MR::RenderTarget::Use(){
    glBindFramebuffer( GL_FRAMEBUFFER, this->framebuffer );
}
*/
