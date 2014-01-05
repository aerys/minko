LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jpeg-compressor
LOCAL_CFLAGS :=
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_SRC_FILES := src/jpgd.cpp

include $(BUILD_STATIC_LIBRARY)
