//
// Created by SPREADTRUM\jihao.zhong on 18-4-13.
//
#include "com_android_h264_swig_SwigTest.h"

JNIEXPORT jstring JNICALL Java_com_android_h264_swig_SwigTest_SayHello(JNIEnv *env, jclass jcls)
{
    return env ->NewStringUTF("SWIG测试");
}
