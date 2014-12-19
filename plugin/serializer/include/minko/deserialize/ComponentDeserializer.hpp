/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/SerializerCommon.hpp"
#include "msgpack.hpp"

namespace minko
{
    namespace file
    {
        struct SceneVersion;
    }

	namespace deserialize
	{

		class ComponentDeserializer
		{

		private:
            typedef std::shared_ptr<file::Dependency>				DependencyPtr;
			typedef std::shared_ptr<file::AssetLibrary>				AssetLibraryPtr;
			typedef std::shared_ptr<component::AbstractComponent>	AbsComponentPtr;
			typedef msgpack::type::tuple<uint, std::string>			SerializedMatrix;
			typedef std::vector<SerializedMatrix>					VectorOfSerializedMatrix;
			typedef msgpack::type::tuple<std::string, std::string>	SurfaceExtension;

		public:
			static
			AbsComponentPtr
            deserializeTransform(file::SceneVersion        sceneVersion,
                                 std::string&	    serializedTransformData,
								 AssetLibraryPtr    assetLibrary,
								 DependencyPtr	    dependencies);

			static
			AbsComponentPtr
            deserializeProjectionCamera(file::SceneVersion    sceneVersion,
                                        std::string&	serializedCameraData,
										AssetLibraryPtr	assetLibrary,
										DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializeAmbientLight(file::SceneVersion    sceneVersion, 
                                    std::string&	serializedAmbientLight,
									AssetLibraryPtr	assetLibrary,
									DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializeDirectionalLight(file::SceneVersion    sceneVersion,
                                        std::string&	serializedDirectionalLight,
										AssetLibraryPtr	assetLibrary,
										DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializePointLight(file::SceneVersion      sceneVersion,
                                  std::string&	    serializedPointLight,
								  AssetLibraryPtr   assetLibrary,
								  DependencyPtr	    dependencies);

			static
			AbsComponentPtr
            deserializeSpotLight(file::SceneVersion       sceneVersion,
                                 std::string&	    serializedSpotLight,
								 AssetLibraryPtr    assetLibrary,
								 DependencyPtr	    dependencies);

			static
			AbsComponentPtr
            deserializeSurface(file::SceneVersion     sceneVersion,
                               std::string&		serializedSurface,
							   AssetLibraryPtr	assetLibrary,
							   DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializeRenderer(file::SceneVersion    sceneVersion,
                                std::string&	serializedRenderer,
							    AssetLibraryPtr	assetLibrary,
							    DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializeAnimation(file::SceneVersion       sceneVersion,
                                 std::string&	    serializedAnimation,
								 AssetLibraryPtr    assetLibrary,
								 DependencyPtr	    dependencies);

			static
			AbsComponentPtr
            deserializeSkinning(file::SceneVersion    sceneVersion,
                                std::string&	serializedAnimation,
								AssetLibraryPtr	assetLibrary,
								DependencyPtr	dependencies);

			static
			AbsComponentPtr
            deserializeBoundingBox(file::SceneVersion     sceneVersion,
                                   std::string&	    serializedBoundingBox,
                                   AssetLibraryPtr  assetLibrary,
                                   DependencyPtr    dependencies);
		};
	}
}
