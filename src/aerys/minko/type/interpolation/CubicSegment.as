package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Interpolation methods, by Paul Bourke <http://paulbourke.net/miscellaneous/interpolation/>
	 */
	public class CubicSegment extends AbstractSegment
	{
		protected var _dirty		: Boolean;
		
		protected var _previous 	: Vector4;
		protected var _next			: Vector4;
		
		protected var _a0			: Vector4;
		protected var _a1			: Vector4;
		protected var _a2			: Vector4;
		protected var _a3			: Vector4;
		
		override public function set start(v : Vector4) : void
		{
			super.start = v;
			_dirty = true;
			_length = NaN;
		}
		
		override public function set end(v : Vector4) : void
		{
			super.end = v;
			_dirty = true;
			_length = NaN;
		}
		
		public function set previous(v : Vector4) : void
		{
			if (v != null)
				_previous = v;
			else
				_previous = new Vector4(
					2 * _start.x - _end.x,
					2 * _start.y - _end.y,
					2 * _start.z - _end.z,
					2 * _start.w - _end.w
				);
			
			_dirty = true;
			_length = NaN;
		}
		
		public function set next(v : Vector4) : void
		{
			if (v != null)
				_next = v;
			else
				_next = new Vector4(
					2 * _end.x - _start.x,
					2 * _end.y - _start.y,
					2 * _end.z - _start.z,
					2 * _end.w - _start.w
				);
			
			_dirty = true;
			_length = NaN;
		}
		
		public function CubicSegment(start		: Vector4,
									 end		: Vector4,
									 previous	: Vector4 = null,
									 next		: Vector4 = null,
									 at			: Vector4 = null,
									 up			: Vector4 = null)
		{
			super(start, end, at, up);
			
			_dirty		= true;
			
			this.previous	= previous;
			this.next		= next;
			
			_a0			= new Vector4();
			_a1			= new Vector4();
			_a2			= new Vector4();
			_a3			= new Vector4();
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (_tmpPosT == t)
				return;
			
			var term1 : Number = t * t * t;
			var term2 : Number = t * t;
			// term3 = t, term4 = 1
			
			_tmpPos.set(
				term1 * _a0.x + term2 * _a1.x + t * _a2.x + _a3.x, 
				term1 * _a0.y + term2 * _a1.y + t * _a2.y + _a3.y, 
				term1 * _a0.z + term2 * _a1.z + t * _a2.z + _a3.z, 
				term1 * _a0.w + term2 * _a1.w + t * _a2.w + _a3.w
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (_tmpTangentT == t)
				return;
			
			var dTerm1_dT : Number = 3 * t * t;
			var dTerm2_dT : Number = 2 * t;
			// dTerm3_dT = 1; dTerm4_dT = 0
			
			_tmpTangent.set(
				dTerm1_dT * _a0.x + dTerm2_dT * _a1.x + _a2.x, 
				dTerm1_dT * _a0.y + dTerm2_dT * _a1.y + _a2.y, 
				dTerm1_dT * _a0.z + dTerm2_dT * _a1.z + _a2.z, 
				dTerm1_dT * _a0.w + dTerm2_dT * _a1.w + _a2.w
			).normalize();
			
			_tmpTangentT = t;
		}
		
		override protected function updatePointAt(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			super.updatePointAt(t);
		}
		
		protected function updatePrecomputedTerms() : void
		{
			var y0 : Vector4 = _previous;
			var y1 : Vector4 = _start;
			var y2 : Vector4 = _end;
			var y3 : Vector4 = _next;
			
			_a0.set(
				y3.x - y2.x - y0.x + y1.x,
				y3.y - y2.y - y0.y + y1.y,
				y3.z - y2.z - y0.z + y1.z,
				y3.w - y2.w - y0.w + y1.w
			);
			
			_a1.set(
				y0.x - y1.x - _a0.x,
				y0.y - y1.y - _a0.y,
				y0.z - y1.z - _a0.z,
				y0.w - y1.w - _a0.w
			);
			
			_a2.set(
				y2.x - y0.x,
				y2.y - y0.y,
				y2.z - y0.z,
				y2.w - y0.w
			);
			
			_a3.set(y1.x, y1.y, y1.z, 0/*y1.w*/);
			
			_dirty			= false;
			_tmpPosT		= -1;
			_tmpPointAtT	= -1;
			_tmpTangentT	= -1;
			_length			= NaN;
		}
	}
}
