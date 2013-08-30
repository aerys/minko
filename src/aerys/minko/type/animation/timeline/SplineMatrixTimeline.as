package aerys.minko.type.animation.timeline
{
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.interpolation.Path;
	import aerys.minko.type.math.Matrix4x4;

	public final class SplineMatrixTimeline extends AbstractTimeline
	{
		private var _path				: IInterpolation	= null;
		
		private var _flattenedAnimation	: MatrixTimeline	= null;
		private var _flattenAnimation	: Boolean			= false;
		private var	_numFps				: uint				= 0;
		private var _updateAnimation	: Boolean			= false;
		
		public function SplineMatrixTimeline(propertyPath		: String, 
											 duration			: uint,
											 path				: IInterpolation,
											 flattenAnimation	: Boolean	= false,
											 numFps				: uint		= 0)
		{
			super(propertyPath, duration);
			
			_flattenAnimation	= flattenAnimation;
			_numFps				= numFps;
			if (_flattenAnimation && _numFps == 0)
				throw new Error("'numFps' must be positive.");
			_updateAnimation	= _flattenAnimation;
			
			this.path = path;
		}
		
		public function set path(value	: IInterpolation) : void
		{
			if (value == null)
				throw new Error("'value' cannot be null.");
			
			_path				= value;
			_updateAnimation	= _flattenAnimation;
		}
		
		private function updateFlattenedAnimation() : void
		{
			var numFrames	: uint					= uint(this.duration * 1e-3 * _numFps); // duration is in milliseconds
			numFrames	= numFrames < 2 ? 2 : numFrames;
			
			var timetable	: Vector.<uint>			= new Vector.<uint>(numFrames, true);
			var matrices	: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>(numFrames, true);
			
			var paramStep	: Number				= 1.0 / (numFrames - 1);
			var timeStep	: Number				= this.duration * paramStep;
			
			var param		: Number 				= 0.0;
			var time		: Number				= 0.0;
			var matrix		: Matrix4x4				= new Matrix4x4();
			for (var frameId : uint = 0; frameId < numFrames; ++frameId)
			{
				_path.updateMatrix(param, matrix);
				
				timetable[frameId]	= uint(time);
				matrices[frameId]	= new Matrix4x4().copyFrom(matrix);
				
				param	+= paramStep;
				time	+= timeStep;
			}
			
			_flattenedAnimation	= new MatrixTimeline(
				this.propertyPath,
				timetable,
				matrices,
				true
			);
			
			_updateAnimation = false;
		}
		
		override public function updateAt(t : int, target : Object) : void
		{
			super.updateAt(t, target);
			
			if (!_flattenAnimation)
			{
				// dynamic spline interpolation
				
				// change matrix value.
				var out : Matrix4x4 = _currentTarget[_propertyName];
				
				if (!out)
				{
					throw new Error(
						"'" + _propertyName + "' could not be found in '" + _currentTarget + "'."
					);
				}
				
				var totalTime	: uint = this.duration;
				if (totalTime == 0)
					return;
				
				var time : Number = (t < 0.0 ? totalTime + t : t) / totalTime; 
				time = time > 1.0 ? 1.0 : time;
				
				_path.updateMatrix(time, out);
			}
			else
			{
				// static spline interpolation
				if (_updateAnimation)
					updateFlattenedAnimation();
				
				_flattenedAnimation.updateAt(t, target);
			}
		}
		
		override public function clone() : ITimeline
		{
			return new SplineMatrixTimeline(
				new String(_propertyName), 
				this.duration, 
				_path.clone() as Path,
				_flattenAnimation,
				_numFps
			);
		}
	}
}