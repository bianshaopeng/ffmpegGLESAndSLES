#include <string>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <android/log.h>
#include <unistd.h>
#include <jni.h>
#include "consumQueue.h"
#include "decode.h"


JavaVM *jvm;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jvm = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}



bool isPlay = true;
extern "C"
JNIEXPORT void JNICALL
Java_com_csii_openglvideo_MainActivity_play(JNIEnv *env, jobject obj, jstring url) {
    const char *urlPath = env->GetStringUTFChars(url, NULL);
    /**
     * obj局部变量
     * jobj全局变量
     */
    jobject jobj = env->NewGlobalRef(obj);
    ConsumeAction consumeAction ();
    Decode decode(jvm,jobj,env) ;
    decode.ffmpegDecode(const_cast<char *>(urlPath));
}