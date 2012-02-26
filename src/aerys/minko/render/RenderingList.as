package aerys.minko.render
{
	import aerys.minko.Minko;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.display3D.Context3D;
	import flash.utils.Dictionary;

	public final class RenderingList
	{
		private var _shaderToDrawCalls	: Dictionary				= new Dictionary();
		private var _numDrawCalls		: uint						= 0;
		
		private var _shaders			: Vector.<ShaderInstance>	= new Vector.<ShaderInstance>();
		private var _numShaders			: int						= 0;
		
		private var _sorted				: Boolean					= false;
		
		private var _numTriangles		: uint						= 0;
		
		public function get numDrawCalls() : uint
		{
			return _numDrawCalls;
		}
		
		public function get numShaderInstances() : uint
		{
			return _numShaders;
		}
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function clear() : void
		{
			_numShaders = 0;
			_numDrawCalls = 0;
			_shaderToDrawCalls = new Dictionary();
		}
		
		public function addDrawCall(shader		: ShaderInstance,
									drawCall	: DrawCall) : void
		{
			++_numDrawCalls;

			var calls 	: Vector.<DrawCall>	= _shaderToDrawCalls[shader] as Vector.<DrawCall>;
			
			if (!calls)
			{
				_shaderToDrawCalls[shader] = new <DrawCall>[drawCall];
				_shaders[_numShaders] = shader;
				++_numShaders;
				
				_sorted = false;
			}
			else
			{
				calls.push(drawCall);
			}
		}
		
		public function addDrawCalls(passes		: Vector.<ShaderInstance>,
									 drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls += numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var instance 	: ShaderInstance 	= passes[i] as ShaderInstance;
				var calls 		: Vector.<DrawCall>	= _shaderToDrawCalls[instance] as Vector.<DrawCall>;
				
				if (!calls)
				{
					_shaderToDrawCalls[instance] = new <DrawCall>[drawCalls[i]];
					_shaders[_numShaders] = instance;
					++_numShaders;
					
					_sorted = false;
				}
				else
				{
					calls.push(drawCalls[i]);
				}
			}
		}
		
		public function removeDrawCall(pass			: ShaderInstance,
									   drawCall		: DrawCall) : void
		{
			var state 		: ShaderInstance 	= pass;
			var calls 		: Vector.<DrawCall>	= _shaderToDrawCalls[state] as Vector.<DrawCall>;
			var numCalls	: int				= calls.length - 1;
			var callindex 	: int 				= calls.indexOf(drawCall);
			
			--_numDrawCalls;
			
			if (numCalls == 0)
			{
				--_numShaders;
				delete _shaderToDrawCalls[state];
				
				var numShaders : uint = _shaders.length - 1;
				
				_shaders[_shaders.indexOf(state)] = _shaders[numShaders];
				_shaders.length = numShaders;
			}
			else
			{
				calls[callindex] = calls[numCalls];
				calls.length = numCalls;
			}
		}
		
		public function removeDrawCalls(passes		: Vector.<ShaderInstance>,
										drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls -= numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var toDelete	: DrawCall			= drawCalls[i] as DrawCall;
				var state 		: ShaderInstance 	= passes[i] as ShaderInstance;
				var calls 		: Vector.<DrawCall>	= _shaderToDrawCalls[state] as Vector.<DrawCall>;
				var numCalls	: int				= calls.length;
				
				for (var callId : int = 0; callId < numCalls; ++callId)
				{
					if (calls[callId] == toDelete)
					{
						--numCalls;
						calls[callId] = calls[numCalls];
						calls.length = numCalls;
						
						break ;
					}
				}
				
				if (calls.length == 0)
				{
					--_numShaders;
					_shaders[_shaders.indexOf(state)] = _shaders[_numShaders];
					
					delete _shaderToDrawCalls[state];
				}
			}
		}
		
		public function render(context : Context3D, backBuffer : RenderTarget) : uint
		{
			_numTriangles = 0;
			
			context.clear();
			context.enableErrorChecking = (Minko.debugLevel & DebugLevel.CONTEXT) != 0;
			
			// sort states
			if (!_sorted && _numShaders != 0)
			{
				ShaderInstance.sort(_shaders, _numShaders);
				_sorted = true;
			}
			
			// apply states
			var previous 		: ShaderInstance 	= null;
			var callTriangles	: uint				= 0;
			var call			: DrawCall			= null;
			var previousCall	: DrawCall			= null;
			
			for (var i : int = 0; i < _numShaders; ++i)
			{
				var shaderInstance 	: ShaderInstance	= _shaders[i];
				var calls 			: Vector.<DrawCall> = _shaderToDrawCalls[shaderInstance];
				var numCalls		: int				= calls.length;

//				if (state.enabled)
				{
					var shader	: Shader	= shaderInstance.owner;
					
					shader.begin.execute(
						shader, shaderInstance, context, backBuffer, previous
					);
					
					shaderInstance.prepareContext(context, backBuffer, previous);
					previous = shaderInstance;
					
					for (var j : int = 0; j < numCalls; ++j)
					{
						call = calls[j];
						callTriangles = call.apply(context, previousCall);
						
						if (callTriangles != 0)
						{
							_numTriangles += callTriangles;
							previousCall = call;
						}
					}
					
					shader.end.execute(
						shader, shaderInstance, context, backBuffer, previous
					);
				}
			}
			
			return _numTriangles;
		}
	}
}