#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <raylib.h>
#include <GL/glut.h>

namespace AssetManager {

Texture arrowNormal;
Texture arrowExpanded;

Texture LoadTextureFix(const char* filePath) {
    Texture output;
    output = LoadTexture(filePath);
    glBindTexture(GL_TEXTURE_2D, output.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 33071);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 33071);
    glBindTexture(GL_TEXTURE_2D, 0);
    return output;
}

Image LoadImageFix(const char* filePath) {
    return LoadImageFromTexture(LoadTextureFix(filePath));
}

void LoadAssets() {
	arrowNormal = LoadTextureFix("engine/arrowNormal.png");
    arrowExpanded = LoadTextureFix("engine/arrowExpanded.png");
}

};
#endif
