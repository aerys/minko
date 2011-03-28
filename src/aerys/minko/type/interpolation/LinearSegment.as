package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class LinearSegment extends AbstractSegment
	{
		protected var _tangentDirty : Boolean; 
		
		override public function set start(value : Vector4) : void
		{
			super.start = value;
			_tangentDirty = true;
		}
		
		override public function set end(value : Vector4) : void
		{
			super.end = value;
			_tangentDirty = true;
		}
		
		public function LinearSegment(start	: Vector4,
									  end	: Vector4)
		{
			super(start, end);
			
			_start			= start;
			_end			= end;
			_tangentDirty	= true;
		}
		
		override protected function updatePosition(t : Number) : void
		{
			if (_tmpPosT == t)
				return;
			
			var term1 : Number = 1 - t;
			_tmpPos.set(
				term1 * _start.x + t * _end.x,
				term1 * _start.y + t * _end.y,
				term1 * _start.z + t * _end.z,
				0
			);
			
			_tmpPosT = t;
		}
		
		override protected function updateTangent(t : Number) : void
		{
			if (_tangentDirty)
				_tmpTangent	= Vector4.subtract(_end, _start, _tmpTangent).normalize();
			
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