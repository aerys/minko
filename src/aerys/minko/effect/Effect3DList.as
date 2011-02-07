package aerys.minko.effect
{
	import aerys.minko.ns.minko;

	public final class Effect3DList
	{
		use namespace minko;
		
		minko var _data		: Vector.<IEffect3D>	= new Vector.<IEffect3D>();
		
		public function Effect3DList()
		{
		}
		
		public function push(effect : IEffect3D) : void
		{
			_data.push(effect);
		}
		
		public function pop(numElements : uint = 1) : void
		{
			_data.length = _data.length - numElements;
		}
		
		public function pushEffects(effects : Vector.<IEffect3D>) : void
		{
			var numEffects 	: int 	= _data.length;
			var numPush		: int	= effects.length;
			
			for (var i : int = 0; i < numPush; ++i)
				_data[int(numEffects + i)] = effects[i];
		}
	}
}