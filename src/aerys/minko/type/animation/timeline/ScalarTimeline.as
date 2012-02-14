package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.ISceneNode;

	public class ScalarTimeline implements ITimeline
	{
		protected var _propertyName	: String;
		protected var _timeTable	: Vector.<uint>
		protected var _values		: Vector.<Number>;

		public function get propertyName()	: String	{ return _propertyName; }
		public function get duration()		: uint		{ return _timeTable[_timeTable.length - 1]; }

		minko_animation function get timeTable() : Vector.<uint>
		{
			return _timeTable;
		}
		
		minko_animation function get values() : Vector.<Number>
		{
			return _values;
		}
		
		public function ScalarTimeline(propertyName	: String,
											 timeTable 		: Vector.<uint>,
											 values			: Vector.<Number>)
		{
			_propertyName	= propertyName;
			_timeTable		= timeTable;
			_values			= values;
		}

		public function updateAt(t : int, target : Object):void
		{
			var time		: int	= t < 0 ? duration + t : t;
			var timeId		: uint 	= getIndexForTime(time);
			var timeCount	: uint 	= _timeTable.length;

			// change value.
			var previousTime		: Number	= _timeTable[int(timeId - 1)];
			var nextTime			: Number	= _timeTable[timeId];
			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);

			if (t < 0.)
				interpolationRatio = 1. - interpolationRatio;
			
			target[_propertyName] =
				(1 - interpolationRatio) * _values[timeId - 1] +
				interpolationRatio * _values[timeId];
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
			return new ScalarTimeline(_propertyName, _timeTable.slice(), _values.slice());
		}
	}
}