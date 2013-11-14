/*
Copyright (c) 2013 Aerys

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

#include "minko/Common.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/Loader.hpp"

struct aiMesh;
struct aiNode;
struct aiScene;

namespace minko
{
	namespace file
	{
		class ASSIMPParser :
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<ASSIMPParser> Ptr;

        private:
            typedef std::shared_ptr<AbstractLoader>				LoaderPtr;
			typedef std::shared_ptr<scene::Node>				NodePtr;
			typedef std::shared_ptr<component::SceneManager>	SceneManagerPtr;
            
        private:
            const aiScene*                  _aiscene;
            unsigned int                    _numDependencies;
			unsigned int                    _numLoadedDependencies;
            std::string                     _filename;
            std::shared_ptr<AssetLibrary>   _assetLibrary;
			std::vector<LoaderPtr>			_dependencies;
            
		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<ASSIMPParser>(new ASSIMPParser());
			}

			void
			parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetLibrary>		assetLibrary);

		private:
			ASSIMPParser()
			{
			}

            void
			createSceneTree(NodePtr minkoNode, aiNode* ainode, SceneManagerPtr sceneManager);

            void
            createMeshGeometry(NodePtr minkoNode, aiMesh* mesh, SceneManagerPtr sceneManager);
            
            std::shared_ptr<component::Transform>
            getTransformFromAssimp(aiNode* ainode);
            
            void
            createMeshSurface(NodePtr minkoNode, aiMesh* mesh, SceneManagerPtr sceneManager);

            void
            createLights(NodePtr minkoRoot);
            
            NodePtr
            findNode(std::string name, NodePtr root);
            
            void
            queueAssimpTexture(SceneManagerPtr sceneManager);
            
            void
			parseDependencies(const std::string& 				filename,
							  std::shared_ptr<file::Options> 	options,
							  std::vector<LoaderPtr>& 			store);
            
            void
			finalize();
		};
	}
}
