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

namespace minko
{
	namespace file
	{
		class Options
		{
		private:
			typedef std::shared_ptr<AbstractLoader>								AbsLoaderPtr;
			typedef std::shared_ptr<data::Provider>								ProviderPtr;
			typedef std::shared_ptr<material::Material>							MaterialPtr;
			typedef std::shared_ptr<scene::Node>								NodePtr;
			typedef std::shared_ptr<render::Effect>								EffectPtr;

		public:
			typedef std::shared_ptr<Options>									Ptr;
			typedef std::function<MaterialPtr(const std::string&, MaterialPtr)> MaterialFunction;
			typedef std::function<AbsLoaderPtr(const std::string&)>				LoaderFunction;
			typedef std::function<const std::string(const std::string&)>		UriFunction;
			typedef std::function<NodePtr(NodePtr)>								NodeFunction;
			typedef std::function<EffectPtr(EffectPtr)>							EffectFunction;

		private:
			std::shared_ptr<render::AbstractContext>	_context;
			std::set<std::string>						_includePaths;
			std::list<std::string>						_platforms;

            bool                                        _generateMipMaps;
            std::shared_ptr<render::Effect>             _effect;
			MaterialPtr									_material;
			MaterialFunction							_materialFunction;
			LoaderFunction								_loaderFunction;
			UriFunction									_uriFunction;
			NodeFunction								_nodeFunction;
			EffectFunction								_effectFunction;

		public:
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				return std::shared_ptr<Options>(new Options(context));
			}

			inline static
			Ptr
			create(Ptr options)
			{
				auto opt = std::shared_ptr<Options>(new Options(options->_context));

				opt->_includePaths.insert(options->_includePaths.begin(), options->_includePaths.end());
                opt->_generateMipMaps = options->_generateMipMaps;
                opt->_effect = options->_effect;
				opt->_materialFunction = options->_materialFunction;
				opt->_loaderFunction = options->_loaderFunction;
				opt->_uriFunction = options->_uriFunction;
				opt->_nodeFunction = options->_nodeFunction;

				return opt;
			}

			inline
			std::shared_ptr<render::AbstractContext>
			context()
			{
				return _context;
			}
			
			inline
			std::set<std::string>&
			includePaths()
			{
				return _includePaths;
			}

			inline
			std::list<std::string>&
			platforms()
			{
				return _platforms;
			}

            inline
            bool
            generateMipmaps()
            {
                return _generateMipMaps;
            }

            inline
            void
            generateMipmaps(bool generateMipmaps)
            {
                _generateMipMaps = generateMipmaps;
            }

            inline
            std::shared_ptr<render::Effect>
            effect()
            {
                return _effect;
            }

            inline
            void
            effect(std::shared_ptr<render::Effect> effect)
            {
                _effect = effect;
            }

			inline
			MaterialPtr
			material()
			{
				return _material;
			}

			inline
			void
			material(MaterialPtr material)
			{
				_material = material;
			}

			inline
			const MaterialFunction&
			materialFunction() const
			{
				return _materialFunction;
			}

			inline
			const LoaderFunction&
			loaderFunction() const
			{
				return _loaderFunction;
			}

			inline
			void
			materialFunction(const MaterialFunction& func)
			{
				_materialFunction = func;
			}

			inline
			const UriFunction&
			uriFunction()
			{
				return _uriFunction;
			}

			inline
			void
			uriFunction(const UriFunction& func)
			{
				_uriFunction = func;
			}

			inline
			const NodeFunction&
			nodeFunction()
			{
				return _nodeFunction;
			}

			inline
			void
			nodeFunction(const NodeFunction& func)
			{
				_nodeFunction = func;
			}

			inline
			const EffectFunction&
			effectFunction()
			{
				return _effectFunction;
			}

			inline
			void
			effectFunction(const EffectFunction& func)
			{
				_effectFunction = func;
			}

		private:
			Options(std::shared_ptr<render::AbstractContext> context);

			void
			initializePlatforms();
		};
	}
}
