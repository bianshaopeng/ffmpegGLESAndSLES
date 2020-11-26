package com.csii.openglvideo;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class FFmpegUtil {

    public static String readFileFromRaw(Context context, int resourceId) {


        InputStream inputStream = context.getResources().openRawResource(resourceId);
        BufferedReader buffer = new BufferedReader(new InputStreamReader(inputStream));
        String line;
        StringBuilder builder = new StringBuilder();

        try {
            while ((line=buffer.readLine()) != null) {
                builder.append(line);
                builder.append("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();


        }
        return builder.toString();


    }
}
