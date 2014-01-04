#include "Camera.hpp"

void MR::Camera::Use(GLuint matrixUniform) {
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, &mvp[0][0]);
}
