LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := cube
LOCAL_SHARED_LIBRARIES := minko minko_png
LOCAL_SRC_FILES := cube.cpp cubeJNI.cpp

LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

include $(BUILD_SHARED_LIBRARY)
