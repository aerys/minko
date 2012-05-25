package aerys.minko.type.math
{
	public final class Ray
	{
		private var _origin		: Vector4	= null;
		private var _direction	: Vector4	= null;
		
		public function get origin() : Vector4
		{
			return _origin;
		}
		
		public function get direction() : Vector4
		{
			return _direction;
		}
		
		public function Ray(origin		: Vector4	= null,
							direction	: Vector4 	= null)
		{
			_origin = origin || new Vector4();
			_direction = direction || new Vector4();
		}
	}
}