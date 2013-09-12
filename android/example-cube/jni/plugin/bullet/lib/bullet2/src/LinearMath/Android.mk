
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet-linearMath
LOCAL_CFLAGS :=
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
./btVector3.cpp \
./btConvexHull.cpp \
./btGeometryUtil.cpp \
./btPolarDecomposition.cpp \
./btSerializer.cpp \
./btConvexHullComputer.cpp \
./btAlignedAllocator.cpp \
./btQuickprof.cpp

include $(BUILD_STATIC_LIBRARY)
