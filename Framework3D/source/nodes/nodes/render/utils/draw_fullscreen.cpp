#include "draw_fullscreen.h"

#include "pxr/imaging/garch/glApi.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
float vertices[] = {
    // positions
    -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  0.0f
};

void CreateFullScreenVAO(unsigned& VAO, unsigned& VBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void DestroyFullScreenVAO(unsigned& VAO, unsigned& VBO)
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
