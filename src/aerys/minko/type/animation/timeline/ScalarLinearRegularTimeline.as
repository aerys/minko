package aerys.minko.type.animation.timeline
{
	import aerys.minko.scene.node.IScene;

	public class ScalarLinearRegularTimeline implements ITimeline
	{
		private var _targetName		: String;
		private var _propertyName	: String;
		private var _deltaTime		: uint;
		private var _values			: Vector.<Number>;

		public function get targetName()	: String	{ return _targetName; }
		public function get propertyName()	: String	{ return _propertyName; }
		public function get duration()		: uint		{ return _deltaTime * (_values.length - 1); }
		public function get deltaTime() 	: uint 		{ return _deltaTime; }
		public function get values()		: Vector.<Number> { return _values; }
		
		public function ScalarLinearRegularTimeline(targetName		: String,
													propertyName	: String,
													deltaTime 		: uint,
													values			: Vector.<Number>)
		{
			_targetName		= targetName;
			_propertyName	= propertyName;
			_deltaTime		= deltaTime;
			_values			= values;
		}

		public function updateAt(t : uint, scene : IScene) : void
		{
			var previousTimeId		: uint		= Math.floor(t / _deltaTime);
			var nextTimeId			: uint		= Math.ceil(t / _deltaTime);
			var interpolationRatio	: Number	= (t % _deltaTime) / _deltaTime;

			scene[_propertyName] =
				(1 - interpolationRatio) * _values[previousTimeId] +
				interpolationRatio * _values[nextTimeId];
		}

		public function clone() : ITimeline
		{
			return new ScalarLinearRegularTimeline(_targetName, _propertyName, _deltaTime, _values.slice());
		}

		public function reverse() : void
		{
			_values.reverse();
		}
	}
}