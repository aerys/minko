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

		public function updateAt(t : int, scene : IScene) : void
		{
			var time			: uint	= t < 0 ? duration + t : t;
			var timeCount		: uint 	= _values.length;
			var previousTimeId	: int 	= Math.floor(time / _deltaTime);
			var nextTimeId		: int 	= (previousTimeId + 1) % timeCount;

			// change matrix value.
			var out : Matrix4x4 = scene[_propertyName];
			
			if (!out)
			{
				throw new Error(
					"'" + _propertyName
					+ "' could not be found in scene node '"
					+ scene.name + "'."
				);
			}
			
			var previousTime		: Number	= previousTimeId * _deltaTime;
			var nextTime			: Number	= nextTimeId * _deltaTime;
			var interpolationRatio	: Number	= (time - previousTime) / (nextTime - previousTime);
			var previousMatrix		: Matrix4x4 = _values[previousTimeId];
			var nextMatrix			: Matrix4x4 = _values[nextTimeId];
			
			if (t < 0)
				interpolationRatio = 1 - interpolationRatio;
			
			if (interpolationRatio == 0.)
				Matrix4x4.copy(previousMatrix, out);
			else if (interpolationRatio == 1.)
				Matrix4x4.copy(nextMatrix, out);
			else
			{
				Matrix4x4.copy(previousMatrix, out);
				out.interpolateTo(nextMatrix, interpolationRatio);
			}
		}

		public function clone() : ITimeline
		{
			return new MatrixLinearRegularTimeline(
				_propertyName,
				_deltaTime,
				_values.slice()
			);
		}
	}
}
