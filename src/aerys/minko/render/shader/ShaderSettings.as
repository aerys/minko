package aerys.minko.render.shader
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.Program3DResource;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DCompareMode;
	import flash.geom.Rectangle;

	public final class ShaderSettings
	{
		private static const TMP_NUMBERS	: Vector.<Number>	= new Vector.<Number>(0xffff, true);
		private static const TMP_INTS		: Vector.<int>		= new Vector.<int>(0xffff, true);
		
		private var _priority			: Number			= 0.;
		
		private var _renderTarget		: RenderTarget		= null;
		private var _program			: Program3DResource	= null;
		private var _compareMode		: String			= null;
		private var _enableDepthWrite	: Boolean			= true;
		private var _rectangle			: Rectangle			= null;
		
		private var _enabled			: Boolean			= true;
		
		public function get priority() : Number
		{
			return _priority;
		}
		public function set priority(value : Number) : void
		{
			_priority = value;
		}
		
		public function get scissorRectangle() : Rectangle
		{
			return _rectangle;
		}
		public function set scissorRectangle(value : Rectangle) : void
		{
			_rectangle = value;
		}
		
		public function get compareMode() : String
		{
			return _compareMode;
		}
		public function set compareMode(value : String) : void
		{
			_compareMode = value;
		}
		
		public function get enableDepthWrite() : Boolean
		{
			return _enableDepthWrite;
		}
		public function set enableDepthWrite(value : Boolean) : void
		{
			_enableDepthWrite = value;
		}
		
		public function get program() : Program3DResource
		{
			return _program;
		}
		public function set program(value : Program3DResource) : void
		{
			_program = value;
		}
		
		public function get renderTarget() : RenderTarget
		{
			return _renderTarget;
		}
		public function set renderTarget(value : RenderTarget) : void
		{
			_renderTarget = value;
		}
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}

		public function ShaderSettings()
		{
			initialize();
		}
		
		private function initialize() : void
		{
			_compareMode = Context3DCompareMode.LESS;
			_enableDepthWrite = true;
			_rectangle = null;
		}
		
		public function clone() : ShaderSettings
		{
			var clone : ShaderSettings = new ShaderSettings();
			
			clone._priority	= _priority;
			
			clone._renderTarget	= _renderTarget;
			clone._program = _program;
			clone._compareMode = _compareMode;
			clone._enableDepthWrite	= _enableDepthWrite;
			clone._rectangle = _rectangle;
			
			clone._enabled = _enabled;
			
			return clone;
		}
		
		public function prepareContext(context 		: Context3D,
									   backBuffer	: RenderTarget,
									   previous		: ShaderSettings) : void
		{
			if (!previous || previous._renderTarget != _renderTarget)
			{
				if (_renderTarget)
				{
					context.setRenderToTexture(
						_renderTarget.resource.getNativeTexture(context),
						_renderTarget.useDepthAndStencil,
						_renderTarget.antiAliasing,
						_renderTarget.surfaceSelector
					);
				}
				else
				{
					context.setRenderToBackBuffer();
				}
				
				var rt : RenderTarget = _renderTarget || backBuffer;
				var color : uint = rt.backgroundColor;
				
				context.clear(
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.,
					((color >> 24) & 0xff) / 255.
				);
			}
			
			context.setProgram(_program.getProgram3D(context));
			context.setScissorRectangle(_rectangle);
			context.setDepthTest(_enableDepthWrite, _compareMode);
		}
	}
}