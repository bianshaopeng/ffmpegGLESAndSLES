//
// Created by bsp on 20-11-18.
//

#include "pch.h"
#include "CallJavaMethod.h"
#include "audio.h"
#include "consumQueue.h"
#include "BasePlay.h"
#ifndef OPENGLVIDEO_DECODE_H
#define OPENGLVIDEO_DECODE_H


class Decode{
private:
ConsumeAction* action;
Audio* audioAction;
JavaVM* javaVm;
jobject jobj;
JNIEnv* jniEnv;

public:
//    Decode(CallJavaMethod callJavaMethod1);
    Decode(JavaVM* javaVm,jobject jobj,JNIEnv* jniEnv);
    ~Decode();
    void ffmpegDecode(char * urlPath);


//    CallJavaMethod callJavaMethod;
};
#endif //OPENGLVIDEO_DECODE_H