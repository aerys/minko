package aerys.minko.render
{
	import aerys.minko.Minko;
	import aerys.minko.render.shader.ShaderTemplate;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.display3D.Context3D;
	import flash.utils.Dictionary;

	public final class RenderingList
	{
		private var _stateToDrawCalls	: Dictionary				= new Dictionary();
		private var _numDrawCalls		: uint						= 0;
		
		private var _states				: Vector.<RendererState>	= new Vector.<RendererState>();
		private var _numStates			: int						= 0;
		
		private var _sorted				: Boolean					= false;
		
		public function get numDrawCalls() : uint
		{
			return _numDrawCalls;
		}
		
		public function get numRendererStates() : uint
		{
			return _numStates;
		}
		
		public function clear() : void
		{
			_numStates = 0;
			_stateToDrawCalls = new Dictionary();
		}
		
		public function addDrawCalls(passes		: Vector.<ShaderTemplate>,
									 drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls += numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var state 	: RendererState 	= (passes[i] as ShaderTemplate).state;
				var calls 	: Vector.<DrawCall>	= _stateToDrawCalls[state] as Vector.<DrawCall>;
				
				if (!calls)
				{
					_stateToDrawCalls[state] = new <DrawCall>[drawCalls[i]];
					_states[_numStates] = state;
					++_numStates;
					
					_sorted = false;
				}
				else
				{
					calls.push(drawCalls[i]);
				}
			}
		}
		
		public function removeDrawCalls(passes		: Vector.<ShaderTemplate>,
										drawCalls	: Vector.<DrawCall>) : void
		{
			var numPasses	: int	= passes.length;
			
			_numDrawCalls -= numPasses;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var toDelete	: DrawCall			= drawCalls[i] as DrawCall;
				var state 		: RendererState 	= (passes[i] as ShaderTemplate).state;
				var calls 		: Vector.<DrawCall>	= _stateToDrawCalls[state] as Vector.<DrawCall>;
				var numCalls	: int				= calls.length - 1;
				
				while (numCalls >= 0)
				{
					if (calls[numCalls] == toDelete)
						calls.splice(numCalls, 1);
					--numCalls;
				}
				
				if (calls.length == 0)
				{
					_states.splice(_states.indexOf(state), 1);
					delete _stateToDrawCalls[state];
					--_numStates;
				}
			}
		}
		
		public function render(context : Context3D) : uint
		{
			var numTriangles : uint 	= 0;
			
			// FIXME: handle render targets
			context.clear();
			
			context.enableErrorChecking = (Minko.debugLevel & DebugLevel.RENDERER) != 0;
			
			// sort states
			if (!_sorted)
			{
				RendererState.sort(_states, _numStates);
				_sorted = true;
			}
			
			// apply states
			for (var i : int = 0; i < _numStates; ++i)
			{
				var state 			: RendererState		= _states[i];
				var calls 			: Vector.<DrawCall> = _stateToDrawCalls[state];
				var numCalls		: int				= calls.length;
				var call			: DrawCall			= null;
				var previousCall	: DrawCall			= null;
				
				state.apply(context);
				
				for (var j : int = 0; j < numCalls; ++j)
				{
					call = calls[j]
					numTriangles += call.apply(context, previousCall);
					previousCall = call;
				}
			}
			
			return numTriangles;
		}
	}
}