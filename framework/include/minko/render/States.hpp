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
			typedef std::shared_ptr<render::AbstractTexture>				AbstractTexturePtr;

        public:
            static const float				    DEFAULT_PRIORITY;
            static const bool					DEFAULT_ZSORTED;
            static const Blending::Source		DEFAULT_BLENDING_SOURCE;
            static const Blending::Destination	DEFAULT_BLENDING_DESTINATION;
            static const bool					DEFAULT_COLOR_MASK;
            static const bool					DEFAULT_DEPTH_MASK;
            static const CompareMode		    DEFAULT_DEPTH_FUNC;
            static const TriangleCulling        DEFAULT_TRIANGLE_CULLING;
            static const CompareMode			DEFAULT_STENCIL_FUNCTION;
            static const int					DEFAULT_STENCIL_REFERENCE;
            static const uint					DEFAULT_STENCIL_MASK;
            static const StencilOperation		DEFAULT_STENCIL_FAIL_OP;
            static const StencilOperation		DEFAULT_STENCIL_ZFAIL_OP;
            static const StencilOperation		DEFAULT_STENCIL_ZPASS_OP;
            static const bool					DEFAULT_SCISSOR_TEST;
            static const ScissorBox 			DEFAULT_SCISSOR_BOX;

        private:
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
				   float					priority					= DEFAULT_PRIORITY,
				   bool						zSorted						= DEFAULT_ZSORTED,
				   Blending::Source			blendingSourceFactor		= DEFAULT_BLENDING_SOURCE,
				   Blending::Destination	blendingDestinationFactor	= DEFAULT_BLENDING_DESTINATION,
				   bool						colorMask					= DEFAULT_COLOR_MASK,
				   bool						depthMask					= DEFAULT_DEPTH_MASK,
				   CompareMode				depthFunc					= DEFAULT_DEPTH_FUNC,
                   TriangleCulling          triangleCulling             = DEFAULT_TRIANGLE_CULLING,
				   CompareMode				stencilFunction				= DEFAULT_STENCIL_FUNCTION,
				   int						stencilRef					= DEFAULT_STENCIL_REFERENCE,
				   uint						stencilMask					= DEFAULT_STENCIL_MASK,
				   StencilOperation			stencilFailOp				= DEFAULT_STENCIL_FAIL_OP, 
				   StencilOperation			stencilZFailOp				= DEFAULT_STENCIL_ZFAIL_OP, 
				   StencilOperation			stencilZPassOp				= DEFAULT_STENCIL_ZPASS_OP, 
				   bool						scissorTest					= DEFAULT_SCISSOR_TEST,
				   const ScissorBox&		scissorBox					= DEFAULT_SCISSOR_BOX,
                   AbstractTexturePtr		target                      = nullptr)
		    {
			    return std::shared_ptr<States>(new States(
                    samplerStates,
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
