MY_LOCAL_PATH := $(call my-dir)

include $(MY_LOCAL_PATH)/lib/lodepng/Android.mk

LOCAL_PATH=$(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := minko_png
LOCAL_SRC_FILES := src/minko/file/PNGParser.cpp
LOCAL_CFLAGS := -std=c++11
LOCAL_SHARED_LIBRARIES := minko
LOCAL_STATIC_LIBRARIES := lodepng
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

include $(BUILD_SHARED_LIBRARY)
