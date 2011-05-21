package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Bézier curve, Wikipedia <http://en.wikipedia.org/wiki/Bezier_curve>
	 */
	public class BezierCubicSegment extends AbstractBezierSegment
	{
		protected var _control1	: Vector4;
		protected var _control2	: Vector4;
		
		override public function get firstControl() : Vector4
		{
			return _control1;
		}
		
		override public function get lastControl() : Vector4
		{
			return _control2;
		}
		
		public function set control1(value : Vector4) : void
		{
			_control1 = value;
			_length = NaN;
		}
		
		public function set control2(value : Vector4) : void
		{
			_control2 = value;
			_length = NaN;
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
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_tmpPosT == t)
				return;
			
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
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_tmpTangentT == t)
				return;
			
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
			
			_tmpTangentT = t;
		}
		
	}
}
