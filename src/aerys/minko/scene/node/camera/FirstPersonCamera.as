package aerys.minko.scene.node.camera
{
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * FirstPersonCamera3D objects represent a "first-person" camera.
	 * 
	 * The look-at point of such camera is computed according to the position
	 * of the camera and the rotation property.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class FirstPersonCamera extends Camera
	{
		private static const EPSILON		: Number	= .001;
		private static const MAX_ROTATION_X	: Number	= Math.PI / 2. - EPSILON;
		private static const MIN_ROTATION_X	: Number	= -MAX_ROTATION_X;
		
		private var _version	: uint		= 0;
		private var _ghostMode	: Boolean	= false;
		private var _rotation	: Vector4	= new Vector4();
		
		override public function get version():uint
		{
			return super.version + _rotation.version + _version;
		}
		
		public function get ghostMode() : Boolean
		{
			return _ghostMode;
		}
		
		public function set ghostMode(value : Boolean) : void
		{
			_ghostMode = value;
			++_version;
		}
		
		public function get rotation() : Vector4
		{
			return _rotation;
		}
		
		public function FirstPersonCamera(ghostMode : Boolean = false)
		{
			super();
			
			_ghostMode = ghostMode;
			
			_rotation.y = Math.PI / 2.;
		}
		
		override public function getData(localData : LocalData) : IWorldData
		{
			if (_rotation.x >= MAX_ROTATION_X)
				_rotation.x = MAX_ROTATION_X;
			else if (_rotation.x <= MIN_ROTATION_X)
				_rotation.x = MIN_ROTATION_X;
			
			lookAt.x = position.x + Math.cos(_rotation.y) * Math.cos(_rotation.x);
			lookAt.y = position.y + Math.sin(_rotation.x);
			lookAt.z = position.z + Math.sin(_rotation.y) * Math.cos(_rotation.x);
			
			return super.getData(localData);
		}
		
		public function walk(distance : Number) : void
		{
			if (_ghostMode)
			{
				position.x += Math.cos(_rotation.y) * Math.cos(-_rotation.x) * distance;
				position.y += Math.sin(_rotation.x) * distance;
				position.z += Math.sin(_rotation.y) * Math.cos(-_rotation.x) * distance;
			}
			else
			{
				position.x += Math.cos(_rotation.y) * distance;
				position.z += Math.sin(_rotation.y) * distance;
			}
		}
		
		public function strafe(distance : Number) : void
		{
			position.x += Math.cos(_rotation.y + Math.PI / 2) * -distance;
			position.z += Math.sin(_rotation.y + Math.PI / 2) * -distance;
		}
	}
}