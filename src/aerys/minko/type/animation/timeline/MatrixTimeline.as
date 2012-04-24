package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.math.Matrix4x4;
	
	public class MatrixTimeline extends AbstractTimeline
	{
		use namespace minko_animation;
		
		private var _timeTable		: Vector.<uint>
		private var _values			: Vector.<Matrix4x4>;

		minko_animation function get timeTable() : Vector.<uint>
		{
			return _timeTable;
		}
		
		minko_animation function get matrices() : Vector.<Matrix4x4>
		{
			return _values;
		}
		
		public function MatrixTimeline(propertyPath	: String,
									   timeTable	: Vector.<uint>,
									   matrices		: Vector.<Matrix4x4>)
		{
			super(propertyPath, timeTable[int(timeTable.length - 1)]);
			
			_timeTable = timeTable;
			_values = matrices;
		}

		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			var time		: uint	= t < 0 ? duration + t : t;
			var timeId		: uint 	= getIndexForTime(time);
			var timeCount	: uint 	= _timeTable.length;
			
			if (timeId >= timeCount)
				timeId = timeCount - 1;
			
			// change matrix value.
			var out : Matrix4x4 = currentTarget[propertyName];
			
			if (!out)
			{
				throw new Error(
					"'" + propertyName
					+ "' could not be found in '"
					+ currentTarget + "'."
				);
			}

			var previousTime		: Number	= _timeTable[int(timeId - 1)];
			var nextTime			: Number	= _timeTable[timeId];
			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);
			var previousMatrix		: Matrix4x4 = _values[int(timeId - 1)];
			var nextMatrix			: Matrix4x4 = _values[timeId];
			
			out.interpolateBetween(previousMatrix, nextMatrix, interpolationRatio);
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
			return new MatrixTimeline(
				propertyPath,
				_timeTable.slice(),
				_values.slice()
			);
		}
	}
}
