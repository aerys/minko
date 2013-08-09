package aerys.minko.scene.node.camera
{
	import flash.geom.Point;
	
	import aerys.minko.ns.minko_camera;
	import aerys.minko.scene.controller.camera.CameraController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;
	
	
	public class AbstractCamera extends AbstractSceneNode
	{
		use namespace minko_camera;
		
		public static const DEFAULT_ZNEAR	: Number	= .1;
		public static const DEFAULT_ZFAR	: Number	= 500.;
		
		minko_camera var _cameraCtrl	: CameraController;
		minko_camera var _cameraData	: CameraDataProvider;
		
		protected var _enabled		: Boolean;
		
		protected var _activated	: Signal;
		protected var _deactivated	: Signal;
		
		public function get zNear() : Number
		{
			return _cameraData.zNear;
		}
		public function set zNear(value : Number) : void
		{
			_cameraData.zNear = value;
		}
		
		public function get zFar() : Number
		{
			return _cameraData.zFar;
		}
		public function set zFar(value : Number) : void
		{
			_cameraData.zFar = value;
		}
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			if (value != _enabled)
			{
				_enabled = value;
				
				if (_enabled)
					_activated.execute(this);
				else
					_deactivated.execute(this);
			}
		}
		
		public function get activated() : Signal
		{
			return _activated;
		}
		public function get deactivated() : Signal
		{
			return _deactivated;
		}
		
		public function get frustum() : Frustum
		{
			return _cameraData.frustum;
		}
		
		protected function get cameraController() : CameraController
		{
			return _cameraCtrl;
		}
		
		protected function get cameraData() : CameraDataProvider
		{
			return _cameraData;
		}
		
		public function AbstractCamera(zNear	: Number = DEFAULT_ZNEAR,
									   zFar		: Number = DEFAULT_ZFAR)
		{
			super();
			
			initializeCameraData(zNear, zFar);
		}
		
		override protected function initialize() : void
		{
			_enabled = true;
			
			super.initialize();
		}

		override protected function initializeSignals():void
		{
			super.initializeSignals();
			
			_activated = new Signal('Camera.activated');
			_deactivated = new Signal('Camera.deactivated');	
		}
		
		override protected function initializeContollers():void
		{
			super.initializeContollers();
			
			_cameraCtrl = new CameraController();
			addController(_cameraCtrl);
		}
		
		override protected function initializeDataProviders():void
		{
			super.initializeDataProviders();
			
			_cameraData = _cameraCtrl.cameraData;
		}
		
		protected function initializeCameraData(zNear : Number, zFar : Number) : void
		{
			_cameraData.zNear = zNear;
			_cameraData.zFar = zFar;
		}
		
		public function unproject(x : Number, y : Number, out : Ray = null) : Ray
		{
			throw new Error('Must be overriden.');
		}
		
		public function project(worldPosition : Vector4, output : Point = null) : Point
		{
			throw new Error('Must be overriden.');
		}
		
		/**
		 * Return a copy of the world to view matrix. This method is an alias on the
		 * getWorldToLocalTransform() method.
		 *  
		 * @param output
		 * @return 
		 * 
		 */
		public function getWorldToViewTransform(forceUpdate	: Boolean	= false,
												output 		: Matrix4x4 = null) : Matrix4x4
		{
			return getWorldToLocalTransform(forceUpdate, output);
		}
		
		/**
		 * Return a copy of the view to world matrix. This method is an alias on the
		 * getLocalToWorldTransform() method.
		 *  
		 * @param output
		 * @return 
		 * 
		 */
		public function getViewToWorldTransform(forceUpdate : Boolean	= false,
												output 		: Matrix4x4 = null) : Matrix4x4
		{
			return getLocalToWorldTransform(forceUpdate, output);
		}
		
		/**
		 * Return a copy of the projection matrix.
		 *  
		 * @param output
		 * @return 
		 * 
		 */
		public function getProjection(output : Matrix4x4) : Matrix4x4
		{
			output ||= new Matrix4x4();
			
			return output.copyFrom(_cameraData.projection);
		}
	}
}