package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Bézier curve, Wikipedia <http://en.wikipedia.org/wiki/Bezier_curve>
	 */
	public class BezierCubicSegment extends AbstractSegment
	{
		protected var _control1	: Vector4;
		protected var _control2	: Vector4;
		
		public function set control1(value : Vector4) : void
		{
			_control1 = value;
		}
		
		public function set control2(value : Vector4) : void
		{
			_control2 = value;
		}
		
		public function BezierCubicSegment(start	: Vector4, 
										   control1	: Vector4,
										   control2	: Vector4, 
										   end		: Vector4)
		{
			super(start, end);
			
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
				0
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
				0 
			).normalize();
			
			_tmpTangentT = t;
		}
		
		override protected function updatePointAt(t : Number) : void
		{
			if (_tmpPointAtT == t)
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
	}
}
