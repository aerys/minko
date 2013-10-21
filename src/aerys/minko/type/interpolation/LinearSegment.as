package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;
	
	import flash.text.engine.LigatureLevel;
	
	public class LinearSegment extends AbstractSegment
	{
		protected var _dirty : Boolean; 
		
		public function get firstControl() : Vector4
		{
			return _end;
		}
		
		public function get lastControl() : Vector4
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
		
		override public function clone() : IInterpolation
		{
			return new LinearSegment(
				_start.clone(), 
				_end.clone(), 
				_at.clone(), 
				_up.clone()
			);			
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (Math.abs(t - _tmpPosT) < 1e-6)
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
			Vector4.subtract(_end, _start, _tmpTangent);
			
			_length		= _tmpTangent.length;
			_tmpTangent	= _tmpTangent.normalize();
		}
	}
}