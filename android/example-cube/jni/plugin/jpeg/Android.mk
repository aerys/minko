MY_LOCAL_PATH := $(call my-dir)

include $(MY_LOCAL_PATH)/lib/jpeg-compressor/Android.mk

LOCAL_PATH=$(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := minko_jpeg
LOCAL_CFLAGS := -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_SHARED_LIBRARIES := minko
LOCAL_STATIC_LIBRARIES := jpeg-compressor
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

LOCAL_SRC_FILES := src/minko/file/JPEGParser.cpp

include $(BUILD_SHARED_LIBRARY)


