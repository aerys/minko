package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.TriangleCulling;
	
	import flash.geom.Rectangle;

	public class ShaderSettings
	{
		use namespace minko_render;
		
		private var _numUses								: uint			= 0;
		private var _signature								: Signature		= null;
		
		private var _enabled								: Boolean		= true;
		
		private var _depthTest								: uint			= 0;
		private var _compareMode							: String		= null;
		private var _priority								: Number		= 0.;

		private var _blending								: uint			= 0;
		private var _blendingSource							: String		= null;
		private var _blendingDest							: String		= null;
		
		private var _triangleCulling						: uint			= 0;
		private var _triangleCullingStr						: String		= null;
		
		private var _renderTarget							: RenderTarget	= null;
		private var _enableDepthWrite						: Boolean		= true;
		private var _rectangle								: Rectangle		= null;
		
		private var _depthSortDrawCalls						: Boolean		= false;
		
		private var _stencilTriangleFace					: String		= null;
		private var _stencilCompareMode						: String		= null;
		private var _stencilActionOnBothPass				: String		= null;
		private var _stencilActionOnDepthFail				: String		= null;
		private var _stencilActionOnDepthPassStencilFail	: String		= null;
		private var _stencilReferenceValue					: uint			= 0;
		private var _stencilReadMask						: uint			= 255;
		private var _sentcilWriteMask						: uint			= 255;
		
		minko_render function get signature() : Signature
		{
			return _signature;
		}
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(v : Boolean) : void
		{
			_enabled = v;
		}
		
		public function get priority() : Number
		{
			return _priority;
		}
		public function set priority(v : Number) : void
		{
			_priority = v;
		}
		
		public function get renderTarget() : RenderTarget
		{
			return _renderTarget;
		}
		public function set renderTarget(value : RenderTarget) : void
		{
			_renderTarget = value;
		}
		
		public function get scissorRectangle() : Rectangle
		{
			return _rectangle;
		}
		public function set scissorRectangle(value : Rectangle) : void
		{
			_rectangle = value;
		}
		
		public function get depthWriteEnabled() : Boolean
		{
			return _enableDepthWrite;
		}
		public function set depthWriteEnabled(value : Boolean) : void
		{
			_enableDepthWrite = value;
		}
		
		public function get depthTest() : uint
		{
			return _depthTest;
		}
		public function set depthTest(value : uint) : void
		{
			_depthTest = value;
			
			var index : int = DepthTest.FLAGS.indexOf(value);
			
			if (index < 0)
				throw new Error("Invalid depth test value: " + value);
			
			_compareMode = DepthTest.STRINGS[index];
		}
		
		/**
		 * The blending factors to use. This value must be one of the values
		 * in the Blending enumeration or can be created by associating the values
		 * provided by the BlendingSource and BlendingDestination enumerations. 
		 * @return 
		 * 
		 * @see aerys.minko.type.enum.Blending
		 * @see aerys.minko.type.enum.BlendingSource
		 * @see aerys.minko.type.enum.BlendingDestination
		 */
		public function get blending() : uint
		{
			return _blending;
		}
		public function set blending(value : uint) : void
		{
			_blending = value;
			_blendingSource = Blending.STRINGS[int(value & 0xffff)];
			_blendingDest	= Blending.STRINGS[int(value >>> 16)]
		}
		
		public function get triangleCulling() : uint
		{
			return _triangleCulling;
		}
		public function set triangleCulling(value : uint) : void
		{
			_triangleCulling = value;
			_triangleCullingStr = TriangleCulling.STRINGS[value];
		}
		
		public function get depthSortDrawCalls() : Boolean
		{
			return _depthSortDrawCalls;
		}
		public function set depthSortDrawCalls(value : Boolean) : void
		{
			_depthSortDrawCalls = value;
		}
		
		public function ShaderSettings(signature : Signature)
		{
			_numUses			= 0;
			_signature			= signature;
			
			depthTest			= DepthTest.LESS;
			blending			= Blending.NORMAL;
			triangleCulling		= TriangleCulling.BACK;
			renderTarget		= null;
			depthWriteEnabled	= true;
			scissorRectangle	= null;
		}
		
		public function retain() : void
		{
			++_numUses;
		}
		
		public function release() : void
		{
			--_numUses;
		}
		
		minko_render function clone(signature : Signature) : ShaderSettings
		{
			var clone : ShaderSettings = new ShaderSettings(signature);
			
			clone.depthTest = depthTest;
			clone.blending = blending;
			clone.triangleCulling = triangleCulling;
			clone.renderTarget = renderTarget;
			clone.depthWriteEnabled	= depthWriteEnabled;
			clone.scissorRectangle = scissorRectangle;
			clone.depthSortDrawCalls = depthSortDrawCalls;
			
			return clone;
		}
		
		public function prepareContext(context 		: Context3DResource,
									   backBuffer	: RenderTarget,
									   previous		: ShaderSettings) : void
		{
			if (!previous || previous._renderTarget != _renderTarget)
			{
				var rt 	: RenderTarget 	= _renderTarget || backBuffer;
				
				if (rt && rt.textureResource)
					context.setRenderToTexture(
						rt.textureResource.getNativeTexture(context),
						rt.useDepthAndStencil,
						rt.antiAliasing,
						rt.surfaceSelector
					);
				else
					context.setRenderToBackBuffer();
				
				var color 	: uint 	= rt.backgroundColor;
				
				context.clear(
					((color >> 24) & 0xff) / 255.,
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.
				);
			}
			
			context.setScissorRectangle(_rectangle);
			context.setDepthTest(_enableDepthWrite, _compareMode);
			
			context.setBlendFactors(_blendingSource, _blendingDest);
			context.setCulling(_triangleCullingStr);
		}
		
	}
}