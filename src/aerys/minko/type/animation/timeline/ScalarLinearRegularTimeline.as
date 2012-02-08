package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.ISceneNode;

	public class ScalarLinearRegularTimeline implements ITimeline
	{
		private var _propertyName	: String;
		private var _deltaTime		: uint;
		private var _values			: Vector.<Number>;

		public function get propertyName()	: String	{ return _propertyName; }
		public function get duration()		: uint		{ return _deltaTime * (_values.length - 1); }

		minko_animation function get deltaTime() : uint
		{
			return _deltaTime;
		}
		
		minko_animation function get values() : Vector.<Number>
		{
			return _values;
		}
		
		public function ScalarLinearRegularTimeline(propertyName	: String,
													deltaTime 		: uint,
													values			: Vector.<Number>)
		{
			_propertyName	= propertyName;
			_deltaTime		= deltaTime;
			_values			= values;
		}

		public function updateAt(t : int, scene : ISceneNode) : void
		{
			var time				: int		= t < 0 ? duration + t : t;
			var previousTimeId		: uint		= Math.floor(time / _deltaTime);
			var nextTimeId			: uint		= Math.ceil(time / _deltaTime);
			var interpolationRatio	: Number	= (time % _deltaTime) / _deltaTime;
			
			if (t < 0)
				interpolationRatio = 1 - interpolationRatio;

			scene[_propertyName] =
				(1 - interpolationRatio) * _values[previousTimeId] +
				interpolationRatio * _values[nextTimeId];
		}

		public function clone() : ITimeline
		{
			return new ScalarLinearRegularTimeline(_propertyName, _deltaTime, _values.slice());
		}
	}
}