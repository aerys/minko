package aerys.minko.type.animation.timeline
{
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.interpolation.Path;
	import aerys.minko.type.math.Matrix4x4;

	public final class SplineMatrixTimeline extends AbstractTimeline
	{
		private var _path	: IInterpolation	= null;
		
		public function SplineMatrixTimeline(propertyPath	: String, 
											 duration		: uint,
											 path			: IInterpolation)
		{
			super(propertyPath, duration);
			
			if (path == null)
				throw new Error("'path' cannot be null.");
			_path = path;	
		}
		
		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			var totalTime	: int = this.duration;
			if (totalTime <= 0)
				return;
			
			var time : Number = (t < 0.0 ? totalTime + t : t) / totalTime; 
			time = time > 1.0 ? 1.0 : time;
			
			// change matrix value.
			var out : Matrix4x4 = _currentTarget[_propertyName];
			
			if (!out)
			{
				throw new Error(
					"'" + _propertyName + "' could not be found in '" + _currentTarget + "'."
				);
			}
			
			_path.updateMatrix(time, out);
		}
		
		override public function clone() : ITimeline
		{
			return new SplineMatrixTimeline(
				new String(_propertyName), 
				this.duration, 
				_path.clone() as Path
			);
		}
	}
}