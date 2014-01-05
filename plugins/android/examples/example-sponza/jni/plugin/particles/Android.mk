LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := minko_particles
LOCAL_CFLAGS := -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/
LOCAL_SHARED_LIBRARIES := minko
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src/
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

LOCAL_SRC_FILES := src/minko/geometry/ParticlesGeometry.cpp \
src/minko/particle/modifier/SizeBySpeed.cpp \
src/minko/particle/modifier/ForceOverTime.cpp \
src/minko/particle/modifier/StartSprite.cpp \
src/minko/particle/modifier/StartColor.cpp \
src/minko/particle/modifier/VelocityOverTime.cpp \
src/minko/particle/modifier/ColorBySpeed.cpp \
src/minko/particle/modifier/StartRotation.cpp \
src/minko/particle/modifier/StartVelocity.cpp \
src/minko/particle/modifier/StartSize.cpp \
src/minko/particle/modifier/SizeOverTime.cpp \
src/minko/particle/modifier/StartForce.cpp \
src/minko/particle/modifier/StartAngularVelocity.cpp \
src/minko/particle/modifier/ColorOverTime.cpp \
src/minko/particle/shape/Cone.cpp \
src/minko/particle/shape/EmitterShape.cpp \
src/minko/particle/shape/Cylinder.cpp \
src/minko/particle/shape/Point.cpp \
src/minko/particle/shape/Sphere.cpp \
src/minko/particle/shape/Box.cpp \
src/minko/render/ParticleIndexBuffer.cpp \
src/minko/render/ParticleVertexBuffer.cpp \
src/minko/controller/ParticleSystem.cpp

include $(BUILD_SHARED_LIBRARY)
