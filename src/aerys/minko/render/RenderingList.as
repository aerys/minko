package aerys.minko.render
{
	import aerys.minko.Minko;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.display3D.Context3D;
	import flash.utils.Dictionary;

	public final class RenderingList
	{
		private var _shaderToDrawCalls	: Dictionary				= new Dictionary();
		private var _numDrawCalls		: uint						= 0;
		
		private var _shaders			: Vector.<Shader>	= new Vector.<Shader>();
		private var _numShaders			: int						= 0;
		
		private var _sorted				: Boolean					= false;
		
		private var _numTriangles		: uint						= 0;
		
		public function get numDrawCalls() : uint
		{
			return _numDrawCalls;
		}
		
		public function get numShaders() : uint
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
		
		public function addDrawCall(shader		: Shader,
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
		
		public function addDrawCalls(passes		: Vector.<Shader>,
									 drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls += numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var instance 	: Shader 	= passes[i] as Shader;
				var calls 		: Vector.<DrawCall>	= _shaderToDrawCalls[instance] as Vector.<DrawCall>;
				
				if (!calls)
				{
					_shaderToDrawCalls[instance] = new <DrawCall>[drawCalls[i]];
					_shaders[_numShaders] = instance;
					++_numShaders;
					
					_sorted = false;
					
					instance.addedToRenderingList.execute(instance, this);
				}
				else
				{
					calls.push(drawCalls[i]);
				}
			}
		}
		
		public function removeDrawCall(pass			: Shader,
									   drawCall		: DrawCall) : void
		{
			var instance 	: Shader 	= pass;
			var calls 		: Vector.<DrawCall>	= _shaderToDrawCalls[instance] as Vector.<DrawCall>;
			var numCalls	: int				= calls.length - 1;
			var callindex 	: int 				= calls.indexOf(drawCall);
			
			--_numDrawCalls;
			
			if (numCalls == 0)
			{
				--_numShaders;
				delete _shaderToDrawCalls[instance];
				
				var numShaders : uint = _shaders.length - 1;
				
				_shaders[_shaders.indexOf(instance)] = _shaders[numShaders];
				_shaders.length = numShaders;
				
				instance.removedFromRenderingList.execute(instance, this);
			}
			else
			{
				calls[callindex] = calls[numCalls];
				calls.length = numCalls;
			}
		}
		
		public function removeDrawCalls(passes		: Vector.<Shader>,
										drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls -= numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var toDelete	: DrawCall			= drawCalls[i] as DrawCall;
				var state 		: Shader 	= passes[i] as Shader;
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
				Shader.sort(_shaders, _numShaders);
				_sorted = true;
			}
			
			// apply states
			var previous 		: Shader 	= null;
			var callTriangles	: uint				= 0;
			var call			: DrawCall			= null;
			var previousCall	: DrawCall			= null;
			
			for (var i : int = 0; i < _numShaders; ++i)
			{
				var shader 		: Shader			= _shaders[i];
				var calls 		: Vector.<DrawCall> = _shaderToDrawCalls[shader];
				var numCalls	: int				= calls.length;

				if (shader.enabled)
				{
					shader.begin.execute(
						shader, context, backBuffer, previous
					);
					
					shader.prepareContext(context, backBuffer, previous);
					previous = shader;
					
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
						shader, context, backBuffer, previous
					);
				}
			}
			
			return _numTriangles;
		}
	}
}