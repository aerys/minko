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
            float				    _priority;
		    Blending::Source		_blendingSourceFactor;
		    Blending::Destination	_blendingDestinationFactor;
		    bool					_depthMask;
		    CompareMode				_depthFunc;
            TriangleCulling         _triangleCulling;
            SamplerStates           _samplerStates;

	    public:
		    inline static
		    Ptr
		    create(const SamplerStates&     samplerSates,
				   const float				priority					= 0.f,
				   Blending::Source			blendingSourceFactor		= Blending::Source::ONE,
				   Blending::Destination	blendingDestinationFactor	= Blending::Destination::ZERO,
				   bool						depthMask					= true,
				   CompareMode				depthFunc					= CompareMode::LESS,
                   TriangleCulling          triangleCulling             = TriangleCulling::BACK)
		    {
			    return std::shared_ptr<States>(new States(
                    samplerSates,
                    priority,
                    blendingSourceFactor,
                    blendingDestinationFactor,
                    depthMask,
                    depthFunc,
                    triangleCulling
                ));
		    }

            inline
            float
            priority()
            {
                return _priority;
            }

            inline
            Blending::Source
            blendingSourceFactor()
            {
                return _blendingSourceFactor;
            }

            inline
            Blending::Destination
            blendingDestinationFactor()
            {
                return _blendingDestinationFactor;
            }

            inline
            bool
            depthMask()
            {
                return _depthMask;
            }

            inline
            CompareMode
            depthFun()
            {
                return _depthFunc;
            }

            inline
            TriangleCulling
            triangleCulling()
            {
                return _triangleCulling;
            }

            inline
            const SamplerStates&
            samplers()
            {
                return _samplerStates;
            }

	    private:
		    States(const SamplerStates&     samplerSates,
				   const float			    priority,
				   Blending::Source		    blendingSourceFactor,
				   Blending::Destination    blendingDestinationFactor,
				   bool					    depthMask,
				   CompareMode			    depthFunc,
                   TriangleCulling          triangleCulling) :
                _samplerStates(samplerSates),
                _priority(priority),
                _blendingSourceFactor(blendingSourceFactor),
                _blendingDestinationFactor(blendingDestinationFactor),
                _depthMask(depthMask),
                _depthFunc(depthFunc),
                _triangleCulling(triangleCulling)
		    {
		    }
        };
	}
}
