
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet-minicl
LOCAL_CFLAGS :=
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
./MiniCLTask/MiniCLTask.cpp \
./MiniCL.cpp \
./MiniCLTaskScheduler.cpp

include $(BUILD_STATIC_LIBRARY)
