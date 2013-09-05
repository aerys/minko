package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;
	
	public class BezierCubicSegment extends AbstractBezierSegment
	{
		protected var _control1	: Vector4;
		protected var _control2	: Vector4;
		
		private var _diff10 : Vector4 = null;
		private var _diff21 : Vector4 = null;
		private var _diff30 : Vector4 = null;
		
		
		override public function get firstControl() : Vector4
		{
			return _control1;
		}
		
		override public function get lastControl() : Vector4
		{
			return _control2;
		}
		
		override public function set start(value : Vector4) : void
		{
			super._start	= value;
			_length			= NaN;
			_dirty			= true;
		}
		
		override public function set end(value : Vector4) : void
		{
			super._end	= value;
			_length		= NaN;
			_dirty		= true;
		}
		
		public function set control1(value : Vector4) : void
		{
			_control1	= value;
			_length		= NaN;
			_dirty		= true;
		}
		
		public function set control2(value : Vector4) : void
		{
			_control2	= value;
			_length		= NaN;
			_dirty		= true;
		}
		
		public function BezierCubicSegment(start	: Vector4, 
										   control1	: Vector4,
										   control2	: Vector4, 
										   end		: Vector4,
										   at		: Vector4 = null, 
										   up		: Vector4 = null)
		{
			super(start, end, at, up);
			
			_control1 = control1;
			_control2 = control2;
			
			_diff10 = new Vector4();
			_diff21 = new Vector4();
			_diff30 = new Vector4();
			
			_dirty = true;
		}
		
		override public function clone() : IInterpolation
		{
			return new BezierCubicSegment(
				_start.clone(), 
				_control1.clone(), 
				_control2.clone(), 
				_end.clone(), 
				_at.clone(), 
				_up.clone()
			);
		}
		
		override protected function updatePrecomputedTerms() : void
		{
			Vector4.subtract(_control1,	_start,		_diff10);
			Vector4.subtract(_control2,	_control1,	_diff21);
			Vector4.subtract(_end,		_start,		_diff30);
			
			_dirty = false;
			_tmpPosT		= -1;
			_tmpPointAtT	= -1;
			_tmpTangentT	= -1;
			_length			= NaN;
		}
			
		override protected function updatePosition(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (Math.abs(t - _tmpPosT) < 1e-6)
				return;
			
			_tmpPos.set(
				_start.x + t * ( 3 * _diff10.x + t * ( 3 * (_diff21.x - _diff10.x) + t * ( _diff30.x - 3 * _diff21.x ) ) ),
				_start.y + t * ( 3 * _diff10.y + t * ( 3 * (_diff21.y - _diff10.y) + t * ( _diff30.y - 3 * _diff21.y ) ) ),
				_start.z + t * ( 3 * _diff10.z + t * ( 3 * (_diff21.z - _diff10.z) + t * ( _diff30.z - 3 * _diff21.z ) ) ),
				_start.w + t * ( 3 * _diff10.w + t * ( 3 * (_diff21.w - _diff10.w) + t * ( _diff30.w - 3 * _diff21.w ) ) )
			);
			
			/*
			var term1	: Number = (1 - t) * (1 - t) * (1 - t);
			var term2	: Number = 3 * t * (1 - t) * (1 - t);
			var term3	: Number = 3 * t * t * (1 - t);
			var term4	: Number = t * t * t * t;
			
			_tmpPos.set(
				term1 * _start.x + term2 * _control1.x + term3 * _control2.x + term4 * _end.x,
				term1 * _start.y + term2 * _control1.y + term3 * _control2.y + term4 * _end.y,
				term1 * _start.z + term2 * _control1.z + term3 * _control2.z + term4 * _end.z,
				term1 * _start.w + term2 * _control1.w + term3 * _control2.w + term4 * _end.w
			);
			*/
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (Math.abs(t - _tmpTangentT) < 1e-6)
				return;
			
			_tmpTangent.set(
				3 * ( _diff10.x + t * ( 2 * (_diff21.x - _diff10.x) + t * (_diff30.x - 3 * _diff21.x) ) ),
				3 * ( _diff10.y + t * ( 2 * (_diff21.y - _diff10.y) + t * (_diff30.y - 3 * _diff21.y) ) ),
				3 * ( _diff10.z + t * ( 2 * (_diff21.z - _diff10.z) + t * (_diff30.z - 3 * _diff21.z) ) ),
				3 * ( _diff10.w + t * ( 2 * (_diff21.w - _diff10.w) + t * (_diff30.w - 3 * _diff21.w) ) )
			).normalize();
			
			/*
			var dTerm1_dT : Number = - 3 * (1 - t) * (1 - t);
			var dTerm2_dT : Number = 6 * (1 - t);
			var dTerm3_dT : Number = 3 * t * (2 - 3 * t * t);
			var dTerm4_dT : Number = 3 * t * t;
			
			_tmpTangent.set(
				dTerm1_dT * _start.x + dTerm2_dT * _control1.x + dTerm3_dT * _control2.x + dTerm4_dT * _end.x,
				dTerm1_dT * _start.y + dTerm2_dT * _control1.y + dTerm3_dT * _control2.y + dTerm4_dT * _end.y,
				dTerm1_dT * _start.z + dTerm2_dT * _control1.z + dTerm3_dT * _control2.z + dTerm4_dT * _end.z,
				dTerm1_dT * _start.w + dTerm2_dT * _control1.w + dTerm3_dT * _control2.w + dTerm4_dT * _end.w
			).normalize();
			*/
			
			_tmpTangentT = t;
		}
		
		override protected function updatePointAt(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			super.updatePointAt(t);
		}
	}
}