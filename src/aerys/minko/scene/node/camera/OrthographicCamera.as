package aerys.minko.scene.node.camera
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.math.Ray;
	
	use namespace minko_scene;
	
	public class OrthographicCamera extends AbstractCamera
	{
		public static const ZOOM_DEFAULT : Number = 1;
		
		public function get zoom() : Number
		{
			return cameraData.zoom;
		}

		public function set zoom(value : Number) : void
		{
			cameraData.zoom = value;
		}
		
		public function OrthographicCamera(zoom 	: Number = ZOOM_DEFAULT,
										   zNear 	: Number = AbstractCamera.DEFAULT_ZNEAR,
										   zFar 	: Number = AbstractCamera.DEFAULT_ZFAR)
		{
			super(zNear, zFar);
			
			_cameraData.zoom = zoom;
		}
		
		override protected function initializeContollers() : void
		{
			super.initializeContollers();
			
			cameraController.orthographic = true;
		}

		override public function unproject(x : Number, y : Number, out : Ray = null) : Ray
		{
			if (!(root is Scene))
				throw new Error('Camera must be in the scene to unproject vectors.');
			
			out ||= new Ray();
			
			var sceneBindings	: DataBindings	= (root as Scene).bindings;
			var zNear			: Number		= _cameraData.zNear;
			var zFar			: Number		= _cameraData.zFar;
			var width			: Number		= sceneBindings.getProperty('viewportWidth');
			var height			: Number		= sceneBindings.getProperty('viewportHeight');
			var xPercent		: Number		= (x / width - 0.5);
			var yPercent 		: Number		= -(y / height - 0.5);
			
			out.origin.set(
				xPercent * width / _cameraData.zoom, yPercent * height / _cameraData.zoom, zNear
			);
			out.direction.set(0, 0, 1);
			
			localToWorld(out.origin, out.origin);
			localToWorld(out.direction, out.direction, true);
			
			return out;
		}
		
		override minko_scene function cloneNode():AbstractSceneNode
		{
			var clone : OrthographicCamera = new OrthographicCamera(zoom, zNear, zFar);
			
			clone.transform.copyFrom(transform);
			
			return clone as AbstractSceneNode;
		}
	}
}