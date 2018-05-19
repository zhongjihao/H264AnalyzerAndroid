//
// Created by zhongjihao on 18-4-25.
//

#define LOG_TAG "H264Analyze-Jni"

#include <jni.h>
#include <vector>

#include "../log.h"
#include "h264bitstream/NalParse.h"

//定义java中类全名
static const char* classPathName = "com/android/h264/h264analyzerandroid/H264AnalyzeNativeJni";

struct InfoCustomersNativeJNI{
    jclass     naluitem_cls;
    jmethodID  constructionID;
   // jmethodID  setNal_indexID;
    jmethodID  setNal_idcID;
    jmethodID  setNal_typeID;
    jmethodID  setNal_lenID;
};

static void initnaluItemNativeJNI(JNIEnv* env,struct InfoCustomersNativeJNI& g_naluItemNativeJNI ,const char* className)
{
    LOGD("====zhongjihao=====initInfoCustomerNativeJNI==start======");
    //根据java类NaluItem获取在jni中的类表示形式
    g_naluItemNativeJNI.naluitem_cls = env->FindClass(className);//FindClass获取的是局部引用,离开作用域会被系统自动回收
    if(g_naluItemNativeJNI.naluitem_cls == NULL){
        LOGE("======not find c++ class NaluItem by java NaluItem======");
        return;
    }

    //获取java类NaluItem的构造函数ID
    if(g_naluItemNativeJNI.constructionID == NULL){
        g_naluItemNativeJNI.constructionID = env->GetMethodID(g_naluItemNativeJNI.naluitem_cls, "<init>", "()V");
        if(g_naluItemNativeJNI.constructionID == NULL){
            LOGE("==not get c++ class NaluItem constructionID======");
            return;
        }
    }

    //获取JAVA类NaluItem中setNal_idc方法ID
    if(g_naluItemNativeJNI.setNal_idcID == NULL){
        g_naluItemNativeJNI.setNal_idcID = env->GetMethodID(g_naluItemNativeJNI.naluitem_cls,"setNal_idc","(Ljava/lang/String;)V");
        if(g_naluItemNativeJNI.setNal_idcID == NULL){
            LOGE("=======not get c++ class NaluItem setNal_idcID======");
            return;
        }
    }

    //获取JAVA类NaluItem中setNal_type方法ID
    if(g_naluItemNativeJNI.setNal_typeID == NULL){
        g_naluItemNativeJNI.setNal_typeID = env->GetMethodID(g_naluItemNativeJNI.naluitem_cls,"setNal_type","(Ljava/lang/String;)V");
        if(g_naluItemNativeJNI.setNal_typeID == NULL){
            LOGE("=======not get c++ class NaluItem setNal_typeID======");
            return;
        }
    }

    //获取JAVA类NaluItem中setNal_len方法ID
    if(g_naluItemNativeJNI.setNal_lenID == NULL){
        g_naluItemNativeJNI.setNal_lenID = env->GetMethodID(g_naluItemNativeJNI.naluitem_cls,"setNal_len","(I)V");
        if(g_naluItemNativeJNI.setNal_lenID == NULL){
            LOGE("=======not get c++ class NaluItem setNal_lenID======");
            return;
        }
    }
}

jlong JNI_startH264Engine(JNIEnv *env,jclass jcls __unused)
{
    NalParse* pNalParse =  new NalParse;
    return reinterpret_cast<long> (pNalParse);
}

jint JNI_getTotalNalItem(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jstring jfilepath)
{
    NalParse* pNalParse = reinterpret_cast<NalParse *> (jcPtr);
    const char* filePath = env ->GetStringUTFChars(jfilepath, NULL);
    int total =  pNalParse->h264_nal_total(filePath);
    env ->ReleaseStringUTFChars(jfilepath, filePath);
    return total;
}

jobject /*jobjectArray*/ JNI_h264_nal_parse(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jint jloadItemCount,jstring jfilepath)
{
    NalParse* pNalParse = reinterpret_cast<NalParse *> (jcPtr);
    const char* filePath = env ->GetStringUTFChars(jfilepath, NULL);
    LOGD("%s: ===zhongjihao===java pass filePath: %s",__FUNCTION__,filePath);
    std::vector<NALU_ITEM*>& naluSets = pNalParse->h264_nal_parse(filePath,(int)jloadItemCount);
    int elementCount = naluSets.size();
    struct InfoCustomersNativeJNI g_naluItemNativeJNI;
    initnaluItemNativeJNI(env,g_naluItemNativeJNI,"com/android/h264/h264analyzerandroid/NaluItem");
    LOGD("%s: ===zhongjihao===1===elementCount: %d",__FUNCTION__,elementCount);
    jclass list_cls = env->FindClass("java/util/ArrayList");//获得ArrayList类引用
    LOGD("%s: ===zhongjihao===2===elementCount: %d",__FUNCTION__,elementCount);
    jmethodID list_construct = env->GetMethodID(list_cls,"<init>","()V");
    LOGD("%s: ===zhongjihao===3===elementCount: %d",__FUNCTION__,elementCount);
    jobject list_obj = env->NewObject(list_cls,list_construct,"");
    LOGD("%s: ===zhongjihao===4===elementCount: %d",__FUNCTION__,elementCount);
    jmethodID list_add = env->GetMethodID(list_cls,"add","(Ljava/lang/Object;)Z");
    LOGD("%s: ===zhongjihao===5===elementCount: %d",__FUNCTION__,elementCount);
    //创建一个元素个数为elementCount的对象NaluItem数组
   // jobjectArray resultArray = env->NewObjectArray(elementCount,g_naluItemNativeJNI.naluitem_cls,NULL);
    for(int i = 0;i < elementCount;i++)
    {
        //创建java对象NaluItem
        jobject naluItem = env->NewObject(g_naluItemNativeJNI.naluitem_cls,g_naluItemNativeJNI.constructionID);
        //给java对象NaluItem成员nal_index赋值
       // env->CallVoidMethod(naluItem,g_naluItemNativeJNI.setNal_indexID,(jint)naluSets[i]->nal_index);
        //给java对象NaluItem成员nal_idc赋值
        jstring idc = env ->NewStringUTF(naluSets[i]->nal_idc);
        env->CallVoidMethod(naluItem,g_naluItemNativeJNI.setNal_idcID,idc);
        env->DeleteLocalRef(idc);
        //给java对象NaluItem成员nal_type赋值
        jstring type = env ->NewStringUTF(naluSets[i]->nal_type);
        env->CallVoidMethod(naluItem,g_naluItemNativeJNI.setNal_typeID,type);
        env->DeleteLocalRef(type);
        //给java对象NaluItem成员nal_len赋值
        env->CallVoidMethod(naluItem,g_naluItemNativeJNI.setNal_lenID,(jint)naluSets[i]->nal_len);

        //给数组中的相应元素赋值
       // env->SetObjectArrayElement(resultArray, i,naluItem);
        env->CallBooleanMethod(list_obj,list_add,naluItem);
        env->DeleteLocalRef(naluItem);
    }
    env->DeleteLocalRef(g_naluItemNativeJNI.naluitem_cls);
    env ->ReleaseStringUTFChars(jfilepath, filePath);
    return list_obj;//resultArray;//从该函数返回，创建的本地引用resultArray对象数组会被JVM自动回收
}

void JNI_setCheckBox(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jboolean jisCheck,jint jshowNalCount)
{
    NalParse* pNalParse = reinterpret_cast<NalParse *> (jcPtr);
    pNalParse->setCheckBox((bool)jisCheck,(int)jshowNalCount);
}

jstring JNI_probe_nal_unit(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jint jindex)
{
    NalParse* pNalParse = reinterpret_cast<NalParse *> (jcPtr);
    int index;
    index = (int)jindex;
    LOGD("%s: ===zhongjihao===index: %d",__FUNCTION__, index);
    const char* nalInfo = pNalParse->getH264Analyze()->probe_nal_unit(index);
    jstring jnalInfo = env ->NewStringUTF(nalInfo);
    LOGD("%s: ===zhongjihao===nalInfo: %s",__FUNCTION__,nalInfo);
    return jnalInfo;
}

void JNI_stopH264Engine(JNIEnv *env,jclass jcls __unused,jlong jcPtr)
{
    NalParse* pNalParse = reinterpret_cast<NalParse *> (jcPtr);
    delete pNalParse;
}

//JAVA函数和C++函数映射关系表
static JNINativeMethod gMethods[] = {
        { "startH264Engine","()J",(void*)JNI_startH264Engine},
        { "getTotalNalItem","(JLjava/lang/String;)I",(void*)JNI_getTotalNalItem},
    //  { "h264_nal_parse","(JLjava/lang/String;)[Lcom/android/h264/h264analyzerandroid/NaluItem;",(void*)JNI_h264_nal_parse},
        { "h264_nal_parse","(JILjava/lang/String;)Ljava/util/ArrayList;",(void*)JNI_h264_nal_parse},
        { "setCheckBox","(JZI)V",(void*)JNI_setCheckBox},
        { "probe_nal_unit","(JI)Ljava/lang/String;",(void*)JNI_probe_nal_unit},
        { "stopH264Engine","(J)V",(void*)JNI_stopH264Engine},
};

typedef union{
    JNIEnv* env;
    void* venv;
}UnionJNIEnvToVoid;

static jint registerNativeMethods(JNIEnv* env,const char* className,JNINativeMethod* gMethods,int numMethods)
{
    jclass clazz;
    clazz = env ->FindClass(className);

    if(clazz == NULL){
        return JNI_FALSE;
    }

    if(env->RegisterNatives(clazz,gMethods,numMethods) < 0){
        env ->DeleteLocalRef(clazz);
        return JNI_FALSE;
    }

    env ->DeleteLocalRef(clazz);
    return JNI_TRUE;
}


static jint registerNatives(JNIEnv* env,const char* classPathName,JNINativeMethod *gMethods,int methodsNum)
{
    if(!registerNativeMethods(env,classPathName,gMethods,methodsNum)){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

//JNI库加载时自动调用该函数
jint JNI_OnLoad(JavaVM* vm, void* reserved __unused)
{
    UnionJNIEnvToVoid uenv;
    JNIEnv* env = NULL;

    LOGD("%s: ==zhongjihao========1===",__FUNCTION__);
    //获得JNI版本
    if(vm->GetEnv((void**)&uenv.venv,JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }

    LOGD("%s: ===zhongjihao======2====",__FUNCTION__);
    env = uenv.env;
    //注册java函数
    if(registerNatives(env,classPathName,gMethods,sizeof(gMethods)/sizeof(gMethods[0])) != JNI_TRUE){
        return -1;
    }

    LOGD("%s: ===zhongjihao======3===",__FUNCTION__);
    return JNI_VERSION_1_4;
}


