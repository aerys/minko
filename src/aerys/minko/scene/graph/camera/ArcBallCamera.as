package aerys.minko.scene.graph.camera
{
	import aerys.common.Factory;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix3D;
	
	/**
	 * ArcBallCamera3D objects represents a "third-person" camera that can move around a
	 * point in space.
	 * 
	 * The position of the camera will be computed according to the look-at point and the
	 * rotation angles given by the "rotation" property.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ArcBallCamera extends Camera
	{
		private static const EPSILON		: Number	= .001;
		private static const MAX_ROTATION_X	: Number	= Math.PI / 2. - EPSILON;
		private static const MIN_ROTATION_X	: Number	= -MAX_ROTATION_X;
		
		private var _distance	: Number	= 1.;
		private var _rotation	: Vector4	= new Vector4();
		
		private var _rv			: uint		= 0;
		
		override public function get version():uint
		{
			return super.version + _rotation.version;
		}
		
		public function ArcBallCamera()
		{
			super();
			
			lookAt.set(0., 0., 0.);
			rotation.y = Math.PI * .5;
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
			if (value != _distance)
			{
				_distance = value;
				invalidateView();
			}
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
			return _rotation;
		}

		override protected function updateMatrices(query : RenderingVisitor = null) : void
		{
			if (_rotation.x >= MAX_ROTATION_X)
				_rotation.x = MAX_ROTATION_X;
			else if (_rotation.x <= MIN_ROTATION_X)
				_rotation.x = MIN_ROTATION_X;
			
			if (_distance <= 0.)
				_distance = EPSILON;

			position.x = lookAt.x - _distance * Math.cos(_rotation.y) * Math.cos(_rotation.x);
			position.y = lookAt.y - _distance * Math.sin(_rotation.x);
			position.z = lookAt.z - _distance * Math.sin(_rotation.y) * Math.cos(_rotation.x);
			
			super.updateMatrices(query);
		}
		
		override public function visited(visitor : ISceneVisitor) : void
		{
			if (_rotation.version != _rv)
			{
				invalidateView();
				_rv = _rotation.version;
			}
			
			super.visited(visitor);
		}
	}
}