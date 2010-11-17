package aerys.minko.scene.camera
{
	import aerys.minko.render.IScene3DVisitor;
	
	import flash.geom.Vector3D;
	
	public class FirstPersonCamera3D extends AbstractCamera3D
	{
		private static const MAX_ROTATION_X	: Number	= Math.PI / 2. - .001;
		private static const MIN_ROTATION_X	: Number	= -MAX_ROTATION_X;
		
		private var _ghostMode	: Boolean	= false;
		private var _rotation	: Vector3D	= new Vector3D();
		
		public function get ghostMode() : Boolean
		{
			return _ghostMode;
		}
		
		public function set ghostMode(value : Boolean) : void
		{
			_ghostMode = value;
		}
		
		public function set rotationX(value : Number) : void
		{
			if (value != _rotation.x)
			{
				if (value > MAX_ROTATION_X)
					_rotation.x = MAX_ROTATION_X;
				else if (value < MIN_ROTATION_X)
					_rotation.x = MIN_ROTATION_X;
				else
					_rotation.x = value;
			}
		}
		
		public function set rotationY(value : Number) : void
		{
			_rotation.y = value;
		}
		
		public function set rotationZ(value : Number) : void
		{
			_rotation.z = value;
		}
		
		public function get rotationX() : Number { return _rotation.x; }
		public function get rotationY() : Number { return _rotation.y; }
		public function get rotationZ() : Number { return _rotation.z; }
		
		public function FirstPersonCamera3D(ghostMode : Boolean = false)
		{
			super();
			
			_ghostMode = ghostMode;
		}
		
		override protected function invalidateTransform(visitor : IScene3DVisitor = null) : void
		{
			lookAtX = _position.x + Math.sin(_rotation.y) * Math.cos(_rotation.x);
			lookAtY = _position.y + Math.sin(_rotation.x);
			lookAtZ = _position.z + Math.cos(_rotation.y) * Math.cos(_rotation.x);
			
			super.invalidateTransform(visitor);
		}
		
		public function walk(myDistance : Number) : void
		{
			if (_ghostMode)
			{
				_position.y += Math.sin(_rotation.x) * myDistance;
				_position.x += Math.sin(_rotation.y) * Math.cos(-_rotation.x) * myDistance;
				_position.z += Math.cos(_rotation.y) * Math.cos(-_rotation.x) * myDistance;
			}
			else
			{
				_position.x += Math.sin(_rotation.y) * myDistance;
				_position.z += Math.cos(_rotation.y) * myDistance;
			}
			
			_update = true;
		}
		
		public function strafe(myDistance : Number) : void
		{
			_position.x += Math.sin(_rotation.y + Math.PI / 2) * myDistance;
			_position.z += Math.cos(_rotation.y + Math.PI / 2) * myDistance;
			_update = true;
		}
		
		public function setPosition(myPosition : Vector3D) : void
		{
			_position.x = myPosition.x;
			_position.y = myPosition.y;
			_position.z = myPosition.z;
			_update = true;
		}
		/* ! METHODS */
		
	}
}