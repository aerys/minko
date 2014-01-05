LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet-soft
LOCAL_CFLAGS :=
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
./btSoftBodyRigidBodyCollisionConfiguration.cpp \
./btSoftBody.cpp \
./btSoftBodyHelpers.cpp \
./btSoftRigidDynamicsWorld.cpp \
./btDefaultSoftBodySolver.cpp \
./btSoftSoftCollisionAlgorithm.cpp \
./btSoftBodyConcaveCollisionAlgorithm.cpp \
./btSoftRigidCollisionAlgorithm.cpp

include $(BUILD_STATIC_LIBRARY)
