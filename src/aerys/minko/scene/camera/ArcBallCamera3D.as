package aerys.minko.scene.camera
{
	import aerys.common.Factory;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.Scene3DVisitor;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	
	public class ArcBallCamera3D extends AbstractCamera3D
	{
		private static const MATRIX3D		: Factory	= Factory.getFactory(Matrix3D);
		private static const PI_DIV_2		: Number	= Math.PI / 2.;
		private static const PI_2			: Number	= Math.PI * 2.;
		private static const EPSILON		: Number	= .0000001;
		
		private var _distance	: Number	= 1.;
		private var _rotation	: Vector3D	= new Vector3D();
		
		public function ArcBallCamera3D()
		{
			super();
			
			lookAtX = lookAtY = lookAtZ = 0.;
		}
		
		//{ region getters/setters
		public function get distance() : Number { return _distance; }
		
		public function set distance(value : Number) : void
		{
			if (value != _distance)
				_distance = value;
		}
		
		public function set rotationX(value : Number) : void
		{
			if (value != _rotation.x)
				_rotation.x = value;
		}
		
		public function set rotationY(value : Number) : void
		{
			if (value != _rotation.y)
				_rotation.y = value;
		}
		
		public function set rotationZ(value : Number) : void
		{
			if (value != _rotation.z)
				_rotation.z = value;
		}
		
		public function get rotationX() : Number { return _rotation.x; }
		public function get rotationY() : Number { return _rotation.y; }
		public function get rotationZ() : Number { return _rotation.z; }
		//} endregion
		
		//{ region methods
		override protected function invalidateTransform(myVisitor : IScene3DVisitor = null) : void
		{
			if (_rotation.x >= PI_DIV_2)
				_rotation.x = PI_DIV_2 - EPSILON;
			else if (_rotation.x <= -PI_DIV_2)
				_rotation.x = -PI_DIV_2 + EPSILON;
				
			if (_distance <= 0.)
				_distance = EPSILON;

			_position.x = lookAtX - _distance * Math.sin(_rotation.y) * Math.cos(_rotation.x);
			_position.y = lookAtY - _distance * Math.sin(_rotation.x);
			_position.z = lookAtZ - _distance * Math.cos(_rotation.y) * Math.cos(_rotation.x);
			
			super.invalidateTransform(myVisitor);
		}
		//} endregion
	}
}