package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class AbstractSegment
	{
		protected var _start		: Vector4;
		protected var _end			: Vector4;
		
		protected var _tmpPosT		: Number;
		protected var _tmpTangentT	: Number;
		protected var _tmpPointAtT	: Number;
		
		protected var _tmpPos		: Vector4;
		protected var _tmpTangent	: Vector4;
		protected var _tmpPointAt	: Vector4;
		
		public function set start(value : Vector4) : void
		{
			_start = value;
		}
		
		public function set end(value : Vector4) : void
		{
			_end = value;
		}
		
		public function AbstractSegment(start : Vector4, end : Vector4)
		{
			_start	= start;
			_end	= end;
			
			_tmpPosT = _tmpTangentT = _tmpPointAtT = -1;
			
			_tmpPos		= new Vector4();
			_tmpTangent	= new Vector4();
			_tmpPointAt	= new Vector4();
		}
		
		public function position(t : Number, out : Vector4) : Vector4
		{
			updatePosition(t);
			return Vector4.copy(_tmpPos, out);
		}
		
		public function translation(t : Number, out : Vector4) : Vector4
		{
			return position(t, out);
		}
		
		public function tangent(t : Number, out : Vector4) : Vector4
		{
			updateTangent(t);
			return Vector4.copy(_tmpTangent, out);
		}
		
		public function lookAt(t : Number, out : Vector4) : Vector4
		{
			return tangent(t, out);
		}
		
		public function pointAt(t : Number, out : Vector4) : Vector4
		{
			updatePointAt(t);
			return Vector4.copy(_tmpTangent, out);
		}
		
		public function setTransform(transform : Transform3D, t : Number) : void
		{
			updatePosition(t);
			updatePointAt(t);
			
			Vector4.copy(_tmpPos, transform.position);
			transform.pointAt(_tmpPointAt, new Vector4(0, 0, -1), new Vector4(0, -1, 0));
		}
		
		protected function updatePosition(t : Number) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function updateTangent(t : Number) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function updatePointAt(t : Number) : void
		{
			throw new Error('Must be overriden');
		}
	}
}
