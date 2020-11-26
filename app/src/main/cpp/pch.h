//
// Created by bsp on 20-5-12.
//

#include <jni.h>
#include <android/log.h>

#ifndef CAMERAPUSH_PCH_H
#define CAMERAPUSH_PCH_H
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);
#endif //CAMERAPUSH_PCH_H
extern "C"{
#include "../jniLibs/include/libavcodec/avcodec.h"
#include "../jniLibs/include/libavformat/avformat.h"
#include "../jniLibs/include/libavutil/avutil.h"
#include "../jniLibs/include/libavfilter/avfilter.h"
#include "../jniLibs/include/libswresample/swresample.h"
#include "../jniLibs/include/libswscale/swscale.h"
#include "../jniLibs/include/libavutil/imgutils.h"
};
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"OpenglVideo",FORMAT,##__VA_ARGS__);