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
		}
		
		override public function set end(v : Vector4) : void
		{
			super.end = v;
			_dirty = true;
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
					0
				);
			
			_dirty = true;
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
					0
				);
			
			_dirty = true;
		}
		
		public function CubicSegment(start		: Vector4,
									 end		: Vector4,
									 previous	: Vector4 = null,
									 next		: Vector4 = null)
		{
			super(start, end);
			
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
			else if (_tmpPosT == t)
				return;
			
			var term1 : Number = t * t * t;
			var term2 : Number = t * t;
			var term3 : Number = t;
			// term4 = 1
			
			_tmpPos.set(
				term1 * _a0.x + term2 * _a1.x + term3 * _a2.x + _a3.x, 
				term1 * _a0.y + term2 * _a1.y + term3 * _a2.y + _a3.y, 
				term1 * _a0.z + term2 * _a1.z + term3 * _a2.z + _a3.z, 
				0
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			else if (_tmpTangentT == t)
				return;
			
			var dTerm1_dT : Number = 3 * t * t;
			var dTerm2_dT : Number = 2 * t;
			// dTerm3_dT = 1; dTerm4_dT = 0
			
			_tmpTangent.set(
				dTerm1_dT * _a0.x + dTerm2_dT * _a1.x + _a2.x, 
				dTerm1_dT * _a0.y + dTerm2_dT * _a1.y + _a2.y, 
				dTerm1_dT * _a0.z + dTerm2_dT * _a1.z + _a2.z, 
				0
			).normalize();
			
			_tmpTangentT = t;
		}
		
		override protected function updatePointAt(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			else if (_tmpPointAtT == t)
				return;
			
			updatePosition(t);
			updateTangent(t);
			
			_tmpPointAt.set(
				_tmpPos.x - _tmpTangent.x,
				_tmpPos.y - _tmpTangent.y,
				_tmpPos.z - _tmpTangent.z,
				0
			);
			
			_tmpPointAtT = t;
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
				0
			);
			
			_a1.set(
				y0.x - y1.x - _a0.x,
				y0.y - y1.y - _a0.y,
				y0.z - y1.z - _a0.z,
				0
			);
			
			_a2.set(
				y2.x - y0.x,
				y2.y - y0.y,
				y2.z - y0.z,
				0
			);
			
			_a3.set(y1.x, y1.y, y1.z, 0);
			
			_dirty = false;
		}
	}
}