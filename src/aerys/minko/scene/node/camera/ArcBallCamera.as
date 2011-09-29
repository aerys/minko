package aerys.minko.scene.node.camera
{
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * ArcBallCamera3D objects represents a "third-person" camera that can move around a
	 * point in space.
	 * 
	 * The position of the camera will be computed according to the look-at point and the
	 * rotation angles given by the "rotation" property. Therefore, setting the
	 * (components of) the 'position' property will have no effect.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ArcBallCamera extends Camera
	{
		private static const EPSILON		: Number	= .001;
		private static const MAX_ROTATION_X	: Number	= Math.PI / 2. - EPSILON;
		private static const MIN_ROTATION_X	: Number	= -MAX_ROTATION_X;
		
		private var _rotationVersion	: uint			= uint(-1);
		private var _invalidPosition	: Boolean		= true;
		private var _lookAtVersion		: uint			= uint(-1);
		
		private var _distance			: Number		= 1.;
		private var _rotation			: Vector4		= new Vector4(0., Math.PI * .5, 0., 0.);
		
		override public function get version() : uint
		{
			return super.version + _rotation.version;
		}
		
		public function ArcBallCamera()
		{
			super();
			
			lookAt.set(0., 0., 0.);
		}
		
		/**
		 * The distance between the camera position and the look-at point.
		 * 
		 * @return 
		 * 
		 */
		public function get distance() : Number { return _distance; }
		
		public function set distance(value : Number) : void
		{
			_distance = value;
			if (_distance <= 0.)
				_distance = EPSILON;
			_invalidPosition = true;
		}
		
		/**
		 * The rotation (as 3 Euler angles) that will be used to compute
		 * the position according to its look-at point.
		 *  
		 * @return 
		 * 
		 */
		public function get rotation() : Vector4
		{
			/*if (_rotation.x >= MAX_ROTATION_X)
				_rotation.x = MAX_ROTATION_X;
			else if (_rotation.x <= MIN_ROTATION_X)
				_rotation.x = MIN_ROTATION_X;*/
			
			return _rotation;
		}
		
		override public function get position() : Vector4
		{
			var pos : Vector4 = super.position;
			
			if (_invalidPosition || _rotationVersion != _rotation.version || _lookAtVersion != lookAt.version)
			{
				_invalidPosition = false;
				_rotationVersion = _rotation.version;
				_lookAtVersion = lookAt.version;
				
				pos.x = lookAt.x - _distance * Math.cos(rotation.y) * Math.cos(rotation.x);
				pos.y = lookAt.y - _distance * Math.sin(rotation.x);
				pos.z = lookAt.z - _distance * Math.sin(rotation.y) * Math.cos(rotation.x);			
			}
			
			return pos;
		}
	}
}