package aerys.minko.scene.node.camera
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.camera.CameraController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Point;
	
	use namespace minko_scene;

	public class Camera extends AbstractCamera
	{
		public static const DEFAULT_FOV		: Number	= Math.PI * .25;
        
        private static const TMP_VECTOR4    : Vector4   = new Vector4();
		
		public function get fieldOfView() : Number
		{
			return _cameraData.fieldOfView;
		}
		public function set fieldOfView(value : Number) : void
		{
			_cameraData.fieldOfView = value;
		}
		
		public function Camera(fieldOfView	: Number = DEFAULT_FOV,
							   zNear		: Number = AbstractCamera.DEFAULT_ZNEAR,
							   zFar			: Number = AbstractCamera.DEFAULT_ZFAR)
		{
			super(zNear, zFar);
			
			_cameraData.fieldOfView = fieldOfView;			
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var clone : Camera = new Camera(fieldOfView, zNear, zFar);
			
			clone.transform.copyFrom(this.transform);
			
			return clone as AbstractSceneNode;
		}
		
		override public function unproject(x : Number, y : Number, out : Ray = null) : Ray
		{
			if (!(root is Scene))
				throw new Error('Camera must be in the scene to unproject vectors.');
			
			out ||= new Ray();
			
			var sceneBindings	: DataBindings	= (root as Scene).bindings;
			var zNear			: Number		= _cameraData.zNear;
			var zFar			: Number		= _cameraData.zFar;
			var fovDiv2			: Number		= _cameraData.fieldOfView * 0.5;
			var width			: Number		= sceneBindings.getProperty('viewportWidth');
			var height			: Number		= sceneBindings.getProperty('viewportHeight');
			var xPercent		: Number		= 2.0 * (x / width - 0.5);
			var yPercent 		: Number		= 2.0 * (y / height - 0.5);
			var dx				: Number 		= Math.tan(fovDiv2) * xPercent * (width / height);
			var dy				: Number 		= -Math.tan(fovDiv2) * yPercent;
			
			out.origin.set(dx * zNear, dy * zNear, zNear);
			out.direction.set(dx * zNear, dy * zNear, zNear).normalize();
			
			localToWorld(out.origin, out.origin);
			localToWorld(out.direction, out.direction, true);
			
			return out;
		}
		
		override public function project(localToWorld : Matrix4x4, output : Point = null) : Point
		{
			output ||= new Point();
			
			var sceneBindings	: DataBindings	= (root as Scene).bindings;
			var width			: Number		= sceneBindings.getProperty('viewportWidth');
			var height			: Number		= sceneBindings.getProperty('viewportHeight');
			var translation		: Vector4		= localToWorld.getTranslation();
			var screenPosition	: Vector4		= _cameraData.worldToScreen.projectVector(
                translation, TMP_VECTOR4
            );
            
			output.x = width * ((screenPosition.x + 1.0) * .5);
			output.y = height * ((1.0 - ((screenPosition.y + 1.0) * .5)));
			
			return output;
		}
	}
}