LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := minko_mk
LOCAL_CFLAGS := -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_SHARED_LIBRARIES := minko
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2

LOCAL_SRC_FILES := src/minko/data/HalfEdge.cpp \
src/minko/data/HalfEdgeCollection.cpp \
src/minko/file/MkParser.cpp \
src/minko/deserialize/NodeDeserializer.cpp \
src/minko/deserialize/GeometryDeserializer.cpp \
src/minko/deserialize/TypeDeserializer.cpp \
src/minko/deserialize/AssetsDeserializer.cpp \
src/minko/deserialize/SceneDeserializer.cpp \
src/minko/deserialize/NameConverter.cpp

include $(BUILD_SHARED_LIBRARY)
