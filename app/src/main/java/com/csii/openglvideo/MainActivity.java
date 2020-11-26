package com.csii.openglvideo;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends BaseActivity implements View.OnClickListener {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avfilter");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
    }
//    public  native void couAndPro();
    public native void play(String url);
//    public void getCallBack(int width,int height){
//        Log.d("tag","width:"+width+"  height:"+height);
////        rendeder.setFrameData(width,height,y,u,v);
////        glSurfaceView.requestRender();
//    }

    public void getCallBack(int width,int height,byte [] y,byte[] u,byte[] v){
//        Log.d("tag","width:"+width+"  height:"+height+"y:"+new String(y)+ "u:"+new String(u)+"v:"+new String(v));
        rendeder.setFrameData(width,height,y,u,v);
        glSurfaceView.requestRender();
    }
    private GLSurfaceView glSurfaceView;
    private YUVplayRendeder rendeder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();

    }

    private void initView() {
        findViewById(R.id.btn2).setOnClickListener(this);
        glSurfaceView = findViewById(R.id.glSurfaceView);
        glSurfaceView.setEGLContextClientVersion(2);
        rendeder = new YUVplayRendeder(this);
        glSurfaceView.setRenderer(rendeder);

        /*渲染方式，RENDERMODE_WHEN_DIRTY表示被动渲染，只有在调用requestRender或者onResume等方法时才会进行渲染。RENDERMODE_CONTINUOUSLY表示持续渲染*/
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }


    @Override
    public void onClick(View v) {
      switch (v.getId()){
              case R.id.btn2:
                  final String url = "rtmp://58.200.131.2:1935/livetv/hunantv";
//                      final String url = Environment.getExternalStorageDirectory()+"/1.mp4";
//                      String url = "/storage/emulated/0/2.flv";
//                      play(url);

                  new Thread(new Runnable() {
                      @Override
                      public void run() {
                          play(url);
                      }
                  }).start();

              break;
      }
    }
}
