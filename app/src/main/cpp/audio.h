//
// Created by bsp on 20-11-20.
//

#ifndef OPENGLVIDEO_AUDIO_H
#define OPENGLVIDEO_AUDIO_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <iostream>
#include <queue>
#include "pch.h"
#include "BasePlay.h"

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"OpenglVideo",FORMAT,##__VA_ARGS__);
using namespace std;
class Audio :public BasePlay{
public:
    pthread_mutex_t audioMutex;
    pthread_cond_t audioCond;
    pthread_t  audioId;
    queue<AVPacket*> audioQueue;
    SLObjectItf pEngine =NULL;//用SLObjectItf声明引擎接口对象
    SLEngineItf engineEngine = NULL;//声明具体的引擎对象
    SLObjectItf outputMixObject = NULL;//用SLObjectItf创建混音器接口对象
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;//具体的混音器对象实例
    SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;//默认情况


    SLObjectItf audioplayer=NULL;//用SLObjectItf声明播放器接口对象
    SLPlayItf  slPlayItf=NULL;//播放器接口
    SLAndroidSimpleBufferQueueItf  slBufferQueueItf=NULL;//缓冲区队列接口
    size_t buffersize =0;
    uint8_t *buffer;
    AVFrame* avFrame;
    bool isPlay;
    SwrContext* swrContext;

public:
    Audio();
    void init();
    void push(AVPacket * packet);
   //创建引擎
    void createEngine();
   //创建混音器
    void createMixVolume();
    //播放音频
    int getPcm(void **pcm,size_t *pcm_size);
    void playAudio();
    void getAudioFrame(AVFrame* frame);
//    void getQueueCallBack(SLAndroidSimpleBufferQueueItf  slBufferQueueItf, void* context);
};


#endif //OPENGLVIDEO_AUDIO_H
