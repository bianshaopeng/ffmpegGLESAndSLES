//
// Created by bsp on 20-11-19.
//

#include "CallJavaMethod.h"

CallJavaMethod::CallJavaMethod(JavaVM *vm, JNIEnv *env, jobject obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = obj;
    jclass jls = env->GetObjectClass(obj);
    jmethodId = env->GetMethodID(jls, "getCallBack", "(II[B[B[B)V");
}

void CallJavaMethod::callBackJava(int w, int h, jbyte* y, jbyte* u, jbyte* v) {
    javaVm->AttachCurrentThread(&jniEnv, 0);
    jbyteArray y1 = jniEnv->NewByteArray(w * h);
    jbyteArray u1 = jniEnv->NewByteArray(w * h);
    jbyteArray v1 = jniEnv->NewByteArray(w * h);
    jniEnv->SetByteArrayRegion(y1, 0, w * h, y);
    jniEnv->SetByteArrayRegion(u1, 0, w * h / 4, u);
    jniEnv->SetByteArrayRegion(v1, 0, w * h / 4, v);
    jniEnv->CallVoidMethod(jobj, jmethodId,w,h,y1,u1,v1);
    /**
    * 注意  env->DeleteGlobalRef()删除全局变量引用
    *      env->DeleteLocalRef()删除局部变量引用
    */
    jniEnv->DeleteLocalRef(y1);
    jniEnv->DeleteLocalRef(u1);
    jniEnv->DeleteLocalRef(v1);
    javaVm->DetachCurrentThread();

}
