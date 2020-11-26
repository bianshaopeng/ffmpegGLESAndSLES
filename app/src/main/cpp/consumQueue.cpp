#include <unistd.h>
#include "consumQueue.h"



void *consumer(void *data) {
    ConsumeAction *consumeAct = static_cast<ConsumeAction*>(data) ;

    consumeAct->flag = true;
    while (consumeAct->flag) {

        int ret = 0;
        if (consumeAct->queueVideo.size() > 0) {
            pthread_mutex_lock(&consumeAct->mutex);
              AVPacket* avPacket = consumeAct->queueVideo.front();
               ret = avcodec_send_packet(consumeAct->avCodecContext,avPacket);

               if (ret>=0){
                   AVFrame* avFrame = av_frame_alloc();
                   ret = avcodec_receive_frame(consumeAct->avCodecContext,avFrame);
                   if (ret>=0){
                       LOGE("解码视频成功")
                       consumeAct->callJavaMethod.callBackJava(avFrame->width,
                               avFrame->height,
                               (jbyte*)avFrame->data[0],
                               (jbyte*)avFrame->data[1],
                               (jbyte*)avFrame->data[2]);
                   }
                   av_frame_free(&avFrame);
               }
               av_packet_unref(avPacket);

            consumeAct->queueVideo.pop();
            if (consumeAct->queueVideo.size()<32){
                pthread_mutex_unlock(&consumeAct->mutex);
            }



        } else {
            LOGE("消费者正在等待中产品")
            pthread_cond_wait(&consumeAct->cond, &consumeAct->mutex);
        }
        pthread_mutex_unlock(&consumeAct->mutex);
//        usleep(1000 * 1);
    }
    pthread_exit(&consumeAct->cusId);
}


ConsumeAction::ConsumeAction(CallJavaMethod callJavaMethod)
        : callJavaMethod(callJavaMethod) {
    
};
//void * consumer1(void* data){
//    ConsumeAction* cc = static_cast<ConsumeAction *>(data);
//    cc->consumer(data);
//}

void ConsumeAction::init() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&cusId, NULL, consumer,this);
}


void ConsumeAction::push(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    AVPacket* refPacket = av_packet_alloc();
    if(av_packet_ref(refPacket,avPacket)==0){
        queueVideo.push(refPacket );
        if (queueVideo.size()>32){

        }
    } else {
        LOGE("视频帧入队出错")
    }

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);


}







