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
#include "minko/render/Blending.hpp"

namespace Json
{
	class Value;
}

namespace minko
{
	namespace file
	{
		class EffectParser :
			public std::enable_shared_from_this<EffectParser>,
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<EffectParser>	Ptr;

		private:
			typedef std::shared_ptr<Loader>			LoaderPtr;

		private:
			static std::unordered_map<std::string, unsigned int>		_blendFactorMap;
			static std::unordered_map<std::string, render::CompareMode>	_depthFuncMap;

			std::shared_ptr<render::Effect>								_effect;
			std::string													_effectName;
			
			float														_defaultPriority;
			render::Blending::Source									_defaultBlendSrcFactor;
			render::Blending::Destination								_defaultBlendDstFactor;
			bool														_defaultDepthMask;
			render::CompareMode											_defaultDepthFunc;
            render::TriangleCulling                                     _defaultTriangleCulling;
			std::unordered_map<std::string, std::string>				_defaultAttributeBindings;
			std::unordered_map<std::string, std::string>				_defaultUniformBindings;
			std::unordered_map<std::string, std::string>				_defaultStateBindings;
			std::unordered_map<std::string, std::string>				_defaultMacroBindings;
            std::unordered_map<std::string, render::SamplerState>       _defaultSamplerStates;

			unsigned int												_numDependencies;
			unsigned int												_numLoadedDependencies;
			std::shared_ptr<AssetsLibrary>								_assetsLibrary;

			std::string													_dependenciesCode;

			std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>		_loaderCompleteSlots;
			std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>		_loaderErrorSlots;

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
				  std::shared_ptr<Options>			options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetsLibrary>	assetsLibrary);

			void
			finalize();

		private:
			EffectParser();

			void
			parseDefaultValues(Json::Value& root);

			void
			parsePasses(Json::Value& root, std::shared_ptr<file::Options> options);

			void
			parseBindings(Json::Value&									contextNode,
						  std::unordered_map<std::string, std::string>&	attributeBindings,
						  std::unordered_map<std::string, std::string>&	uniformBindings,
						  std::unordered_map<std::string, std::string>&	stateBindings,
						  std::unordered_map<std::string, std::string>&	macroBindings);

			void
			parseBlendMode(Json::Value&						contextNode,
						   render::Blending::Source&		srcFactor,
						   render::Blending::Destination&	dstFactor);

			void
			parseDepthTest(Json::Value&			contextNode,
						   bool&				depthMask,
						   render::CompareMode&	depthFunc);

            void
            parseTriangleCulling(Json::Value&               contextNode,
                                 render::TriangleCulling&   triangleCulling);

            void
            parseSamplerStates(Json::Value&                                             contextNode,
                               std::unordered_map<std::string, render::SamplerState>&   samplerStates);

            std::shared_ptr<render::Texture>
            parseTarget(Json::Value& contextNode, std::shared_ptr<render::AbstractContext> context, std::string& name);

			void
			parseDependencies(Json::Value& root, const std::string& filename, std::shared_ptr<file::Options> options);

			void
			dependencyCompleteHandler(std::shared_ptr<Loader> loader);

			void
			dependencyErrorHandler(std::shared_ptr<Loader> loader);

			static
			std::unordered_map<std::string, unsigned int>
			initializeBlendFactorMap();

			static
			std::unordered_map<std::string, render::CompareMode>
			initializeDepthFuncMap();
		};
	}
}
