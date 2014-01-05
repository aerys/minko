MY_LOCAL_PATH_2 := $(call my-dir)

include $(MY_LOCAL_PATH_2)/src/BulletSoftBody/Android.mk
include $(MY_LOCAL_PATH_2)/src/BulletCollision/Android.mk
include $(MY_LOCAL_PATH_2)/src/LinearMath/Android.mk
include $(MY_LOCAL_PATH_2)/src/BulletDynamics/Android.mk
include $(MY_LOCAL_PATH_2)/src/MiniCL/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH_2)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet
LOCAL_CFLAGS :=
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_STATIC_LIBRARIES := \
bullet-dynamics \
bullet-collision \
bullet-soft \
bullet-linearMath \
bullet-minicl

include $(BUILD_STATIC_LIBRARY)
