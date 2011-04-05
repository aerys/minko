package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	import spark.effects.easing.Power;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class LinearSegment extends AbstractBezierSegment
	{
		protected var _dirty : Boolean; 
		
		
		override public function get firstControl() : Vector4
		{
			return _end;
		}
		
		override public function get lastControl() : Vector4
		{
			return _start;
		}
		
		/**
		 * Length of the segment
		 */
		override public function get length() : Number
		{
			if (_dirty)
				update();
			
			return _length;
		}
		
		override public function set start(value : Vector4) : void
		{
			super.start = value;
			_dirty = true;
			_length = NaN;
		}
		
		override public function set end(value : Vector4) : void
		{
			super.end = value;
			_dirty = true;
			_length = NaN;
		}
		
		public function LinearSegment(start	: Vector4,
									  end	: Vector4,
									  at	: Vector4 = null,
									  up	: Vector4 = null)
		{
			super(start, end, at, up);
			
			_start			= start;
			_end			= end;
			_dirty			= true;
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_tmpPosT == t)
				return;
			
			var term1 : Number = 1 - t;
			_tmpPos.set(
				term1 * _start.x + t * _end.x,
				term1 * _start.y + t * _end.y,
				term1 * _start.z + t * _end.z,
				term1 * _start.w + t * _end.w
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_dirty)
				update();
			
			_tmpTangentT = t;
		}
		
		protected function update() : void
		{
			_tmpTangent	= Vector4.subtract(_end, _start, _tmpTangent).normalize();
			_length		= Math.sqrt(
				Math.pow(_end.x - _start.x, 2) +
				Math.pow(_end.y - _start.y, 2) +
				Math.pow(_end.z - _start.z, 2)
			);
		}

	}
}