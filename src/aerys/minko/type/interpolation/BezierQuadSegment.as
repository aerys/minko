package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Bézier curve, Wikipedia <http://en.wikipedia.org/wiki/Bezier_curve>
	 */
	public class BezierQuadSegment extends AbstractBezierSegment
	{
		protected var _control : Vector4;
		
		override public function get firstControl() : Vector4
		{
			return _control;
		}
		
		override public function get lastControl() : Vector4
		{
			return _control;
		}
		
		public function set control(value : Vector4) : void
		{
			_control = value;
			_length = NaN;
		}
		
		public function BezierQuadSegment(start		: Vector4, 
										  control	: Vector4,
										  end		: Vector4,
										  at		: Vector4 = null,
										  up		: Vector4 = null)
		{
			super(start, end, at, up);
			_control = control;
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_tmpPosT == t)
				return;
			
			var term1 : Number = (1 - t) * (1 - t);
			var term2 : Number = 2 * t * (1 - t);
			var term3 : Number = t * t;
			
			_tmpPos.set(
				term1 * _start.x + term2 * _control.x + term3 * _end.x,
				term1 * _start.y + term2 * _control.y + term3 * _end.y,
				term1 * _start.z + term2 * _control.z + term3 * _end.z,
				term1 * _start.w + term2 * _control.w + term3 * _end.w
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_tmpTangentT == t)
				return;
			
			var dTerm1_dT : Number = - 2 * (1 - t);
			var dTerm2_dT : Number = 2 * (1 - 2 * t);
			var dTerm3_dT : Number = 2 * t;
			
			_tmpTangent.set(
				dTerm1_dT * _start.x + dTerm2_dT * _control.x + dTerm3_dT * _end.x,
				dTerm1_dT * _start.y + dTerm2_dT * _control.y + dTerm3_dT * _end.y,
				dTerm1_dT * _start.z + dTerm2_dT * _control.z + dTerm3_dT * _end.z,
				dTerm1_dT * _start.w + dTerm2_dT * _control.w + dTerm3_dT * _end.w
			).normalize();
			
			_tmpTangentT = t;
		}
		
	}
}