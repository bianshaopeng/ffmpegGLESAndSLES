
#include <mutex>
#include "decode.h"
#include "audio.h"
AVFormatContext* inafc = NULL;
int videoIndex;
int audioIndex;
uint8_t* out_buffer;
int size;
void Decode::ffmpegDecode(char * urlPath){
    action = new ConsumeAction(CallJavaMethod(javaVm,jniEnv, jobj));
    action->init();
    audioAction = new Audio();


    int ret = 0;
    ret = avformat_open_input(&inafc, urlPath, NULL, NULL);
    if (ret < 0) {
        LOGE("open file fail:%d", ret);
        return;
    }
    LOGE("open file success");
    //查找流信息
    ret = avformat_find_stream_info(inafc, NULL);
    if (ret < 0) {
        LOGE("find stream info fail");
        return;
    }
    LOGE("find stream info success");
//        for (int i = 0; i <inafc->nb_streams ; ++i) {
//            if (inafc->streams[i]->codecpar->codec_type = AVMEDIA_TYPE_VIDEO){
//                  videoIndex = i;
//            }
//
//        }
    videoIndex = av_find_best_stream(inafc, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    audioIndex = av_find_best_stream(inafc, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    //寻找解码器

    if (videoIndex!=-1){
        AVCodec *avCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (avCodec == NULL) {
            LOGE("find h264 decoder fail");
        }
        LOGE("find h264 decoder success");

        action->avCodecContext   = avcodec_alloc_context3(avCodec);

        avcodec_parameters_to_context(  action->avCodecContext, inafc->streams[videoIndex]->codecpar);
        //打开解码器
        ret = avcodec_open2(  action->avCodecContext, avCodec, NULL);
    }
    if (audioIndex!=-1){
        audioAction->avCodecContext = inafc->streams[audioIndex]->codec;
        AVCodec *avCodec = avcodec_find_decoder(audioAction->avCodecContext->codec_id);
        ret = avcodec_open2(  audioAction->avCodecContext, avCodec, NULL);
    }
    audioAction->init();
//    audioAction->playAudio();
    if (ret < 0) {
        LOGE("open avcodec fail:%d", ret);
    }

    LOGE("open avcodec success");
    //视频解码
    AVPacket *avPacket = av_packet_alloc();
//    av_init_packet(avPacket);



    while (av_read_frame(inafc, avPacket) >= 0) {

        if (avPacket->stream_index == videoIndex) {
            action->push(avPacket);
        } else if (avPacket->stream_index == audioIndex){
            audioAction->push(avPacket);
        }
        av_packet_unref(avPacket);

    }
    LOGE("播放结束");
    avcodec_close(action->avCodecContext);
    avformat_close_input(&inafc);
    avformat_free_context(inafc);
}
int getPcm(void **pcm,size_t *pcm_size){
    *pcm = out_buffer;
    *pcm_size = size;
    return 0;
}

//Decode::Decode(CallJavaMethod callJavaMethod1):callJavaMethod(callJavaMethod1) {
//
//}

Decode::Decode(JavaVM* Vm,jobject obj,JNIEnv* Env):javaVm(Vm),jobj(obj),jniEnv(Env){

}

Decode::~Decode() {
    delete(action);
}








