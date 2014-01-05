LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := minko_particles
LOCAL_CFLAGS := -std=c++11
LOCAL_SHARED_LIBRARIES := minko
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

LOCAL_SRC_FILES := ./minko/geometry/ParticlesGeometry.cpp \
./minko/particle/modifier/SizeBySpeed.cpp \
./minko/particle/modifier/ForceOverTime.cpp \
./minko/particle/modifier/StartSprite.cpp \
./minko/particle/modifier/StartColor.cpp \
./minko/particle/modifier/VelocityOverTime.cpp \
./minko/particle/modifier/ColorBySpeed.cpp \
./minko/particle/modifier/StartRotation.cpp \
./minko/particle/modifier/StartVelocity.cpp \
./minko/particle/modifier/StartSize.cpp \
./minko/particle/modifier/SizeOverTime.cpp \
./minko/particle/modifier/StartForce.cpp \
./minko/particle/modifier/StartAngularVelocity.cpp \
./minko/particle/modifier/ColorOverTime.cpp \
./minko/particle/shape/Cone.cpp \
./minko/particle/shape/EmitterShape.cpp \
./minko/particle/shape/Cylinder.cpp \
./minko/particle/shape/Point.cpp \
./minko/particle/shape/Sphere.cpp \
./minko/particle/shape/Box.cpp \
./minko/render/ParticleIndexBuffer.cpp \
./minko/render/ParticleVertexBuffer.cpp \
./minko/controller/ParticleSystem.cpp

include $(BUILD_SHARED_LIBRARY)
