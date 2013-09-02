package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;
	
	public class BezierQuadSegment extends AbstractBezierSegment
	{
		protected var	_control	: Vector4;
		
		private var		_diff10		: Vector4	= null;	
		private var		_diff21		: Vector4	= null;
		
		override public function get firstControl() : Vector4
		{
			return _control;
		}
		
		override public function get lastControl() : Vector4
		{
			return _control;
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
		
		public function set control(value : Vector4) : void
		{
			_control	= value;
			_length		= NaN;
			_dirty		= true;
		}
		
		public function BezierQuadSegment(start		: Vector4, 
										  control	: Vector4,
										  end		: Vector4,
										  at		: Vector4 = null,
										  up		: Vector4 = null)
		{
			super(start, end, at, up);
			_control = control;
			
			_diff10 = new Vector4();
			_diff21	= new Vector4();
			
			_dirty = true;
		}
		
		override public function clone() : IInterpolation
		{
			return new BezierQuadSegment(
				_start.clone(), 
				_control.clone(), 
				_end.clone(), 
				_at.clone(), 
				_up.clone()
			);
		}
		
		override protected function updatePrecomputedTerms() : void
		{
			Vector4.subtract(_control,	_start,		_diff10);
			Vector4.subtract(_end,		_control,	_diff21);
			
			_dirty = false;
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_dirty)
				updatePrecomputedTerms();
			
			if (Math.abs(t - _tmpPosT) < 1e-6)
				return;
			
			_tmpPos.set(
				_start.x + t * ( 2 * _diff10.x + t * (_diff21.x - _diff10.x) ),
				_start.y + t * ( 2 * _diff10.y + t * (_diff21.y - _diff10.y) ),
				_start.z + t * ( 2 * _diff10.z + t * (_diff21.z - _diff10.z) ),
				_start.w + t * ( 2 * _diff10.w + t * (_diff21.w - _diff10.w) )
			);
			/*
			var term1 : Number = (1 - t) * (1 - t);
			var term2 : Number = 2 * t * (1 - t);
			var term3 : Number = t * t;
			
			_tmpPos.set(
				term1 * _start.x + term2 * _control.x + term3 * _end.x,
				term1 * _start.y + term2 * _control.y + term3 * _end.y,
				term1 * _start.z + term2 * _control.z + term3 * _end.z,
				term1 * _start.w + term2 * _control.w + term3 * _end.w
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
				2 * (_diff10.x + t * (_diff21.x - _diff10.x)),
				2 * (_diff10.y + t * (_diff21.y - _diff10.y)),
				2 * (_diff10.z + t * (_diff21.z - _diff10.z)),
				2 * (_diff10.w + t * (_diff21.w - _diff10.w))
			);
			
			/*
			var dTerm1_dT : Number = - 2 * (1 - t);
			var dTerm2_dT : Number = 2 * (1 - 2 * t);
			var dTerm3_dT : Number = 2 * t;
			
			_tmpTangent.set(
				dTerm1_dT * _start.x + dTerm2_dT * _control.x + dTerm3_dT * _end.x,
				dTerm1_dT * _start.y + dTerm2_dT * _control.y + dTerm3_dT * _end.y,
				dTerm1_dT * _start.z + dTerm2_dT * _control.z + dTerm3_dT * _end.z,
				dTerm1_dT * _start.w + dTerm2_dT * _control.w + dTerm3_dT * _end.w
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