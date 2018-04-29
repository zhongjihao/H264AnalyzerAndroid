LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
 
LOCAL_MODULE    	:= h264Analyzer

LOCAL_SRC_FILES 	:= \
	./h264bitstream/NalParse.cpp \
	./h264bitstream/h264_sei.cpp \
	./h264bitstream/h264_stream.cpp \
	./h264bitstream/h264_avcc.cpp \
	./h264bitstream/h264_analyze.cpp \
	jni_h264analy_wrap.cpp

LOCAL_C_INCLUDES := \
    ./h264bitstream/bs.h \
    ./h264bitstream/h264_stream.h \
    ./h264bitstream/h264_analyze.h \
    ./h264bitstream/h264_sei.h \
    ./h264bitstream/h264_avcc.h \
    ./h264bitstream/NalParse.h

LOCAL_LDLIBS := -llog -lm


LOCAL_CFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)
