#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <raylib.h>

#if defined(_WIN32)
    #define NOGDICAPMASKS
    #define NOVIRTUALKEYCODES
    #define NOWINMESSAGES
    #define NOWINSTYLES
    #define NOSYSMETRICS
    #define NOMENUS
    #define NOICONS
    #define NOKEYSTATES
    #define NOSYSCOMMANDS
    #define NORASTEROPS
    #define NOSHOWWINDOW
    #define OEMRESOURCE
    #define NOATOM
    #define NOCLIPBOARD
    #define NOCOLOR
    #define NOCTLMGR
    #define NODRAWTEXT
    #define NOGDI
    #define NOKERNEL
    #define NOUSER
    #define NONLS
    #define NOMB
    #define NOMEMMGR
    #define NOMETAFILE
    #define NOMSG
    #define NOOPENFILE
    #define NOSCROLL
    #define NOSERVICE
    #define NOSOUND
    #define NOTEXTMETRIC
    #define NOWH
    #define NOWINOFFSETS
    #define NOCOMM
    #define NOKANJI
    #define NOHELP
    #define NOPROFILER
    #define NODEFERWINDOWPOS
    #define NOMCX
    #define _INC_MMSYSTEM // Disable winmm.lib (PlaySound)

    typedef struct tagMSG *LPMSG;

    #undef near
    #undef far
#endif

#include <GL/gl.h>

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
