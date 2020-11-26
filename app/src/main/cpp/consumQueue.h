//
// Created by bsp on 20-11-17.
//

#ifndef OPENGLVIDEO_CONSUMQUEUE_H
#define OPENGLVIDEO_CONSUMQUEUE_H



#include <pthread.h>
#include <queue>
#include <string.h>
#include <jni.h>
#include "pch.h"
#include "BasePlay.h"
#include "CallJavaMethod.h"
//extern pthread_mutex_t mutex;
//extern pthread_cond_t cond;
//extern pthread_t cusId;
//void *consumer(void *data);
using namespace std;
class ConsumeAction :public BasePlay{

public:
     pthread_mutex_t mutex;
     pthread_cond_t cond;
     pthread_t cusId;
    JavaVM* javaVm;
    JNIEnv* jniEnv;
    jobject jobj;
    bool flag;
    queue<AVPacket*> queueVideo;
    CallJavaMethod callJavaMethod;
public:
    ConsumeAction(CallJavaMethod callJavaMethod);

    void init();
//    void *consumer(void *data);
    void push(AVPacket* avPacket);


  //  void getAVCodecContext(AVCodecContext* avCodecContext);

};
#endif //OPENGLVIDEO_CONSUMQUEUE_H