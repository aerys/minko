package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.math.Matrix4x4;

	use namespace minko_animation;
	
	public final class MatrixSegmentTimeline extends AbstractTimeline
	{
		minko_animation var _timeTable	: Vector.<uint>			= null;
		minko_animation var _matrices	: Vector.<Matrix4x4>	= null;

		public function MatrixSegmentTimeline(propertyPath	: String,
											  timeTable 	: Vector.<uint>,
											  values		: Vector.<Matrix4x4>)
		{
			super(propertyPath, timeTable[int(timeTable.length - 1)]);
			
			_timeTable = timeTable;
			_matrices = values;
		}

		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			var reverse		: Boolean	= t < 0;
			var timeId		: uint 		= getIndexForTime(reverse ? duration - t : t);
			var timeCount	: uint 		= _timeTable.length;

			// change matrix value
			var out : Matrix4x4 = currentTarget[propertyName];
			
			if (!out)
			{
				throw new Error(
					"'" + propertyName
					+ "' could not be found in '"
					+ currentTarget + "'."
				);
			}

			if (timeId == 0)
				out.copyFrom(_matrices[0]);
			else
				out.copyFrom(_matrices[int(timeId - 1)]);
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
				_matrices.slice()
			);
		}
	}
}
