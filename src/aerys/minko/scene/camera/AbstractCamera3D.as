package aerys.minko.scene.camera
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.type.math.Transform3D;
	
	import flash.geom.Vector3D;
	
	/**
	 * The AbstractCamera3D is the the base camera class. It should
	 * <strong>not</strong> be used directly. One should use the
	 * <code>ArcBallCamera3D</code> or the <code>FirstPersonCamera3D</code>
	 * classes or extend this one.
	 * 
	 * <p>
	 * When extending this class to create a new camera class, one should
	 * override the <code>invalidateTransform</code> method.
	 * </p>
	 * 
	 * @see ArcBallCamera3D
	 * @see FirstPersonCamera3D
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractCamera3D extends AbstractScene3D implements ICamera3D
	{
		use namespace minko;
		
		//{ region vars
		private var _enabled		: Boolean		= true;
		
		protected var _update		: Boolean		= true;
		
		private var _lookAt			: Vector3D		= Vector3D.Y_AXIS.clone();
		private var _up				: Vector3D		= Vector3D.Y_AXIS.clone();
		private var _transform		: Transform3D	= new Transform3D();
		
		protected var _position		: Vector3D		= new Vector3D();
		//} endregion
		
		//{ region getters/setters
		/**
		 * Specify whether the camera is active or not. If the camera is
		 * not active, it will not push its 3D transformation into the
		 * stack and will not affect the rendering.
		 * 
		 * @return <code>true</code> is the camera is enabled,
		 * <code>false</code> otherwise
		 */
		public function get enabled() 	: Boolean 	{ return _enabled; }
		
		public function get x()			: Number	{ return _position.x; }
		public function get y()			: Number	{ return _position.y; }
		public function get z()			: Number	{ return _position.z; }
		
		/**
		 * The <code>x</code> component of the look-at vector.
		 */
		public function get lookAtX() 	: Number 	{ return _lookAt.x; }
		/**
		 * The <code>y</code> component of the look-at vector.
		 */
		public function get lookAtY() 	: Number 	{ return _lookAt.y; }
		/**
		 * The <code>z</code> component of the look-at vector.
		 */
		public function get lookAtZ() 	: Number 	{ return _lookAt.z; }
		/**
		 * The <code>x</code> component of the up vector.
		 */
		public function get upX() 		: Number 	{ return _up.x; }
		/**
		 * The <code>y</code> component of the up vector.
		 */
		public function get upY() 		: Number 	{ return _up.y; }
		/**
		 * The <code>z</code> component of the up vector.
		 */
		public function get upZ() 		: Number 	{ return _up.z; }
		
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}
				
		public function set lookAtX(value : Number) : void
		{
			if (value != _lookAt.x)
			{
				_update = true;
				_lookAt.x = value;
			}
		}
		
		public function set lookAtY(value : Number) : void
		{
			if (value != _lookAt.y)
			{
				_update = true;
				_lookAt.y = value;
			}
		}
		
		public function set lookAtZ(value : Number) : void
		{
			if (value != _lookAt.z)
			{
				_update = true;
				_lookAt.z = value;
			}
		}
		
		public function set upX(value : Number) : void
		{
			if (value != _up.x)
			{
				_update = true;
				_up.x = value;
			}
		}
		
		public function set upY(value : Number) : void
		{
			if (value != _up.y)
			{
				_update = true;
				_up.y = value;
			}
		}
		
		public function set upZ(value : Number) : void
		{
			if (value != _up.z)
			{
				_update = true;
				_up.z = value;
			}
		}
		//} endregion

		//{ region methods
		public function getPosition() : Vector3D
		{
			return _position.clone();
		}
		
		public function getLookAt() : Vector3D
		{
			return _lookAt.clone();
		}
		
		public function setLookAt(value : Vector3D) : void
		{
			if (value.x != _lookAt.x || value.y != _lookAt.y || value.z != _lookAt.z)
			{
				_update = true;
				_lookAt.x = value.x;
				_lookAt.y = value.y;
				_lookAt.z = value.z;
			}
		}
		
		public function setUp(value : Vector3D) : void
		{
			if (value.x != _up.x || value.y != _up.y || value.z != _up.z)
			{
				_update = true;
				_up.x = value.x;
				_up.y = value.y;
				_up.z = value.z;
			}
		}
		
		public function getUp() : Vector3D
		{
			return _up.clone();
		}
		
		protected function invalidateTransform(visitor : IScene3DVisitor = null) : void
		{
			var t : Transform3D = visitor.renderer.transform.world;
			
			_transform = Transform3D.lookAtLeftHanded(t.transformVector(_position),
													  t.transformVector(_lookAt),
													  t.transformVector(_up));
		}
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			if (!_enabled)
				return ;
			
			invalidateTransform(visitor);
			visitor.renderer.transform.view = _transform;
		}
		
		public function getLocalPosition(worldTransform : Transform3D) : Vector3D
		{
			var world : Transform3D = worldTransform.temporaryClone();
			
			world.invert();
			
			return world.transformVector(_position);
		}
		//} endregion
	}
}