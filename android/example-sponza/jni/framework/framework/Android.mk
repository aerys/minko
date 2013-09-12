LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := minko
LOCAL_LDLIBS	:= -llog -landroid -lGLESv2
LOCAL_CPPFLAGS := -std=c++11 -DDEBUG -DJSON_IS_AMALGAMATION
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib/jsoncpp/src/ $(LOCAL_PATH)/src
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -DDEBUG
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_SRC_FILES := lib/jsoncpp/src/jsoncpp.cpp \
src/minko/data/Container.cpp \
src/minko/data/Provider.cpp \
src/minko/data/ArrayProvider.cpp \
src/minko/file/APKLoader.cpp \
src/minko/file/EffectParser.cpp \
src/minko/file/AssetLibrary.cpp \
src/minko/file/Loader.cpp \
src/minko/file/Options.cpp \
src/minko/geometry/SphereGeometry.cpp \
src/minko/geometry/QuadGeometry.cpp \
src/minko/geometry/Geometry.cpp \
src/minko/geometry/CubeGeometry.cpp \
src/minko/scene/NodeSet.cpp \
src/minko/scene/Node.cpp \
src/minko/math/Quaternion.cpp \
src/minko/math/Matrix4x4.cpp \
src/minko/render/DrawCall.cpp \
src/minko/render/Shader.cpp \
src/minko/render/Texture.cpp \
src/minko/render/IndexBuffer.cpp \
src/minko/render/Program.cpp \
src/minko/render/VertexBuffer.cpp \
src/minko/render/Pass.cpp \
src/minko/render/Effect.cpp \
src/minko/render/OpenGLES2Context.cpp \
src/minko/component/AbstractRootDataComponent.cpp \
src/minko/component/Renderer.cpp \
src/minko/component/AbstractDiscreteLight.cpp \
src/minko/component/SpotLight.cpp \
src/minko/component/AmbientLight.cpp \
src/minko/component/Surface.cpp \
src/minko/component/PointLight.cpp \
src/minko/component/PerspectiveCamera.cpp \
src/minko/component/SceneManager.cpp \
src/minko/component/Transform.cpp \
src/minko/component/AbstractLight.cpp \
src/minko/component/LightManager.cpp \
src/minko/component/DirectionalLight.cpp


include $(BUILD_SHARED_LIBRARY)

