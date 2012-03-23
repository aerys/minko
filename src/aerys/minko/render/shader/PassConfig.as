package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.TriangleCulling;
	
	import flash.display3D.Context3D;
	import flash.geom.Rectangle;

	public class PassConfig
	{
		use namespace minko_render;
		
		private var _numUses					: uint			= 0;
		private var _signature					: Signature		= null;
		
		private var _enabled					: Boolean		= true;
		
		private var _depthTest					: uint			= 0;
		private var _compareMode				: String		= null;
		private var _priority					: Number		= 0.;

		private var _blending					: uint			= 0;
		private var _blendingSource				: String		= null;
		private var _blendingDest				: String		= null;
		
		private var _triangleCulling			: uint			= 0;
		private var _triangleCullingStr			: String		= null;
		
		private var _renderTarget				: RenderTarget	= null;
		private var _enableDepthWrite			: Boolean		= true;
		private var _rectangle					: Rectangle		= null;
		
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
		
		public function get enableDepthWrite() : Boolean
		{
			return _enableDepthWrite;
		}
		public function set enableDepthWrite(value : Boolean) : void
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
		
		public function PassConfig(signature : Signature)
		{
			_numUses			= 0;
			_signature			= signature;
			
			depthTest			= DepthTest.LESS;
			blending			= Blending.NORMAL;
			triangleCulling		= TriangleCulling.BACK;
			renderTarget		= null;
			enableDepthWrite	= true;
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
		
		minko_render function clone(signature : Signature) : PassConfig
		{
			var clone : PassConfig = new PassConfig(signature);
			
			clone.depthTest			= depthTest;
			clone.blending			= blending;
			clone.triangleCulling	= triangleCulling;
			clone.renderTarget		= renderTarget;
			clone.enableDepthWrite	= enableDepthWrite;
			clone.scissorRectangle	= scissorRectangle;
			
			return clone;
		}
		
		public function prepareContext(context 		: Context3D,
									   backBuffer	: RenderTarget,
									   previous		: PassConfig) : void
		{
			if (!previous || previous._renderTarget != _renderTarget)
			{
				if (_renderTarget)
					context.setRenderToTexture(
						_renderTarget.resource.getNativeTexture(context),
						_renderTarget.useDepthAndStencil,
						_renderTarget.antiAliasing,
						_renderTarget.surfaceSelector
					);
					
				else
					context.setRenderToBackBuffer();
				
				var rt : RenderTarget = _renderTarget || backBuffer;
				var color : uint = rt.backgroundColor;
				
				context.clear(
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.,
					((color >> 24) & 0xff) / 255.
				);
			}
			
			context.setScissorRectangle(_rectangle);
			context.setDepthTest(_enableDepthWrite, _compareMode);
			
			context.setBlendFactors(_blendingSource, _blendingDest);
			context.setCulling(_triangleCullingStr);
		}
		
	}
}