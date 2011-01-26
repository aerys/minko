package aerys.minko.scene.camera
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Vector3D;
	
	public class FirstPersonCamera3D extends AbstractCamera3D
	{
		private static const EPSILON		: Number	= .001;
		private static const MAX_ROTATION_X	: Number	= Math.PI / 2. - EPSILON;
		private static const MIN_ROTATION_X	: Number	= -MAX_ROTATION_X;
		
		private var _ghostMode	: Boolean	= false;
		private var _rotation	: Vector4	= new Vector4();
		
		public function get ghostMode() : Boolean
		{
			return _ghostMode;
		}
		
		public function set ghostMode(value : Boolean) : void
		{
			_ghostMode = value;
		}
		
		public function get rotation() : Vector4
		{
			return _rotation;
		}
		
		public function FirstPersonCamera3D(ghostMode : Boolean = false)
		{
			super();
			
			_ghostMode = ghostMode;
		}
		
		override protected function invalidateTransform(query : RenderingQuery = null) : void
		{
			if (_rotation.x >= MAX_ROTATION_X)
				_rotation.x = MAX_ROTATION_X;
			else if (_rotation.x <= MIN_ROTATION_X)
				_rotation.x = MIN_ROTATION_X;
			
			lookAt.x = position.x + Math.sin(_rotation.y) * Math.cos(_rotation.x);
			lookAt.y = position.y + Math.sin(_rotation.x);
			lookAt.z = position.z + Math.cos(_rotation.y) * Math.cos(_rotation.x);
			
			super.invalidateTransform(query);
		}
		
		public function walk(distance : Number) : void
		{
			if (_ghostMode)
			{
				position.y += Math.sin(_rotation.x) * distance;
				position.x += Math.sin(_rotation.y) * Math.cos(-_rotation.x) * distance;
				position.z += Math.cos(_rotation.y) * Math.cos(-_rotation.x) * distance;
			}
			else
			{
				position.x += Math.sin(_rotation.y) * distance;
				position.z += Math.cos(_rotation.y) * distance;
			}
		}
		
		public function strafe(distance : Number) : void
		{
			position.x += Math.sin(_rotation.y + Math.PI / 2) * distance;
			position.z += Math.cos(_rotation.y + Math.PI / 2) * distance;
		}
		/* ! METHODS */

	}
}