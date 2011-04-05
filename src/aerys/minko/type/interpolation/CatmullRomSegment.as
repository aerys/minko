package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Interpolation methods, by Paul Bourke <http://paulbourke.net/miscellaneous/interpolation/>
	 * @see Cubic Hermite Spline, from wikipedia <http://en.wikipedia.org/wiki/Cubic_Hermite_spline>
	 */
	public class CatmullRomSegment extends CubicSegment
	{
		public function CatmullRomSegment(begin		: Vector4,
										  end		: Vector4, 
										  previous	: Vector4 = null, 
										  next		: Vector4 = null,
										  at		: Vector4 = null,
										  up		: Vector4 = null)
		{
			super(begin, end, previous, next);
		}
		
		override protected function updatePrecomputedTerms() : void
		{
			var y0 : Vector4 = _previous;
			var y1 : Vector4 = _start;
			var y2 : Vector4 = _end;
			var y3 : Vector4 = _next;
			
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
			
			_dirty			= false;
			_tmpPosT		= -1;
			_tmpPointAtT	= -1;
			_tmpTangentT	= -1;
			_length			= NaN;
		}
	}
}