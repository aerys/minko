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

#include "minko/Signal.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/Shader.hpp"
#include "minko/scene/Layout.hpp"

namespace Json
{
	class Value;
}

namespace minko
{
	namespace file
	{
		class EffectParser :
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<EffectParser>	Ptr;

		private:
			enum class GLSLBlockType
			{
				TEXT,
				FILE
			};

		private:
			typedef std::shared_ptr<render::AbstractTexture>	AbstractTexturePtr;

			union UniformNumericValue
			{
				int		intValue;
				float	floatValue;
			};
			
			struct UniformValue
			{
				std::vector<UniformNumericValue>	numericValue;
				AbstractTexturePtr					textureValue;
			};

			enum class UniformType
			{
				UNSET,
				INT,
				FLOAT,
				TEXTURE
			};

			typedef std::shared_ptr<Loader>										LoaderPtr;
			typedef std::shared_ptr<render::Effect>								EffectPtr;
			typedef std::shared_ptr<render::Pass>								PassPtr;
			typedef std::shared_ptr<render::Shader>								ShaderPtr;
			typedef std::unordered_map<std::string, AbstractTexturePtr>			TexturePtrMap;
			typedef std::pair<UniformType, UniformValue>						UniformTypeAndValue;
			typedef std::unordered_map<std::string, UniformTypeAndValue>		UniformValues;
			typedef std::pair<GLSLBlockType, std::string> 						GLSLBlock;
			typedef std::forward_list<GLSLBlock> 								GLSLBlockList;
			typedef std::shared_ptr<GLSLBlockList>								GLSLBlockListPtr;

		private:
			static std::unordered_map<std::string, unsigned int>				_blendFactorMap;
			static std::unordered_map<std::string, render::CompareMode>			_compareFuncMap;
			static std::unordered_map<std::string, render::StencilOperation>	_stencilOpMap;
			static std::unordered_map<std::string, float>						_priorityMap;

		private:
            std::string															_filename;
			std::string															_resolvedFilename;
			std::shared_ptr<file::Options>										_options;
			std::shared_ptr<render::Effect>										_effect;
			std::string															_effectName;
			
			std::string															_defaultTechnique;
			std::shared_ptr<render::States>										_defaultStates;

            data::BindingMap													_defaultAttributeBindings;
			data::BindingMap													_defaultUniformBindings;
			data::BindingMap													_defaultStateBindings;
			data::MacroBindingMap                              					_defaultMacroBindings;
			UniformValues														_defaultUniformValues;


			std::shared_ptr<AssetLibrary>										_assetLibrary;
			unsigned int														_numDependencies;
			unsigned int														_numLoadedDependencies;

			std::unordered_map<ShaderPtr, GLSLBlockListPtr>						_glslBlocks;

			std::vector<PassPtr>												_globalPasses;
			std::unordered_map<std::string, AbstractTexturePtr>					_globalTargets;
			std::unordered_map<std::string, TexturePtrMap>						_techniqueTargets;
			std::unordered_map<std::string, std::vector<PassPtr>>				_techniquePasses;
			std::unordered_map<std::string, std::string>						_techniqueFallback;
			
			std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>				_loaderCompleteSlots;
			std::unordered_map<LoaderPtr, Signal<LoaderPtr, const ParserError&>::Slot>				_loaderErrorSlots;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<EffectParser>(new EffectParser());
			}

			inline
			std::shared_ptr<render::Effect>
			effect()
			{
				return _effect;
			}

			inline
			const std::string&
			effectName()
			{
				return _effectName;
			}

			void
			parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetLibrary>		assetLibrary);

		private:
			EffectParser();

			std::shared_ptr<render::States>
			parseRenderStates(const Json::Value&						root,
							  std::shared_ptr<render::AbstractContext>	context,
							  TexturePtrMap&							targets,
							  std::shared_ptr<render::States>			defaultStates,
							  float										priorityOffset);

			void
			parseDefaultValues(const Json::Value& root);

			void
			parsePasses(const Json::Value& 							root,
						const std::string& 							resolvedFilename,
						std::shared_ptr<file::Options> 				options,
						std::shared_ptr<render::AbstractContext>	context,
						std::vector<PassPtr>&						passes,
						TexturePtrMap&								targets,
						data::BindingMap&							defaultAttributeBindings,
						data::BindingMap&							defaultUniformBindings,
						data::BindingMap&							defaultStateBindings,
						data::MacroBindingMap&						defaultMacroBindings,
						std::shared_ptr<render::States>				defaultStates,
						UniformValues&								defaultUniformDefaultValues);

			void
			setUniformDefaultValueOnPass(PassPtr					pass,
										 const std::string&			name,
										 UniformType				type,
										 UniformValue&				value);

			std::shared_ptr<render::Shader>
			parseShader(const Json::Value& 				shaderNode,
						const std::string&				resolvedFilename,
						std::shared_ptr<file::Options>  options,
						render::Shader::Type 			type);

			void
			parseGLSL(std::string 						glsl,
					  std::shared_ptr<file::Options>	options,
					  GLSLBlockListPtr 					blocks,
	 				  GLSLBlockList::iterator 			fileBlock);

			void
			loadGLSLDependencies(GLSLBlockListPtr				blocks,
								 std::shared_ptr<file::Options> options);

			void
			glslIncludeCompleteHandler(LoaderPtr 			    loader,
									   GLSLBlockListPtr 		blocks,
                                       GLSLBlockList::iterator 	fileBlock,
                                       const std::string&       filename);

			inline
			void
			parseBindingNameAndSource(const Json::Value&	contextNode, 
									  std::string&			name, 
									  data::BindingSource&	source)
			{
				RegexPtr regex = nullptr;
				parseBindingNameAndSource(contextNode, name, source, regex);
			}

			void
			parseBindingNameAndSource(const Json::Value&	contextNode, 
									  std::string&			name, 
									  data::BindingSource&	source,
									  RegexPtr&				regexp);

			void
			parseBindings(const Json::Value&		contextNode,
						  data::BindingMap&			attributeBindings,
						  data::BindingMap&			uniformBindings,
						  data::BindingMap&			stateBindings,
						  data::MacroBindingMap&	macroBindings,
						  UniformValues&			uniformDefaultValues);

			void
			parseMacroBindings(const Json::Value&		contextNode,
							   data::MacroBindingMap&	macroBindings);

			void
			parseUniformBindings(const Json::Value&		contextNode,
							 	 data::BindingMap&		uniformBindings,
								 UniformValues&			uniformDefaultValues);

			void
			parseUniformDefaultValues(const Json::Value&	contextNode,
									  UniformTypeAndValue&	uniformTypeAndValue);

			void
			loadTexture(const std::string&				textureFilename,
						UniformTypeAndValue&			uniformTypeAndValue,
						std::shared_ptr<file::Options>	options);

			float
			parsePriority(const Json::Value&, float defaultPriority);

			void
			parseBlendMode(const Json::Value&				contextNode,
						   render::Blending::Source&		srcFactor,
						   render::Blending::Destination&	dstFactor);

			void
			parseZSort(const Json::Value&	contextNode,
					   bool& zSorted) const;

			void
			parseColorMask(const Json::Value&	contextNode,
						   bool& colorMask) const;

			void
			parseDepthTest(const Json::Value&	contextNode,
						   bool&				depthMask,
						   render::CompareMode&	depthFunc);

            void
            parseTriangleCulling(const Json::Value&         contextNode,
                                 render::TriangleCulling&   triangleCulling);

            void
            parseSamplerStates(const Json::Value&                                       contextNode,
                               std::unordered_map<std::string, render::SamplerState>&   samplerStates);

			void
			parseStencilState(const Json::Value&, 
							  render::CompareMode& stencilFunc, 
							  int& stencilRef, 
							  uint& stencilMask, 
							  render::StencilOperation& stencilFailOp,
							  render::StencilOperation& stencilZFailOp,
							  render::StencilOperation& stencilZPassOp) const;

			void
			parseStencilOperations(const Json::Value&, 
								   render::StencilOperation& stencilFailOp,
								   render::StencilOperation& stencilZFailOp,
								   render::StencilOperation& stencilZPassOp) const;

			void
			parseScissorTest(const Json::Value&,
							 bool&					scissorTest,
							 render::ScissorBox&	scissorBox) const;

            AbstractTexturePtr
            parseTarget(const Json::Value&                          contextNode,
                        std::shared_ptr<render::AbstractContext>    context,
                        TexturePtrMap&								targets);

			void
			parseTechniques(const Json::Value&							root,
							const std::string&							filename,
							std::shared_ptr<file::Options>				options,
							std::shared_ptr<render::AbstractContext>	context);

			bool
			parseConfiguration(const Json::Value&	root);

			void
			dependencyCompleteHandler(LoaderPtr loader);


			void
            dependencyErrorHandler(LoaderPtr loader, const ParserError& error, const std::string& filename);

			void
            textureErrorHandler(LoaderPtr loader);

			std::string
			concatenateIncludes(std::vector<LoaderPtr>& store);

			static
			std::unordered_map<std::string, unsigned int>
			initializeBlendFactorMap();

			std::string
			concatenateGLSLBlocks(GLSLBlockListPtr blocks);

			static
			std::unordered_map<std::string, render::CompareMode>
			initializeCompareFuncMap();

			static
			std::unordered_map<std::string, render::StencilOperation>
			initializeStencilOperationMap();

			static
			std::unordered_map<std::string, float>
			initializePriorityMap();

			static
			float
			priority(const std::string&);

            void
            definePlatform();

			void
			finalize();
		};
	}
}
