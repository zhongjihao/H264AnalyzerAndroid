package com.android.h264.h264analyzerandroid;

import java.util.ArrayList;

/**
 * Created by zhongjihao on 18-4-25.
 */

public class H264AnalyzeNativeJni {
    static {
        System.loadLibrary("h264Analyzer");
    }

    public native static long startH264Engine();
    public native static int getTotalNalItem(long cPtr,String h264BitstreamFile);
    public native static ArrayList<NaluItem> h264_nal_parse(long cPtr, int loadItemCount,String h264BitstreamFile);
    public native static void setCheckBox(long cPtr,boolean isCheck,int showNalNumber);
    public native static String probe_nal_unit(long cPtr,int index);
    public native static void stopH264Engine(long cPtr);
}
