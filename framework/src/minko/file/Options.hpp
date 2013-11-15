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

		public:
			typedef std::shared_ptr<Options>									Ptr;
			typedef std::function<ProviderPtr(const std::string&, ProviderPtr)> MaterialFunction;
			typedef std::function<AbsLoaderPtr(const std::string&)>				LoaderFunction;

		private:
			std::shared_ptr<render::AbstractContext>	_context;
			std::set<std::string>						_includePaths;
			std::list<std::string>						_platforms;
			std::list<std::string>						_userFlags;

            bool                                        _generateMipMaps;
            std::shared_ptr<render::Effect>             _effect;
			std::shared_ptr<data::Provider>				_material;
			MaterialFunction							_materialFunction;
			LoaderFunction								_loaderFunction;

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
			std::list<std::string>&
			userFlags()
			{
				return _userFlags;
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
			std::shared_ptr<data::Provider>
			material()
			{
				return _material;
			}

			inline
			void
			material(std::shared_ptr<data::Provider> material)
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

		private:
			Options(std::shared_ptr<render::AbstractContext> context);

			void
			initializePlatforms();

			void
			initializeUserFlags();
		};
	}
}
