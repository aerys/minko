package aerys.minko.type.animation.timeline
{
	import aerys.minko.type.math.Matrix4x4;
	
	public final class MatrixRegularTimeline extends AbstractTimeline
	{
		private var _deltaTime	: uint;
		private var _values		: Vector.<Matrix4x4>;

		public function get deltaTime() : uint
		{
			return _deltaTime;
		}
		
		public function get matrices() : Vector.<Matrix4x4>
		{
			return _values;
		}
		
		public function MatrixRegularTimeline(propertyPath	: String,
											  deltaTime		: uint,
											  matrices		: Vector.<Matrix4x4>)
		{
			super(propertyPath, deltaTime * (matrices.length - 1));
			
			_deltaTime = deltaTime;
			_values	= matrices;
		}

		override public function updateAt(t:int, target:Object):void
		{
			super.updateAt(t, target);
			
			var time			: uint	= t < 0 ? duration + t : t;
			var timeCount		: uint 	= _values.length;
			var previousTimeId	: int 	= int(time / _deltaTime);
			var nextTimeId		: int 	= (previousTimeId + 1) % timeCount;

			// change matrix value.
			var out : Matrix4x4 = currentTarget[propertyName] as Matrix4x4;
			
			if (!out)
			{
				throw new Error(
					"'" + propertyName
					+ "' could not be found in '"
					+ currentTarget + "'."
				);
			}
			
			var previousTime		: Number	= previousTimeId * _deltaTime;
			var nextTime			: Number	= nextTimeId * _deltaTime;
			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);
			var previousMatrix		: Matrix4x4 = _values[previousTimeId] as Matrix4x4;
			var nextMatrix			: Matrix4x4 = _values[nextTimeId] as Matrix4x4;
			
			if (t < 0)
				interpolationRatio = 1 - interpolationRatio;
			
			if (interpolationRatio == 0.)
				out.copyFrom(previousMatrix);
			else if (interpolationRatio == 1.)
				out.copyFrom(nextMatrix);
			else
				out.interpolateBetween(previousMatrix, nextMatrix, interpolationRatio);
		}

		override public function clone() : ITimeline
		{
			return new MatrixRegularTimeline(
				propertyPath,
				_deltaTime,
				_values.slice()
			);
		}
	}
}
