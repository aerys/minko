package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.math.Matrix4x4;
	
	use namespace minko_animation;
	
	public class MatrixLinearTimeline implements ITimeline
	{
		private var _propertyName	: String;

		private var _timeTable		: Vector.<uint>
		private var _values			: Vector.<Matrix4x4>;

		public function get propertyName()	: String	{ return _propertyName; }
		public function get duration()		: uint		{ return _timeTable[_timeTable.length - 1]; }
		minko_animation function get matrices()		: Vector.<Matrix4x4>	{ return _values; }
		minko_animation function get timeTable()		: Vector.<uint>			{ return _timeTable; }
		
		public function MatrixLinearTimeline(propertyName	: String,
											 timeTable		: Vector.<uint>,
											 matrices		: Vector.<Matrix4x4>)
		{
			_propertyName	= propertyName;
			_timeTable		= timeTable;
			_values			= matrices;
		}

		public function updateAt(t : uint, scene : IScene) : void
		{
			var timeId		: uint = getIndexForTime(t);
			var timeCount	: uint = _timeTable.length;

			// change matrix value.
			var out : Matrix4x4 = scene[_propertyName];
			if (!out)
			{
				throw new Error(
					"'" + _propertyName
					+ "' could not be found in scene node '"
					+ scene.name + "'"
				);
			}

			if (timeId == 0)
			{
				Matrix4x4.copy(_values[0], out);
			}
			else if (timeId == timeCount)
			{
				Matrix4x4.copy(_values[int(timeCount - 1)], out);
			}
			else
			{
				var previousTime		: Number	= _timeTable[int(timeId - 1)];
				var nextTime			: Number	= _timeTable[timeId];
				var interpolationRatio	: Number	= (t - previousTime) / (nextTime - previousTime);
				
				var previousMatrix		: Matrix4x4 = _values[int(timeId - 1)];
				var nextMatrix			: Matrix4x4 = _values[timeId];
				
				Matrix4x4.copy(previousMatrix, out);
				out.interpolateTo(nextMatrix, interpolationRatio);
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
			return new MatrixLinearTimeline(_propertyName, _timeTable.slice(), _values.slice());
		}

		public function reverse() : void
		{
			_timeTable.reverse();
			_values.reverse();
		}
	}
}
