package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 * @see Interpolation methods, by Paul Bourke <http://paulbourke.net/miscellaneous/interpolation/>
	 */
	public class CosineSegment extends AbstractSegment
	{
		
		public function CosineSegment(start	: Vector4, 
									  end	: Vector4)
		{
			super(start, end);
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_tmpPosT == t)
				return;
			
			var t2		: Number = .5 * (1 - Math.cos(t * Math.PI));
			var term1 : Number = 1 - t2;
			var term2 : Number = t2;
			
			_tmpPos.set(
				term1 * _start.x + term2 * _end.x,
				term1 * _start.y + term2 * _end.y,
				term1 * _start.z + term2 * _end.z,
				0
			);
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_tmpTangentT == t)
				return;
			
			var dT2_dt	: Number = .5 * Math.PI * Math.sin(Math.PI * t);
			var dTerm1_dT : Number = - dT2_dt;
			var dTerm2_dT : Number = dT2_dt;
			
			_tmpTangent.set(
				dTerm1_dT * _start.x + dTerm2_dT * _end.x,
				dTerm1_dT * _start.y + dTerm2_dT * _end.y,
				dTerm1_dT * _start.z + dTerm2_dT * _end.z,
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