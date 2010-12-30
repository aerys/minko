package aerys.minko.scene.camera
{
	import aerys.common.Factory;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.render.visitor.IScene3DVisitor;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
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
		//{ region vars
		private var _version		: uint		= 0;
		
		private var _enabled		: Boolean	= true;
		
		private var _update			: Boolean	= true;

		private var _position		: Vector4	= new Vector4();
		private var _lookAt			: Vector4	= Vector4.ZERO.clone();
		private var _up				: Vector4	= Vector4.Y_AXIS.clone();
		private var _transform		: Matrix4x4	= new Matrix4x4();
		
		private var _pv				: uint		= 0;
		private var _lv				: uint		= 0;
		private var _uv				: uint		= 0;
		//} endregion
		
		//{ region getters/setters
		public function get position() 	: Vector4	{ return _position; }
		public function get lookAt()	: Vector4	{ return _lookAt; }
		public function get up()		: Vector4	{ return _up; }
		
		/**
		 * Specify whether the camera is active or not. If the camera is
		 * not active, it will not push its 3D transformation into the
		 * stack and will not affect the rendering.
		 * 
		 * @return <code>true</code> is the camera is enabled,
		 * <code>false</code> otherwise
		 */
		public function get enabled() 	: Boolean 	{ return _enabled; }
		/**
		 * The <code>x</code> component of the look-at vector.
		 */
		
		public function set enabled(value : Boolean) : void
		{
			if (value != _enabled)
			{
				++_version;
				_enabled = value;
			}
		}
		//} endregion

		//{ region methods
		protected function invalidate() : uint
		{
			_update = true;

			return ++_version;
		}
		
		protected function invalidateTransform(visitor : IScene3DVisitor = null) : void
		{
			var world : Matrix4x4 	= visitor.renderer.transform.world;
			
			Matrix4x4.lookAtLeftHanded(world.multiplyVector(_position),
									   world.multiplyVector(_lookAt),
									   world.deltaMultiplyVector(_up),
									   _transform);
		}
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			if (!_enabled)
				return ;

			/*if (_update || _position.version != _pv || _lookAt.version != _lv
				|| _up.version != _uv)
			{*/
				invalidateTransform(visitor);
				
				/*_update = false;
				_pv = _position.version;
				_lv = _lookAt.version;
				_uv = _up.version;*/
			//}
			
			var t : TransformManager = visitor.renderer.transform;
			
			t.projection = visitor.renderer.viewport.projection;
			t.view = _transform;
		}
		//} endregion
	}
}