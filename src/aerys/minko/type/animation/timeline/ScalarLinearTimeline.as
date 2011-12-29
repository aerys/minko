package aerys.minko.type.animation.timeline
{
	import aerys.minko.scene.node.IScene;

	public class ScalarLinearTimeline implements ITimeline
	{
		protected var _propertyName	: String;
		protected var _timeTable	: Vector.<uint>
		protected var _values		: Vector.<Number>;

		public function get propertyName()	: String			{ return _propertyName; }
		public function get duration()		: uint				{ return _timeTable[_timeTable.length - 1]; }
		public function get values()		: Vector.<Number>	{ return _values; }
		public function get timeTable()		: Vector.<uint>		{ return _timeTable; }
		
		public function ScalarLinearTimeline(propertyName	: String,
											 timeTable 		: Vector.<uint>,
											 values			: Vector.<Number>)
		{
			_propertyName	= propertyName;
			_timeTable		= timeTable;
			_values			= values;
		}

		public function updateAt(t : uint, scene : IScene):void
		{
			var timeId		: uint = getIndexForTime(t);
			var timeCount	: uint = _timeTable.length;

			// change value.
			if (timeId == 0)
			{
				scene[_propertyName] = _values[0];
			}
			else if (timeId == timeCount)
			{
				scene[_propertyName] = _values[timeCount - 1];
			}
			else
			{
				var previousTime		: Number	= _timeTable[timeId - 1];
				var nextTime			: Number	= _timeTable[timeId];
				var interpolationRatio	: Number	= (t - previousTime) / (nextTime - previousTime);

				scene[_propertyName] =
					(1 - interpolationRatio) * _values[timeId - 1] +
					interpolationRatio * _values[timeId];
			}
		}

		private function getIndexForTime(t : uint) : uint
		{
			// use a dichotomy to find the current frame in the time table.
			var timeCount 		: uint = _timeTable.length;
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

		public function clone() : ITimeline
		{
			return new ScalarLinearTimeline(_propertyName, _timeTable.slice(), _values.slice());
		}

		public function reverse() : void
		{
			_timeTable.reverse();
			_values.reverse();
		}
	}
}