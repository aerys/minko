package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
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
		private static const TMP_NUMBERS		: Vector.<Number>	= new Vector.<Number>(0xffff, true);
		private static const TMP_INTS			: Vector.<int>		= new Vector.<int>(0xffff, true);
		
		private var _numUses	: int					= 0;
		private var _generator	: Shader	= null;
		private var _signature	: Signature				= null;
		
		private var _config		: ShaderSettings		= null;
		private var _program	: Program3DResource		= null;
		
		public function get generator() : Shader
		{
			return _generator;
		}
		
		public function get signature() : Signature
		{
			return _signature;
		}
		
		public function get config() : ShaderSettings
		{
			return _config;
		}
		
		public function get program() : Program3DResource
		{
			return _program;
		}
		
		public function get isDisposable() : Boolean
		{
			return _numUses == 0
		}
		
		public final function ShaderInstance(generator	: Shader,
										   config		: ShaderSettings,
										   program		: Program3DResource,
										   signature	: Signature)
		{
			_generator	= generator;
			_config		= config;
			_program	= program;
			_signature	= signature;
		}
		
		public function prepareContext(context 		: Context3D,
									   backBuffer	: RenderTarget,
									   previous		: ShaderInstance) : void
		{
			_config.prepareContext(context, backBuffer, previous != null ? previous.config : null);
			_program.prepareContext(context, previous != null ? previous.program : null);
		}
		
		public function retain() : void
		{
			++_numUses;
			_config.retain();
			
			if (_program != null)
				_program.retain();
		}
		
		public function release() : void
		{
			--_numUses;
			_config.release();
			
			if (_program != null)
				_program.release();
		}
		
		public static function sort(instances : Vector.<ShaderInstance>, numStates : int) : void
		{
			var n 		: int 		= numStates;
			var i		: int 		= 0;
			var j		: int 		= 0;
			var k		: int 		= 0;
			var t		: int		= 0;
			var state 	: ShaderInstance	= instances[0];
			var anmin	: Number 	= -state._config.priority;
			var nmax	: int  		= 0;
			var p		: Number	= 0.;
			var sorted	: Boolean	= true;
			
			for (i = 0; i < n; ++i)
			{
				state = instances[i];
				p = -state._config.priority;
				
				TMP_INTS[i] = 0;
				TMP_NUMBERS[i] = p;
				if (p < anmin)
					anmin = p;
				else if (p > Number(TMP_NUMBERS[nmax]))
					nmax = i;
			}
			
			if (anmin == Number(TMP_NUMBERS[nmax]))
				return ;
			
			var m		: int 	= Math.ceil(n * .125);
			var nmove	: int 	= 0;
			var c1		: Number = (m - 1) / (Number(TMP_NUMBERS[nmax]) - anmin);
			
			for (i = 0; i < n; ++i)
			{
				k = int(c1 * (Number(TMP_NUMBERS[i]) - anmin));
				TMP_INTS[k] = int(TMP_INTS[k]) + 1;
			}
			
			for (k = 1; k < m; ++k)
				TMP_INTS[k] = int(TMP_INTS[k]) + int(TMP_INTS[int(k - 1)]);
			
			var hold		: Number 		= Number(TMP_NUMBERS[nmax]);
			var holdState 	: ShaderInstance 	= instances[nmax] as ShaderInstance;
			
			TMP_NUMBERS[nmax] = Number(TMP_NUMBERS[0]);
			TMP_NUMBERS[0] = hold;
			instances[nmax] = instances[0];
			instances[0] = holdState;
			
			var flash		: Number			= 0.;
			var flashState	: ShaderInstance	= null;
			
			j = 0;
			k = int(m - 1);
			i = int(n - 1);
			
			while (nmove < i)
			{
				while (j > int(TMP_INTS[k]) - 1)
				{
					++j;
					k = int(c1 * (Number(TMP_NUMBERS[j]) - anmin));
				}
				
				flash = Number(TMP_NUMBERS[j]);
				flashState = instances[j] as ShaderInstance;
				
				while (!(j == int(TMP_INTS[k])))
				{
					k = int(c1 * (flash - anmin));
					
					t = int(TMP_INTS[k]) - 1;
					hold = Number(TMP_NUMBERS[t]);
					holdState = instances[t] as ShaderInstance;
					
					TMP_NUMBERS[t] = flash;
					instances[t] = flashState;
					
					flash = hold;
					flashState = holdState;
					
					TMP_INTS[k] = int(TMP_INTS[k]) - 1;
					++nmove;
				}
			}
			
			for (j = 1; j < n; ++j)
			{
				hold = Number(TMP_NUMBERS[j]);
				holdState = instances[j];
				
				i = int(j - 1);
				while (i >= 0 && Number(TMP_NUMBERS[i]) > hold)
				{
					// not trivial
					TMP_NUMBERS[int(i + 1)] = Number(TMP_NUMBERS[i]);
					instances[int(i + 1)] = instances[i];
					
					--i;
				}
				
				TMP_NUMBERS[int(i + 1)] = hold;
				instances[int(i + 1)] = holdState;
			}
		}
	}
}