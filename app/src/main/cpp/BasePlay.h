//
// Created by bsp on 20-11-21.
//

#ifndef OPENGLVIDEO_BASEPLAY_H
#define OPENGLVIDEO_BASEPLAY_H

#include "pch.h"

class BasePlay {
public:
    AVCodecContext* avCodecContext = nullptr;
    BasePlay();
    ~BasePlay();

};


#endif //OPENGLVIDEO_BASEPLAY_H
