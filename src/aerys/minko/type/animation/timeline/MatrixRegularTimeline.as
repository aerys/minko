package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.type.math.Matrix4x4;
	
	use namespace minko_animation;
	
	public class MatrixRegularTimeline implements ITimeline
	{
		private var _propertyName	: String;

		private var _deltaTime		: uint;
		private var _values			: Vector.<Matrix4x4>;

		public function get propertyName() : String
		{
			return _propertyName;
		}
		
		public function get duration() : uint
		{
			return _deltaTime * (_values.length - 1);
		}
		
		public function get deltaTime() : uint
		{
			return _deltaTime;
		}
		
		public function get matrices() : Vector.<Matrix4x4>
		{
			return _values;
		}
		
		public function MatrixRegularTimeline(propertyName	: String,
													deltaTime		: uint,
													matrices		: Vector.<Matrix4x4>)
		{
			_propertyName	= propertyName;
			_deltaTime		= deltaTime;
			_values			= matrices;
		}

		public function updateAt(t : int, target : Object) : void
		{
			var time			: uint	= t < 0 ? duration + t : t;
			var timeCount		: uint 	= _values.length;
			var previousTimeId	: int 	= int(time / _deltaTime);
			var nextTimeId		: int 	= (previousTimeId + 1) % timeCount;

			// change matrix value.
			var out : Matrix4x4 = target[_propertyName] as Matrix4x4;
			
			if (!out)
			{
				throw new Error(
					"'" + _propertyName
					+ "' could not be found in '"
					+ target.name + "'."
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

		public function clone() : ITimeline
		{
			return new MatrixRegularTimeline(
				_propertyName,
				_deltaTime,
				_values.slice()
			);
		}
	}
}
