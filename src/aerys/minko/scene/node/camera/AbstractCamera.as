package aerys.minko.scene.node.camera
{
	import aerys.minko.scene.controller.camera.CameraController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	
	public class AbstractCamera extends AbstractSceneNode
	{
		public static const DEFAULT_ZNEAR	: Number	= .1;
		public static const DEFAULT_ZFAR	: Number	= 500.;
		
		protected var _cameraData	: CameraDataProvider	= null;
		
		protected var _enabled		: Boolean				= true;
		
		protected var _activated	: Signal				= new Signal('Camera.activated');
		protected var _deactivated	: Signal				= new Signal('Camera.deactivated');
		
		public function get cameraData() : CameraDataProvider
		{
			return _cameraData;
		}
		
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
		
		public function get worldToView() : Matrix4x4
		{
			return worldToLocal;
		}
		
		public function get viewToWorld() : Matrix4x4
		{
			return localToWorld;
		}
		
		public function get worldToScreen() : Matrix4x4
		{
			return _cameraData.worldToScreen;
		}
		
		public function AbstractCamera(zNear	: Number = DEFAULT_ZNEAR,
									   zFar		: Number = DEFAULT_ZFAR)
		{
			super();
			
			_cameraData = new CameraDataProvider(worldToView, viewToWorld);
			
			_cameraData.zNear 	= zNear;
			_cameraData.zFar 	= zFar;
			
			initialize();
		}
		
		protected function initialize() : void
		{
			throw new Error('Must be overriden.');
		}
		
		public function unproject(x : Number, v : Number, out : Ray = null) : Ray
		{
			throw new Error('Must be overriden.');
		}
		
		public function getWorldToScreen(output : Matrix4x4) : Matrix4x4
		{
			return output != null
				? output.copyFrom(_cameraData.worldToScreen)
				: _cameraData.worldToScreen.clone();
		}
		
		public function getScreenToWorld(output : Matrix4x4) : Matrix4x4
		{
			return output != null
				? output.copyFrom(_cameraData.screenToWorld)
				: _cameraData.screenToWorld.clone();
		}
		
		public function getProjection(output : Matrix4x4) : Matrix4x4
		{
			return output != null
				? output.copyFrom(_cameraData.projection)
				: _cameraData.projection.clone();
		}
	}
}