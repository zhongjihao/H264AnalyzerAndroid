package com.android.h264.h264analyzerandroid;

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

    public void startH264Engine() {
        cPtr = H264AnalyzeNativeJni.startH264Engine();
    }

    public NaluItem[] h264_nal_parse(String h264BitstreamFile) {
        if (cPtr != 0) {
            return H264AnalyzeNativeJni.h264_nal_parse(cPtr, h264BitstreamFile);
        }
        return null;
    }

    public void setCheckBox(boolean isCheck,int showNaluNumber) {
        if (cPtr != 0) {
            H264AnalyzeNativeJni.setCheckBox(cPtr, isCheck,showNaluNumber);
        }
    }

    public String probe_nal_unit(int index) {
        if (cPtr != 0) {
            return H264AnalyzeNativeJni.probe_nal_unit(cPtr, index);
        }
        return null;
    }

    public void stopH264Engine() {
        if (cPtr != 0) {
            H264AnalyzeNativeJni.stopH264Engine(cPtr);
        }
    }

}
