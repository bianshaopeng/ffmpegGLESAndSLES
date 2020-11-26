//
// Created by bsp on 20-11-20.
//

#include "audio.h"

void* consumerAudio(void* data){
    Audio* audioContext = static_cast<Audio *>(data);
    audioContext->playAudio();
    return (void *)0;
}
void getQueueCallBack(SLAndroidSimpleBufferQueueItf  slBufferQueueItf, void* context){
    Audio* audioPrt = static_cast<Audio *>(context);
//    buffersize1=0;

    audioPrt->getPcm(reinterpret_cast<void **>(&audioPrt->buffer), &audioPrt->buffersize);
    if(audioPrt->buffer!=NULL&&audioPrt->buffersize!=0){
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,audioPrt->buffer,audioPrt->buffersize);
    }
}
void Audio::createEngine() {
    slCreateEngine(&pEngine, 0, nullptr, 0, nullptr, nullptr);//创建引擎
    (*pEngine)->Realize(pEngine, SL_BOOLEAN_FALSE);//实现SLObjectItf接口对象
    (*pEngine)->GetInterface(pEngine, SL_IID_ENGINE, &engineEngine);//通过引擎调用初始化SLObjectItf

}

void Audio::createMixVolume() {
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);//用引擎创建混音器接口对象
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);//实现混音器接口对象
    SLresult sLresult = (*outputMixObject)->GetInterface(outputMixObject,
                                                         SL_IID_ENVIRONMENTALREVERB,
                                                         &outputMixEnvironmentalReverb);//利用混音器实例对象接口初始化具体的混音器对象
    //设置
    if (SL_RESULT_SUCCESS == sLresult) {
        (*outputMixEnvironmentalReverb)->
                SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
}


//void getQueueCallBack(SLAndroidSimpleBufferQueueItf slBufferQueueItf, void *context) {
//    buffersize=0;
//
////    getPcm(&buffer,&buffersize);
//    if(buffer!=NULL&&buffersize!=0){
//        //将得到的数据加入到队列中
//        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,buffer,buffersize);
//    }
//}

void Audio::playAudio() {
    LOGE("音频播放");
    createEngine();
    createMixVolume();
    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    /**
    typedef struct SLDataFormat_PCM_ {
        SLuint32        formatType;  pcm
        SLuint32        numChannels;  通道数
        SLuint32        samplesPerSec;  采样率
        SLuint32        bitsPerSample;  采样位数
        SLuint32        containerSize;  包含位数
        SLuint32        channelMask;     立体声
        SLuint32        endianness;    end标志位
    } SLDataFormat_PCM;
     */
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM
                            ,2//(立体声)
                            ,SL_SAMPLINGRATE_44_1//44100
                            ,SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                            SL_BYTEORDER_LITTLEENDIAN};

    /*
     * typedef struct SLDataSource_ {
            void *pLocator;//缓冲区队列
            void *pFormat;//数据样式,配置信息
        } SLDataSource;
     * */
    SLDataSource dataSource = {&android_queue, &pcm};


    SLDataLocator_OutputMix slDataLocator_outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};


    SLDataSink slDataSink = {&slDataLocator_outputMix, NULL};


    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE};

    /*
     * SLresult (*Createaudioplayer) (
        SLEngineItf self,
        SLObjectItf * pPlayer,
        SLDataSource *pAudioSrc,//数据设置
        SLDataSink *pAudioSnk,//关联混音器
        SLuint32 numInterfaces,
        const SLInterfaceID * pInterfaceIds,
        const SLboolean * pInterfaceRequired
    );
     * */
    (*engineEngine)->CreateAudioPlayer(engineEngine, &audioplayer, &dataSource, &slDataSink, 3, ids,
                                       req);
    (*audioplayer)->Realize(audioplayer, SL_BOOLEAN_FALSE);
    (*audioplayer)->GetInterface(audioplayer, SL_IID_PLAY, &slPlayItf);//初始化播放器
    //注册缓冲区,通过缓冲区里面 的数据进行播放
    (*audioplayer)->GetInterface(audioplayer, SL_IID_BUFFERQUEUE, &slBufferQueueItf);
    //设置回调接口

    (*slBufferQueueItf)->RegisterCallback(slBufferQueueItf,
                                          getQueueCallBack, this);
    //播放
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    getQueueCallBack(slBufferQueueItf,this);
    LOGE("等待播放");
}



Audio::Audio() {

}

void Audio::getAudioFrame(AVFrame *frame) {
     avFrame = frame;
    //音频
    SwrContext* swrContext = swr_alloc();
    uint8_t* out_buffer = (uint8_t *) av_malloc(44100 * 2);
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);//获取通道数
    swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) avFrame->data, avFrame->nb_samples);
//                缓冲区的大小
    int size = av_samples_get_buffer_size(NULL, out_channer_nb, avFrame->nb_samples,
                                      AV_SAMPLE_FMT_S16, 1);
    buffersize = size;
    buffer = out_buffer;
    if(buffer!=NULL&&buffersize!=0){
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf,buffer,buffersize);
    }
}

void Audio::init() {
//    pthread_mutex_init(&audioMutex,NULL);
//    pthread_cond_init(&audioCond,NULL);
    pthread_create(&audioId,NULL,consumerAudio,this);

}

void Audio::push(AVPacket * packet) {
//    pthread_mutex_lock(&audioMutex);
    AVPacket* aduioPacket = av_packet_alloc();
    if (av_packet_ref(aduioPacket,packet)==0){
        audioQueue.push(aduioPacket);
    }
//    pthread_cond_signal(&audioCond);
//    pthread_mutex_unlock(&audioMutex);

}

int Audio::getPcm(void **pcm, size_t *pcm_size) {
    buffer = (uint8_t *) av_malloc(44100 * 2);
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
//    输出采样位数  16位
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = avCodecContext->sample_rate;


    swrContext = swr_alloc_set_opts(NULL,
                                                  out_ch_layout, out_formart,
                                                  out_sample_rate,
                                                  avCodecContext->channel_layout,
                                                  avCodecContext->sample_fmt,
                                                  avCodecContext->sample_rate, 0,
                                                  NULL);

    swr_init(swrContext);
//    获取通道数  2
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    isPlay = true;
    while (isPlay){
        if (audioQueue.size()>0){
//            pthread_mutex_lock(&audioMutex);
            AVPacket* avPacket = audioQueue.front();
            int ret = avcodec_send_packet(avCodecContext,avPacket);
            if(ret==0){
                AVFrame* avFrame = av_frame_alloc();
                if (avcodec_receive_frame(avCodecContext,avFrame)==0){
                    LOGE("解码音频成功")
                    int ret  = swr_convert(swrContext,
                                           &buffer,
                                           44100 * 2,
                                           (const uint8_t **) avFrame->data,
                                           avFrame->nb_samples);

//                缓冲区的大小
                    buffersize = static_cast<size_t>(av_samples_get_buffer_size(NULL,
                                                                                              out_channer_nb,
                                                                                              avFrame->nb_samples,
                                                                                              AV_SAMPLE_FMT_S16,
                                                                                              1));
                    *pcm = buffer;
                    *pcm_size = buffersize;

                }
//                swr_free(&audioContext->swrContext);
                av_frame_unref(avFrame);
                av_frame_free(&avFrame);
            }
            av_packet_unref(avPacket);
            av_packet_free(&avPacket);
            audioQueue.pop();
            break;
//            pthread_mutex_unlock(&audioMutex);
        } else{
//            pthread_cond_wait(&audioCond,&audioMutex);
        }
//        pthread_mutex_unlock(&audioMutex);
    }
    swr_free(&swrContext);
//    pthread_exit(&audioId);
    return 0;
}


