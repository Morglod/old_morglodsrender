#include "Optimizations.h"

void MR::Optimization_SameGeometryBuffer::Draw(){
	//Bind data
	this->main_gb->vertexBuffer->Bind();
	for(unsigned int i = 0; i < this->main_gb->vertexBuffer->dataBuffers_num; ++i){
		this->main_gb->vertexBuffer->dataBuffers[i]->Bind();
	}

	if(this->material != NULL){
        glColor4f(this->material->AmbientColor[0], this->material->AmbientColor[1], this->material->AmbientColor[2], this->material->AmbientColor[3]);
        if(this->material->AmbientTexture != NULL) this->material->AmbientTexture->Bind(this->material->AmbientTextureStage);
        if(this->material->GlobalShader != NULL) this->material->GlobalShader->Use();
	}

	glPushMatrix();
	this->UseTransformations();

	//Bind indeces and draw, or simply draw
	if(this->main_gb->indexBuffer != NULL){
		this->main_gb->indexBuffer->Bind();

		for(std::vector<MR::Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
			glPushMatrix();
			(*it)->UseTransformations();

			glDrawElements(
				this->main_gb->gl_DRAW_MODE, //mode
				this->main_gb->indexBuffer->GetElementsNum(), //count
				GL_UNSIGNED_INT, //type
				0 //element array buffer offset
			);
			glPopMatrix();
		}
		this->main_gb->indexBuffer->Unbind();
	}
	else{
		for(std::vector<MR::Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
			glPushMatrix();
			(*it)->UseTransformations();
			glDrawArrays( this->main_gb->gl_DRAW_MODE, 0, this->main_gb->vertexBuffer->GetElementsNum() );
			glPopMatrix();
		}
	}

	glPopMatrix();

	//Unbind data
	for(unsigned int i = 0; i < this->main_gb->vertexBuffer->dataBuffers_num; ++i){
		this->main_gb->vertexBuffer->dataBuffers[i]->Unbind();
	}
	this->main_gb->vertexBuffer->Unbind();
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
