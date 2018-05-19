package com.android.h264.h264analyzerandroid;

import java.util.ArrayList;

/**
 * Created by zhongjihao on 18-4-26.
 */

public class H264ParseWrap {
    private long cPtr;
    private static H264ParseWrap mInstance;
    private static Object lockobj = new Object();

    private H264ParseWrap() {
        cPtr = 0;
    }

    public static H264ParseWrap newInstance() {
        synchronized (lockobj) {
            if (mInstance == null) {
                mInstance = new H264ParseWrap();
            }
        }
        return mInstance;
    }

    //启动h264分析器
    public void startH264Engine() {
        cPtr = H264AnalyzeNativeJni.startH264Engine();
    }

    //获取文件中总的nalu条目
    public int getTotalNalItem(String h264BitstreamFile) {
        if (cPtr != 0) {
            return H264AnalyzeNativeJni.getTotalNalItem(cPtr,h264BitstreamFile);
        }
        return -1;
    }

    //从文件中加载loadItemCount个nalu
    public ArrayList<NaluItem> h264_nal_parse(int loadItemCount, String h264BitstreamFile) {
        if (cPtr != 0) {
            return H264AnalyzeNativeJni.h264_nal_parse(cPtr,loadItemCount, h264BitstreamFile);
        }
        return null;
    }

    //checkBox选择事件
    public void setCheckBox(boolean isCheck,int showNaluNumber) {
        if (cPtr != 0) {
            H264AnalyzeNativeJni.setCheckBox(cPtr, isCheck,showNaluNumber);
        }
    }

    //解析ListView中指定位置的nalu条目
    public String probe_nal_unit(int index) {
        if (cPtr != 0) {
            return H264AnalyzeNativeJni.probe_nal_unit(cPtr, index);
        }
        return null;
    }

    //停止h264分析器
    public void stopH264Engine() {
        if (cPtr != 0) {
            H264AnalyzeNativeJni.stopH264Engine(cPtr);
        }
        mInstance = null;
    }

}
