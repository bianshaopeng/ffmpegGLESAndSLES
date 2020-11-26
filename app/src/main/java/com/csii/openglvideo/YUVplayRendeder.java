package com.csii.openglvideo;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class YUVplayRendeder implements GLSurfaceView.Renderer {

    private int mProgram;
    private FloatBuffer vexFloatBuffer;
    private float[] squareCoords = {
            //满屏
            -1.0f, -1.0f,  //1
            1.0f, -1.0f,   //2
            -1.0f, 1.0f,   //3
            1.0f, 1.0f,    //4

    };
    private FloatBuffer fragFloatBuffer;
    private final float[] textureVertexData = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f

    };
    Buffer y, u, v;
    int w, h;
    private Context context;

    public YUVplayRendeder(Context context) {
        this.context = context;
    }

    int positonHandle;
    int textureInHandle;
    int text_y, text_u, text_v;
    int[] texture;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //创建一个渲染程序
        mProgram = GLES20.glCreateProgram();
        //分配内存
        ByteBuffer buffer = ByteBuffer.allocateDirect(squareCoords.length * 4);
        buffer.order(ByteOrder.nativeOrder());
        vexFloatBuffer = buffer.asFloatBuffer();
        vexFloatBuffer.put(squareCoords);
        vexFloatBuffer.position(0);
        ByteBuffer buffer1 = ByteBuffer.allocateDirect(textureVertexData.length * 4);
        buffer1.order(ByteOrder.nativeOrder());
        fragFloatBuffer = buffer1.asFloatBuffer();
        fragFloatBuffer.put(textureVertexData);
        fragFloatBuffer.position(0);
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, FFmpegUtil.readFileFromRaw(context, R.raw.yuv_vex));
        //将着色器添加到渲染程序中
        GLES20.glAttachShader(mProgram, vertexShader);
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, FFmpegUtil.readFileFromRaw(context, R.raw.yuv_fram));
        GLES20.glAttachShader(mProgram, fragmentShader);
        //链接源程序
        GLES20.glLinkProgram(mProgram);
        int[] linkStatus = new int[1];
        GLES20.glGetProgramiv(mProgram, GLES20.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES20.GL_TRUE) {
            GLES20.glDeleteProgram(mProgram);
            mProgram = 0;
        }
        //得到着色器中的属性
        positonHandle = GLES20.glGetAttribLocation(mProgram, "av_Position");
        textureInHandle = GLES20.glGetAttribLocation(mProgram, "af_Position");


        text_y = GLES20.glGetUniformLocation(mProgram, "sampler_y");
        text_u = GLES20.glGetUniformLocation(mProgram, "sampler_u");
        text_v = GLES20.glGetUniformLocation(mProgram, "sampler_v");

        texture = new int[3];
        //创建纹理ID
        GLES20.glGenTextures(3, texture, 0);
        for (int i = 0; i < texture.length; i++) {
            //绑定纹理ID
            GLES20.glActiveTexture(texture[i]);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture[i]);
            //设置属性 当显示的纹理比加载的纹理大时 使用纹理坐标中最接近的若干个颜色 通过加权算法获得绘制颜色
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            // 比加载的小
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            // 如果纹理坐标超出范围 0,0-1,1 坐标会被截断在范围内
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        }

    }

    public void setFrameData(int w, int h, byte[] y, byte[] u, byte[] v) {
        Log.d("YUV", "数据传输");
        this.w = w;
        this.h = h;
        this.y = ByteBuffer.wrap(y);
        this.u = ByteBuffer.wrap(u);
        this.v = ByteBuffer.wrap(v);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        GLES20.glClearColor(0f, 0f, 0f, 1f);
        if (y != null) {
            //使用源程序
            GLES20.glUseProgram(mProgram);
            //使顶点属性数组有效
            GLES20.glEnableVertexAttribArray(positonHandle);
            //为顶点属性赋值
            /**
             * positonHandle 着色器属性。
             * 2  squareCoords物体顶点坐标数组两个坐标确定一个点。
             * GLES20.GL_FLOAT  坐标点的类型。
             * false 不需要同步，因为坐标的大小都是【-1,1】这个范围如果大于【-1,1】这个范围就设为true,程序会自动同步成【-1,1】这个范围
             * 如一个点的坐标是(-20,35),就会同步成(-1,1)。
             * 8   就等于2*4 两个点确定一个坐标，一个点的数组类型是float类型是4个字节
             * vexFloatBuffer 将内存中的坐标数据产给着色器
             */
            GLES20.glVertexAttribPointer(positonHandle, 2, GLES20.GL_FLOAT, false,
                    8, vexFloatBuffer);
            vexFloatBuffer.position(0);
            GLES20.glEnableVertexAttribArray(textureInHandle);
            GLES20.glVertexAttribPointer(textureInHandle, 2, GLES20.GL_FLOAT, false, 8, fragFloatBuffer);

            //使 GL_TEXTURE0 单元 活跃 opengl最多支持16个纹理
            //纹理单元是显卡中所有的可用于在shader中进行纹理采样的显存 数量与显卡类型相关，至少16个
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            //绑定纹理空间 下面的操作就会作用在这个空间中
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture[0]);
            //创建一个2d纹理 使用亮度颜色模型并且纹理数据也是亮度颜色模型
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, w, h, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);
            //绑定采样器与纹理单元
            GLES20.glUniform1i(text_y, 0);


            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture[1]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, w / 2, h / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE,
                    u);
            GLES20.glUniform1i(text_u, 1);


            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture[2]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, w / 2, h / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE,
                    v);
            GLES20.glUniform1i(text_v, 2);
            y.clear();
            u.clear();
            v.clear();
            y = null;
            u = null;
            v = null;
            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
//            GLES20.glDisableVertexAttribArray(positonHandle);
//            GLES20.glDisableVertexAttribArray(textureInHandle);
        }


    }

    int loadShader(int type, String shaderCode) {
        //根据type创建顶点着色器或者片元着色器,返回一个容器的句柄
        int shader = GLES20.glCreateShader(type);
        //将资源加入到着色器句柄所关联的内存中，并编译
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);
        //验证Shader是否编译通过 intBuffer 返回1时编译成功，返回0编译失败
        IntBuffer intBuffer = IntBuffer.allocate(1);
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, intBuffer);
        Log.d("tagxxx", "shader编译：" + intBuffer.get(0));
        return shader;
    }
}
