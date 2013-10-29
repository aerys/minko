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

#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/render/Texture.hpp"

namespace minko
{
    namespace render
    {
	    class States
	    {
	    public:
		    typedef std::shared_ptr<States>                                 Ptr;
            typedef std::unordered_map<std::string, render::SamplerState>   SamplerStates;

        private:
            float				        _priority;
		    Blending::Source		    _blendingSourceFactor;
		    Blending::Destination	    _blendingDestinationFactor;
		    bool					    _depthMask;
		    CompareMode		    		_depthFunc;
            TriangleCulling             _triangleCulling;
			CompareMode					_stencilFunction;
			int							_stencilReference;
			uint						_stencilMask;
			StencilOperations			_stencilOperations;
            SamplerStates               _samplerStates;
            std::shared_ptr<Texture>    _target;

	    public:
		    inline static
		    Ptr
		    create(const SamplerStates&     samplerSates,
				   const float				priority					= 0.f,
				   Blending::Source			blendingSourceFactor		= Blending::Source::ONE,
				   Blending::Destination	blendingDestinationFactor	= Blending::Destination::ZERO,
				   bool						depthMask					= true,
				   CompareMode				depthFunc					= CompareMode::LESS,
                   TriangleCulling          triangleCulling             = TriangleCulling::BACK,
				   CompareMode				stencilFunc					= CompareMode::ALWAYS,
				   int						stencilRef					= 0,
				   uint						stencilMask					= 0x1,
				   const StencilOperations&	stencilOps					= std::make_tuple(StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP),
                   std::shared_ptr<Texture> target                      = nullptr)
		    {
			    return std::shared_ptr<States>(new States(
                    samplerSates,
                    priority,
                    blendingSourceFactor,
                    blendingDestinationFactor,
                    depthMask,
                    depthFunc,
                    triangleCulling,
					stencilFunc,
					stencilRef,
					stencilMask,
					stencilOps,
                    target
                ));
		    }

			inline static
		    Ptr
		    create()
		    {
				SamplerStates states;

			    return create(states);
		    }

            inline
            float
            priority() const
            {
                return _priority;
            }

            inline
            Blending::Source
            blendingSourceFactor() const
            {
                return _blendingSourceFactor;
            }

            inline
            Blending::Destination
            blendingDestinationFactor() const
            {
                return _blendingDestinationFactor;
            }

            inline
            bool
            depthMask() const
            {
                return _depthMask;
            }

            inline
            CompareMode
            depthFunc() const
            {
                return _depthFunc;
            }

            inline
            TriangleCulling
            triangleCulling() const
            {
                return _triangleCulling;
            }

			inline
			CompareMode
			stencilFunction() const
			{
				return _stencilFunction;
			}

			inline
			int
			stencilReference() const
			{
				return _stencilReference;
			}

			inline
			uint
			stencilMask() const
			{
				return _stencilMask;
			}

			inline
			const StencilOperations&
			stencilOperations() const
			{
				return _stencilOperations;
			}

            inline
            const SamplerStates&
            samplers() const
            {
                return _samplerStates;
            }

            inline
            std::shared_ptr<Texture>
            target() const
            {
                return _target;
            }

	    private:
		    States(const SamplerStates&     samplerSates,
				   const float			    priority,
				   Blending::Source		    blendingSourceFactor,
				   Blending::Destination    blendingDestinationFactor,
				   bool					    depthMask,
				   CompareMode			    depthFunc,
                   TriangleCulling          triangleCulling,
				   CompareMode				stencilFunc,
				   int						stencilRef,
				   uint						stencilMask,
				   const StencilOperations&	stencilOps,
                   std::shared_ptr<Texture> target) :
                _samplerStates(samplerSates),
                _priority(priority),
                _blendingSourceFactor(blendingSourceFactor),
                _blendingDestinationFactor(blendingDestinationFactor),
                _depthMask(depthMask),
                _depthFunc(depthFunc),
                _triangleCulling(triangleCulling),
				_stencilFunction(stencilFunc),
				_stencilReference(stencilRef),
				_stencilMask(stencilMask),
				_stencilOperations(stencilOps),
                _target(target)
		    {
		    }
        };
	}
}
