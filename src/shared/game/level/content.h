#ifndef CONTENT_H
#define CONTENT_H

#include "plains.h"

namespace Game {
    extern Level plains;
    extern std::vector<Level> levels;
    extern Texture2D bulletTexture;

    void InitGameContent();

}

#endif