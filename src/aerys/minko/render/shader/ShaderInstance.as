package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.TriangleCulling;
	
	import flash.display3D.Context3D;
	import flash.geom.Rectangle;

	/**
	 * The base class for static shaders. 
	 * 
	 * @author Jean-Marc Le Roux
	 * @author Romain Gilliotte
	 * 
	 */
	public final class ShaderInstance
	{
		private static const TMP_NUMBERS	: Vector.<Number>	= new Vector.<Number>(0xffff, true);
		private static const TMP_INTS		: Vector.<int>		= new Vector.<int>(0xffff, true);
		
		private var _numUses	: int				= 0;
		private var _generator	: Shader			= null;
		private var _signature	: Signature			= null;
		
		private var _settings	: ShaderSettings	= null;
		private var _program	: Program3DResource	= null;
		
		private var _retained	: Signal			= new Signal('ShaderInstance.retained');
		private var _released	: Signal			= new Signal('ShaderInstance.released');
		private var _begin		: Signal			= new Signal('ShaderInstance.begin');
		private var _end		: Signal			= new Signal('ShaderInstance.end');
		
		public function get generator() : Shader
		{
			return _generator;
		}
		
		public function get signature() : Signature
		{
			return _signature;
		}
		
		public function get settings() : ShaderSettings
		{
			return _settings;
		}
		
		public function get program() : Program3DResource
		{
			return _program;
		}
		
		public function get isDisposable() : Boolean
		{
			return _numUses == 0;
		}
		
		public function get retained() : Signal
		{
			return _retained;
		}
		
		public function get released() : Signal
		{
			return _released;
		}
		
		public function get begin() : Signal
		{
			return _begin;
		}
		
		public function get end() : Signal
		{
			return _end;
		}
		
		public final function ShaderInstance(generator	: Shader,
											 settings	: ShaderSettings,
											 program	: Program3DResource,
											 signature	: Signature)
		{
			_generator	= generator;
			_settings	= settings;
			_program	= program;
			_signature	= signature;
		}
		
		public function prepareContext(context 		: Context3DResource,
									   backBuffer	: RenderTarget,
									   previous		: ShaderInstance) : void
		{
			_settings.prepareContext(context, backBuffer, previous != null ? previous.settings : null);
			_program.prepareContext(context, previous != null ? previous.program : null);
		}
		
		public function retain() : void
		{
			++_numUses;
			_settings.retain();
			
			if (_program != null)
				_program.retain();
			
			_retained.execute(this, _numUses);
		}
		
		public function release() : void
		{
			--_numUses;
			_settings.release();
			
			if (_program != null)
				_program.release();
			
			_released.execute(this, _numUses);
		}
	}
}