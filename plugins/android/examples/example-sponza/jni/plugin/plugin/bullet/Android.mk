MY_LOCAL_PATH := $(call my-dir)

include $(MY_LOCAL_PATH)/lib/bullet2/Android.mk

LOCAL_PATH=$(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := minko_bullet
LOCAL_CFLAGS := -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/
LOCAL_SHARED_LIBRARIES := minko
LOCAL_STATIC_LIBRARIES := bullet
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

LOCAL_SRC_FILES := \
src/minko/component/bullet/Collider.cpp \
src/minko/component/bullet/AbstractPhysicsShape.cpp \
src/minko/component/bullet/ColliderData.cpp \
src/minko/component/bullet/PhysicsWorld.cpp \
src/minko/component/bullet/BulletCollider.cpp \
src/minko/component/bullet/LinearIdAllocator.cpp

include $(BUILD_SHARED_LIBRARY)

