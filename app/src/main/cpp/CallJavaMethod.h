//
// Created by bsp on 20-11-19.
//
#include <jni.h>
#ifndef OPENGLVIDEO_CALLJAVAMETHOD_H
#define OPENGLVIDEO_CALLJAVAMETHOD_H


class CallJavaMethod {
private:
     JavaVM* javaVm;
     JNIEnv* jniEnv;
     jmethodID jmethodId;
     jobject jobj;
public:
    CallJavaMethod(JavaVM* javaVm,JNIEnv* jniEnv,jobject jobj);
    void callBackJava(int w,int h,jbyte* y,jbyte* u,jbyte* v);

};


#endif //OPENGLVIDEO_CALLJAVAMETHOD_H
