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
#include "minko/scene/Layout.hpp"

namespace minko
{
    namespace render
    {
	    class States
	    {
	    public:
		    typedef std::shared_ptr<States>                                 Ptr;
            typedef std::unordered_map<std::string, render::SamplerState>   SamplerStates;
			typedef std::shared_ptr<math::Vector4>							Vector4Ptr;
			typedef std::shared_ptr<render::AbstractTexture>				AbstractTexturePtr;

        private:
			Layouts						_layouts;
            float				        _priority;
			bool						_zsorted;
		    Blending::Source		    _blendingSourceFactor;
		    Blending::Destination	    _blendingDestinationFactor;
			bool						_colorMask;
		    bool					    _depthMask;
		    CompareMode		    		_depthFunc;
            TriangleCulling             _triangleCulling;
			CompareMode					_stencilFunction;
			int							_stencilReference;
			uint						_stencilMask;
			StencilOperation			_stencilFailOp;
			StencilOperation			_stencilZFailOp;
			StencilOperation			_stencilZPassOp;
			bool						_scissorTest;
			ScissorBox					_scissorBox;
            SamplerStates               _samplerStates;
            AbstractTexturePtr		    _target;

		public:
		    inline static
		    Ptr
		    create(const SamplerStates&     samplerStates,
				   Layouts					layouts						= scene::Layout::Group::DEFAULT,
				   float					priority					= 0.f,
				   bool						zSorted						= false,
				   Blending::Source			blendingSourceFactor		= Blending::Source::ONE,
				   Blending::Destination	blendingDestinationFactor	= Blending::Destination::ZERO,
				   bool						colorMask					= true,
				   bool						depthMask					= true,
				   CompareMode				depthFunc					= CompareMode::LESS,
                   TriangleCulling          triangleCulling             = TriangleCulling::BACK,
				   CompareMode				stencilFunction				= CompareMode::ALWAYS,
				   int						stencilRef					= 0,
				   uint						stencilMask					= 0x1,
				   StencilOperation			stencilFailOp				= StencilOperation::KEEP, 
				   StencilOperation			stencilZFailOp				= StencilOperation::KEEP, 
				   StencilOperation			stencilZPassOp				= StencilOperation::KEEP, 
				   bool						scissorTest					= false,
				   const ScissorBox&		scissorBox					= ScissorBox(),
                   AbstractTexturePtr		target                      = nullptr)
		    {
			    return std::shared_ptr<States>(new States(
                    samplerStates,
					layouts,
                    priority,
					zSorted,
                    blendingSourceFactor,
                    blendingDestinationFactor,
					colorMask,
                    depthMask,
                    depthFunc,
                    triangleCulling,
					stencilFunction,
					stencilRef,
					stencilMask,
					stencilFailOp,
					stencilZFailOp,
					stencilZPassOp,
					scissorTest,
					scissorBox,
                    target
                ));
		    }

		    inline static
		    Ptr
		    create(Ptr states)
		    {
		    	return std::shared_ptr<States>(new States(
                    states->_samplerStates,
					states->_layouts,
                    states->_priority,
					states->_zsorted,
                    states->_blendingSourceFactor,
                    states->_blendingDestinationFactor,
					states->_colorMask,
                    states->_depthMask,
                    states->_depthFunc,
                    states->_triangleCulling,
					states->_stencilFunction,
					states->_stencilReference,
					states->_stencilMask,
					states->_stencilFailOp,
					states->_stencilZFailOp,
					states->_stencilZPassOp,
					states->_scissorTest,
					states->_scissorBox,
                    states->_target
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
			Layouts
			layouts() const
			{
				return _layouts;
			}

            inline
            float
            priority() const
            {
                return _priority;
            }

            inline
            void
            priority(float priority)
            {
            	_priority = priority;
            }

			inline
			bool
			zSorted() const
			{
				return _zsorted;
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
			colorMask() const
			{
				return _colorMask;
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
			StencilOperation
			stencilFailOperation() const
			{
				return _stencilFailOp;
			}

			inline
			StencilOperation
			stencilDepthFailOperation() const
			{
				return _stencilZFailOp;
			}

			inline
			StencilOperation
			stencilDepthPassOperation() const
			{
				return _stencilZPassOp;
			}

			inline
			bool
			scissorTest() const
			{
				return _scissorTest;
			}

			inline
			const ScissorBox&
			scissorBox() const
			{
				return _scissorBox;
			}
			
            inline
            const SamplerStates&
            samplers() const
            {
                return _samplerStates;
            }

            inline
            AbstractTexturePtr
            target() const
            {
                return _target;
            }

	    private:
		    States(const SamplerStates&     samplerSates,
				   Layouts					layouts,
				   float				    priority,
				   bool						zSorted,
				   Blending::Source		    blendingSourceFactor,
				   Blending::Destination    blendingDestinationFactor,
				   bool						colorMask,
				   bool					    depthMask,
				   CompareMode			    depthFunc,
                   TriangleCulling          triangleCulling,
				   CompareMode				stencilFunc,
				   int						stencilRef,
				   uint						stencilMask,
				   StencilOperation			stencilFailOp,
				   StencilOperation			stencilZFailOp,
				   StencilOperation			stencilZPassOp,
				   bool						scissorTest,
				   const ScissorBox&		scissorBox,
                   AbstractTexturePtr		target) :
                _samplerStates(samplerSates),
				_layouts(layouts),
                _priority(priority),
				_zsorted(zSorted),
                _blendingSourceFactor(blendingSourceFactor),
                _blendingDestinationFactor(blendingDestinationFactor),
				_colorMask(colorMask),
                _depthMask(depthMask),
                _depthFunc(depthFunc),
                _triangleCulling(triangleCulling),
				_stencilFunction(stencilFunc),
				_stencilReference(stencilRef),
				_stencilMask(stencilMask),
				_stencilFailOp(stencilFailOp),
				_stencilZFailOp(stencilZFailOp),
				_stencilZPassOp(stencilZPassOp),
				_scissorTest(scissorTest),
				_scissorBox(scissorBox),
                _target(target)
		    {
		    }
        };
	}
}
