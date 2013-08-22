package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;
	
	public class CatmullRomSegment extends CubicSegment
	{
		public function CatmullRomSegment(start		: Vector4, 
										  end		: Vector4, 
										  previous	: Vector4	= null, 
										  next		: Vector4	= null, 
										  at		: Vector4	= null, 
										  up		: Vector4	= null)
		{
			super(start, end, previous, next, at, up);
		}
		
		override protected function updatePrecomputedTerms() : void
		{
			// Formula given the sequence of control points (P0, P1, P2, P3):
			// B(t) = [ 2 * P1 
			//          + t   * (P2 - P0)
			//          + t^2 * (2 * P0 - 5 * P1 + 4 * P2 - P3) 
			//          + t^3 * (-P0 + 3 * P1 - 3*P2 + P3)      ] / 2
			
			var y0 : Vector4 = _previous; // P0
			var y1 : Vector4 = _start;    // P1
			var y2 : Vector4 = _end;      // P2
			var y3 : Vector4 = _next;     // P3
			
			_a0.set(
				-0.5 * y0.x + 1.5 * y1.x - 1.5 * y2.x + 0.5 * y3.x,
				-0.5 * y0.y + 1.5 * y1.y - 1.5 * y2.y + 0.5 * y3.y,
				-0.5 * y0.z + 1.5 * y1.z - 1.5 * y2.z + 0.5 * y3.z,
				-0.5 * y0.w + 1.5 * y1.w - 1.5 * y2.w + 0.5 * y3.w
			);
			
			_a1.set(
				y0.x - 2.5 * y1.x + 2 * y2.x - 0.5 * y3.x,
				y0.y - 2.5 * y1.y + 2 * y2.y - 0.5 * y3.y,
				y0.z - 2.5 * y1.z + 2 * y2.z - 0.5 * y3.z,
				y0.w - 2.5 * y1.w + 2 * y2.w - 0.5 * y3.w
			);
			
			_a2.set(
				-0.5 * y0.x + 0.5 * y2.x,
				-0.5 * y0.y + 0.5 * y2.y,
				-0.5 * y0.z + 0.5 * y2.z,
				-0.5 * y0.w + 0.5 * y2.w
			);
			
			_a3.set(y1.x, y1.y, y1.z, 0);
			
			// B(t) = a0 * t^3 + a1 * t^2 + a2 * t + a3
			
			_dirty			= false;
			_tmpPosT		= -1;
			_tmpPointAtT	= -1;
			_tmpTangentT	= -1;
			_length			= NaN;
		}
	}
}