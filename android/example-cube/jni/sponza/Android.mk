LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sponza
LOCAL_SHARED_LIBRARIES := minko minko_png minko_jpeg minko_particles minko_mk minko_bullet
LOCAL_SRC_FILES := sponza.cpp minko/component/SponzaLighting.cpp sponzaJNI.cpp

LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

include $(BUILD_SHARED_LIBRARY)
