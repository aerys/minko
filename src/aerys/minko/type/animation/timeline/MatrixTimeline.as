package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.type.math.Matrix4x4;
	
	public class MatrixTimeline extends AbstractTimeline
	{
		use namespace minko_animation;
		
		private var _timeTable			: Vector.<uint>
		private var _values				: Vector.<Matrix4x4>;
        private var _interpolate        : Boolean;
		private var _interpolateScale	: Boolean;
        private var _interpolateW       : Boolean;
		private var _timeTableLength	: uint;

		minko_animation function get interpolate() : Boolean
		{
			return _interpolate;
		}
		
		minko_animation function get interpolateScale() : Boolean
		{
			return _interpolateScale;
		}
		
		minko_animation function get interpolateW() : Boolean
		{
			return _interpolateW;
		}
		
		minko_animation function get timeTable() : Vector.<uint>
		{
			return _timeTable;
		}
		
		minko_animation function get matrices() : Vector.<Matrix4x4>
		{
			return _values;
		}
		
		public function MatrixTimeline(propertyPath		: String,
									   timeTable		: Vector.<uint>,
									   matrices			: Vector.<Matrix4x4>,
                                       interpolate      : Boolean   = false,
									   interpolateScale	: Boolean	= false,
                                       interpolateW     : Boolean   = false)
		{
			super(propertyPath, timeTable[uint(timeTable.length - 1)]);
		
			_timeTableLength	= timeTable.length;
			_timeTable 			= timeTable;
			_values 			= matrices;
            _interpolate 		= interpolate;
			_interpolateScale 	= interpolateScale;
            _interpolateW 		= interpolateW;
		}

		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			var time		: uint	= t < 0 ? duration + t : t;
			var timeId		: uint 	= getIndexForTime(time);
			var timeCount	: uint 	= _timeTableLength;
			
			if (timeId >= timeCount)
				timeId = timeCount - 1;
			
			// change matrix value.
			var out : Matrix4x4 = _currentTarget[_propertyName];
			
			if (!out)
			{
				throw new Error(
                    "'" + _propertyName + "' could not be found in '" + _currentTarget + "'."
				);
			}

            if (_interpolate)
            {
                var previousTime		: Number	= _timeTable[int(timeId - 1)];
                var nextTime			: Number	= _timeTable[timeId];
                
                out.interpolateBetween(
                    _values[int(timeId - 1)],
                    _values[timeId],
                    (time - previousTime) / (nextTime - previousTime),
                    _interpolateScale,
                    _interpolateW
                );
            }
            else
            {
                if (timeId == 0)
                    out.copyFrom(_values[0]);
                else
                    out.copyFrom(_values[uint(timeId - 1)]);
            }
		}

		private function getIndexForTime(t : uint) : uint
		{
			// use a dichotomy to find the current frame in the time table.
			var timeCount 		: uint = _timeTableLength;
			var bottomTimeId	: uint = 0;
			var upperTimeId		: uint = timeCount;
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
			return new MatrixTimeline(
				propertyPath,
				_timeTable.slice(),
				_values.slice(),
				_interpolate,
				_interpolateScale,
				_interpolateW
			);
		}
	}
}
