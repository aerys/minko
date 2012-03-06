package aerys.minko.type.animation.timeline
{
	/**
	 * The base class to extend to create new animation timeline types.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractTimeline implements ITimeline
	{
		private var _propertyPath	: String			= null;
		private var _parts			: Vector.<String>	= null;
		
		private var _duration		: uint				= 0;
		
		private var _currentTarget	: Object			= null;
		private var _propertyName	: String			= null;
		
		public function get duration() : uint
		{
			return _duration;
		}
		
		public function get propertyPath() : String
		{
			return _propertyPath;
		}
		
		protected function get currentTarget() : Object
		{
			return _currentTarget;
		}
		
		protected function get propertyName() : String
		{
			return _propertyName;
		}
		
		public function AbstractTimeline(propertyPath	: String,
										 duration		: uint)
		{
			_propertyPath = propertyPath;
			_duration = duration;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_parts = Vector.<String>(_propertyPath.split("."));
			_propertyName = _parts[int(_parts.length - 1)] as String;
		}
		
		public function updateAt(time : int, target : Object) : void
		{
			_currentTarget = target;
			
			var lastPartId : int = _parts.length - 1;
			for (var partId : int = 0; partId < lastPartId; ++partId)
				_currentTarget = _currentTarget[_parts[partId] as String];
		}
		
		public function clone() : ITimeline
		{
			throw new Error();
		}
	}
}