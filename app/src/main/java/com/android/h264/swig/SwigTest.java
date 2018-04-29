package com.android.h264.swig;

/**
 * Created by apadmin on 18-4-13.
 */

public class SwigTest {
    static {
        System.loadLibrary("swigTest");
    }
    public static native String  SayHello();
}
