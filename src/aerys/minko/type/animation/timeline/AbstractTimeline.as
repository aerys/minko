package aerys.minko.type.animation.timeline
{
	import flash.utils.Dictionary;

	/**
	 * The base class to extend to create new animation timeline types.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractTimeline implements ITimeline
	{
		private var _propertyPath			: String			= null;
		private var _parts					: Vector.<String>	= null;
		private var _numParts				: int 				= 0;
		
		private var _duration				: uint				= 0;
		
		protected var _currentTarget		: Object			= null;
		protected var _propertyName			: String			= null;
		private var _targetToCurrentTarget	: Dictionary 		= new Dictionary();
		
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
			_parts 		= Vector.<String>(_propertyPath.split("."));
			_numParts 	= int(_parts.length - 1);
			
			_propertyName = _parts[_numParts] as String;
		}
		
		public function updateAt(time : int, target : Object) : void
		{
			if (_targetToCurrentTarget[target] == null)
			{		
				_currentTarget = target;
				
				for (var partId : int = 0; partId < _numParts; ++partId)
					_currentTarget = _currentTarget[_parts[partId] as String];
				
				_targetToCurrentTarget[target] = _currentTarget;
			}
			
			_currentTarget = _targetToCurrentTarget[target];
		}
		
		public function invalidate(target : Object = null) : void
		{
			if (target)
				delete _targetToCurrentTarget[target];
			else
				_targetToCurrentTarget = new Dictionary();
		}
		
		public function clone() : ITimeline
		{
			throw new Error();
		}
	}
}