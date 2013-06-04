package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;

	public class ScalarTimeline extends AbstractTimeline
	{
		use namespace minko_animation;
		
		minko_animation var _timeTable	    : Vector.<uint>
		minko_animation var _values		    : Vector.<Number>;
        minko_animation var _interpolate    : Boolean;
		
		private var _timeCount : uint = 0;

		public function ScalarTimeline(propertyPath	: String,
									   timeTable 	: Vector.<uint>,
									   values		: Vector.<Number>,
                                       interpolate  : Boolean   = true)
		{
			super(propertyPath, timeTable[int(timeTable.length - 1)]);
			
			_timeTable 		= timeTable;
			_values			= values;
            _interpolate 	= interpolate;
			_timeCount 		= _timeTable.length;
		}

		override public function updateAt(t : int, target : Object):void
		{
			super.updateAt(t, target);
			
			var time		: int	= t < 0 ? duration + t : t;
			var timeId		: uint 	= getIndexForTime(time);
            
            if (_interpolate)
            {    
    			// change value.
    			var previousTime		: Number	= _timeTable[uint(timeId - 1)];
    			var nextTime			: Number	= _timeTable[uint(timeId % _timeCount)];
    			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);
    
    			if (t < 0.)
    				interpolationRatio = 1. - interpolationRatio;
    			
    			_currentTarget[_propertyName] = (1 - interpolationRatio) * _values[uint(timeId - 1)] +
    				interpolationRatio * _values[uint(timeId % _timeCount)];
            }
            else
            {
                _currentTarget[_propertyName] = _values[timeId];
            }
		}

		private function getIndexForTime(t : uint) : uint
		{
			// use a dichotomy to find the current frame in the time table.
			var bottomTimeId	: uint = 0;
			var upperTimeId		: uint = _timeCount;
			var timeId			: uint;

			while (upperTimeId - bottomTimeId > 1)
			{
				timeId = (bottomTimeId + upperTimeId) >> 1;

				if (_timeTable[timeId] > t)
					upperTimeId = timeId;
				else
					bottomTimeId = timeId;
			}

			return upperTimeId;
		}

		override public function clone() : ITimeline
		{
			return new ScalarTimeline(
				propertyPath,
				_timeTable.slice(),
				_values.slice(),
                _interpolate
			);
		}
	}
}