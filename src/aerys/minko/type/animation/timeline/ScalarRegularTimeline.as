package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;

	public class ScalarRegularTimeline extends AbstractTimeline
	{
		private var _deltaTime		: uint				= 0;
		private var _values			: Vector.<Number>	= null;

		minko_animation function get deltaTime() : uint
		{
			return _deltaTime;
		}
		
		minko_animation function get values() : Vector.<Number>
		{
			return _values;
		}
		
		public function ScalarRegularTimeline(propertyPath	: String,
											  deltaTime 	: uint,
											  values		: Vector.<Number>)
		{
			super(propertyPath, deltaTime * (values.length - 1));
			
			_deltaTime = deltaTime;
			_values	= values;
		}

		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			var time				: int		= t < 0 ? duration + t : t;
			var previousTimeId		: uint		= Math.floor(time / _deltaTime);
			var nextTimeId			: uint		= Math.ceil(time / _deltaTime);
			var interpolationRatio	: Number	= (time % _deltaTime) / _deltaTime;
			
			if (t < 0)
				interpolationRatio = 1 - interpolationRatio;

			currentTarget[propertyName] =
				(1 - interpolationRatio) * _values[previousTimeId] +
				interpolationRatio * _values[nextTimeId];
		}

		override public function clone() : ITimeline
		{
			return new ScalarRegularTimeline(
				propertyPath,
				_deltaTime,
				_values.slice()
			);
		}
	}
}