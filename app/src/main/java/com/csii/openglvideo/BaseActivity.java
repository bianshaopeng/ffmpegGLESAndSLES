package com.csii.openglvideo;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;

import androidx.activity.ComponentActivity;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class BaseActivity extends AppCompatActivity {
    public static final int EXTERNAL_STORAGE_REQ_CODE = 10 ;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestPermission(this);
    }

    private void requestPermission(Context context){
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.CALL_PHONE) !=0){
              if (ActivityCompat.shouldShowRequestPermissionRationale(this,Manifest.permission.CALL_PHONE)){
                  Log.d("permission","请给我权限");
                  ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.CALL_PHONE},EXTERNAL_STORAGE_REQ_CODE);
              }else {
                  ActivityCompat.requestPermissions(this,new String[]{Manifest.permission.CALL_PHONE},EXTERNAL_STORAGE_REQ_CODE);
              }
        }else {

            Log.d("permission","有权限=======");
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case EXTERNAL_STORAGE_REQ_CODE: {
                // 如果请求被拒绝，那么通常grantResults数组为空
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    //申请成功，进行相应操作
                    Log.d("permission","权限申请成功");
                } else {
                    //申请失败，可以继续向用户解释。
                    Log.d("permission","权限申请失败");
                }
                return;
            }
        }
    }
}
