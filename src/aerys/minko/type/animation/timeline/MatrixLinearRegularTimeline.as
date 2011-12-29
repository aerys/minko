package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.math.Matrix4x4;
	
	use namespace minko_animation;
	
	public class MatrixLinearRegularTimeline implements ITimeline
	{
		private var _propertyName	: String;

		private var _deltaTime		: uint;
		private var _values			: Vector.<Matrix4x4>;

		public function get propertyName()		: String				{ return _propertyName; }
		public function get duration()			: uint					{ return _deltaTime * (_values.length - 1); }
		public function get deltaTime() 		: uint 					{ return _deltaTime; }
		minko_animation function get matrices()	: Vector.<Matrix4x4>	{ return _values; }
		
		public function MatrixLinearRegularTimeline(propertyName	: String,
													deltaTime		: uint,
													matrices		: Vector.<Matrix4x4>)
		{
			_propertyName	= propertyName;
			_deltaTime		= deltaTime;
			_values			= matrices;
		}

		public function updateAt(t : uint, scene : IScene) : void
		{
			var previousTimeId	 : int = Math.floor(t / _deltaTime);
			var nextTimeId		 : int = Math.ceil(t / _deltaTime);
			var timeCount	: uint = _values.length;

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
			
			if (previousTimeId == 0)
			{
				Matrix4x4.copy(_values[0], out);
			}
			else if (previousTimeId >= timeCount)
			{
				Matrix4x4.copy(_values[int(timeCount - 1)], out);
			}
			else
			{
				var previousTime		: Number	= previousTimeId * _deltaTime;
				var nextTime			: Number	= nextTimeId * _deltaTime;
				var interpolationRatio	: Number	= (t - previousTime) / (nextTime - previousTime);
				
				var previousMatrix		: Matrix4x4 = _values[previousTimeId];
				var nextMatrix			: Matrix4x4 = _values[nextTimeId];
				
				Matrix4x4.copy(previousMatrix, out);
				out.interpolateTo(nextMatrix, interpolationRatio);
			}
		}

		public function clone() : ITimeline
		{
			return new MatrixLinearRegularTimeline(_propertyName, _deltaTime, _values.slice());
		}

		public function reverse() : void
		{
			_values.reverse();
		}
	}
}
