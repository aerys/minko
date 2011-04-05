package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Interpolation methods, by Paul Bourke <http://paulbourke.net/miscellaneous/interpolation/>
	 */
	public class HermiteSegment extends AbstractSegment
	{
		protected var _dirty		: Boolean;
		
		protected var _previous 	: Vector4;
		protected var _next			: Vector4;
		
		protected var _tension		: Number;
		protected var _bias			: Number;
		
		protected var _m0			: Vector4;
		protected var _m1			: Vector4;
		
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
			if (v !== null)
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
		
		public function set bias(v : Number) : void
		{
			_bias = v;
			_dirty = true;
			_length = NaN;
		}
		
		public function set tension(v : Number) : void
		{
			if (v < -1 || v > 1)
				throw new Error('Tension value must be between -1 and 1.');
			
			_tension = v;
			_dirty = true;
			_length = NaN;
		}
		
		public function HermiteSegment(start	: Vector4,
									   end		: Vector4,
									   bias		: Number,
									   tension	: Number,
									   previous	: Vector4 = null,
									   next		: Vector4 = null,
									   at		: Vector4 = null,
									   up		: Vector4 = null)
		{
			super(start, end, at, up);
			
			if (tension < -1 || tension > 1)
				throw new Error('Tension value must be between -1 and 1.');
			
			_dirty	 	= true;
			
			this.previous	= previous;
			this.next		= next;
			
			_bias		= bias;
			_tension	= tension;
			
			_m0			= new Vector4();
			_m1			= new Vector4();
			
			_tmpPos		= new Vector4();
			_tmpTangent	= new Vector4();
			_tmpPointAt	= new Vector4();
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (_tmpPosT == t)
				return;
			
			var t2 : Number = t * t;
			var t3 : Number = t2 * t;
			
			var m0 : Vector4 = _m0;
			var m1 : Vector4 = _m1;
			
			var a0 : Number = 2 * t3 - 3 * t2 + 1;
			var a1 : Number = t3 - 2 * t2 + t;
			var a2 : Number = t3 - t2;
			var a3 : Number = -2 * t3 + 3 * t2;
			
			_tmpPos.set(
				a0 * _start.x + a1 * _m0.x + a2 * _m1.x + a3 * _end.x,
				a0 * _start.y + a1 * _m0.y + a2 * _m1.y + a3 * _end.y,
				a0 * _start.z + a1 * _m0.z + a2 * _m1.z + a3 * _end.z,
				a0 * _start.w + a1 * _m0.w + a2 * _m1.w + a3 * _end.w
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (_tmpTangentT == t)
				return;
			
			var t2 : Number = t * t;
			var t3 : Number = t2 * t;
			
			var dA0_dT : Number = 6 * t2 - 6 * t;
			var dA1_dT : Number = 3 * t2 - 4 * t + 1;
			var dA2_dT : Number = 3 * t2 - 2 * t;
			var dA3_dT : Number = -6 * t2 + 6 * t;
			
			_tmpTangent.set(
				dA0_dT * _start.x + dA1_dT * _m0.x + dA2_dT * _m1.x + dA3_dT * _end.x,
				dA0_dT * _start.y + dA1_dT * _m0.y + dA2_dT * _m1.y + dA3_dT * _end.y,
				dA0_dT * _start.z + dA1_dT * _m0.z + dA2_dT * _m1.z + dA3_dT * _end.z,
				dA0_dT * _start.w + dA1_dT * _m0.w + dA2_dT * _m1.w + dA3_dT * _end.w
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
			_m0.set(
				0.5 * ((_start.x - _previous.x) * (1 + _bias) * (1 - _tension) + (_end.x - _start.x) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_start.y - _previous.y) * (1 + _bias) * (1 - _tension) + (_end.y - _start.y) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_start.z - _previous.z) * (1 + _bias) * (1 - _tension) + (_end.z - _start.z) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_start.w - _previous.w) * (1 + _bias) * (1 - _tension) + (_end.w - _start.w) * (1 - _bias) * (1 - _tension))
			);
			
			_m1.set(
				0.5 * ((_end.x - _start.x) * (1 + _bias) * (1 - _tension) + (_next.x - _end.x) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_end.y - _start.y) * (1 + _bias) * (1 - _tension) + (_next.y - _end.y) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_end.z - _start.z) * (1 + _bias) * (1 - _tension) + (_next.z - _end.z) * (1 - _bias) * (1 - _tension)),
				0.5 * ((_end.w - _start.w) * (1 + _bias) * (1 - _tension) + (_next.w - _end.w) * (1 - _bias) * (1 - _tension))
			);
			
			_dirty			= false;
			_tmpPointAtT	= -1;
			_tmpPosT		= -1;
			_tmpTangentT	= -1;
			_length			= NaN;
		}
	}
}
