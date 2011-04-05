package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Vector4;

	public class AbstractBezierSegment extends AbstractSegment
	{
		public function get firstControl() : Vector4
		{
			throw new Error('Must be overriden');
		}
		
		public function get lastControl() : Vector4
		{
			throw new Error('Must be overriden');
		}
		
		public function AbstractBezierSegment(start	: Vector4, 
											  end	: Vector4, 
											  at	: Vector4 = null, 
											  up	: Vector4 = null) 
		{
			super(start, end, at, up);
		}
	}
}
