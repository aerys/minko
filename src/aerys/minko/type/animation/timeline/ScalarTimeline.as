package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.ISceneNode;

	public class ScalarTimeline extends AbstractTimeline
	{
		use namespace minko_animation;
		
		minko_animation var _timeTable	: Vector.<uint>
		minko_animation var _values		: Vector.<Number>;

		public function ScalarTimeline(propertyPath	: String,
									   timeTable 	: Vector.<uint>,
									   values		: Vector.<Number>)
		{
			super(propertyPath, timeTable[int(timeTable.length - 1)]);
			
			_timeTable = timeTable;
			_values	= values;
		}

		override public function updateAt(t : int, target : Object):void
		{
			super.updateAt(t, target);
			
			var time		: int	= t < 0 ? duration + t : t;
			var timeId		: uint 	= getIndexForTime(time);
			var timeCount	: uint 	= _timeTable.length;

			// change value.
			var previousTime		: Number	= _timeTable[int(timeId - 1)];
			var nextTime			: Number	= _timeTable[timeId];
			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);

			if (t < 0.)
				interpolationRatio = 1. - interpolationRatio;
			
			currentTarget[propertyName] =
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

		override public function clone() : ITimeline
		{
			return new ScalarTimeline(
				propertyPath,
				_timeTable.slice(),
				_values.slice()
			);
		}
	}
}