package aerys.minko.scene.node
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;

	public class Camera extends AbstractSceneNode
	{
		public static const DEFAULT_FOV			: Number		= Math.PI * .25;
		public static const DEFAULT_ZNEAR		: Number		= .1;
		public static const DEFAULT_ZFAR		: Number		= 500.;
		
		private var _fieldOfView				: Number		= 0;
		private var _zNear						: Number		= 0;
		private var _zFar						: Number		= 0;
		private var _worldToScreen				: Matrix4x4		= new Matrix4x4();
		private var _screenToWorld				: Matrix4x4		= new Matrix4x4();
		private var _projection					: Matrix4x4		= new Matrix4x4();
		private var _screenToView				: Matrix4x4		= new Matrix4x4();
		private var _cameraPosition				: Vector4		= new Vector4();
		private var _cameraWorldUp				: Vector4		= new Vector4();
		private var _cameraWorldLookAt			: Vector4		= new Vector4();
		private var _cameraWorldPosition		: Vector4		= new Vector4();
		private var _cameraLookAt				: Vector4		= new Vector4();
		private var _cameraUp					: Vector4		= new Vector4();
		
		private var _dataProvider : DataProvider = new DataProvider().setProperties({
			"cameraPosition"		: _cameraPosition,
			"cameraLookAt"			: _cameraLookAt,
			"cameraUp"				: _cameraUp,
			"cameraWorldPosition"	: _cameraWorldPosition,
			"cameraWorldLookAt"		: _cameraWorldLookAt,
			"cameraWorldUp"			: _cameraWorldUp,
			"cameraFov"				: 0,
			"cameraZNear"			: 0,
			"cameraZFar"			: 0,
			"worldToView"			: worldToLocal,
			"projection"			: _projection,
			"worldToScreen"			: _worldToScreen,
			"viewToWorld"			: localToWorld,
			"screenToView"			: _screenToView,
			"screenToWorld"			: _screenToWorld
		});
		
		public function get fieldOfView()	: Number { return _fieldOfView;	}
		public function get zNear()			: Number { return _zNear;		}
		public function get zFar()			: Number { return _zFar;		}
		
		public function set fieldOfView(v : Number) : void
		{
			_fieldOfView = v;
			updateProjection();
		}
		
		public function set zNear(v : Number) : void
		{
			_zNear = v;
			updateProjection();
		}
		
		public function set zFar(v : Number) : void
		{
			_zFar = v;
			updateProjection();
		}
		
		public function Camera(fieldOfView	: Number = DEFAULT_FOV,
							   zNear		: Number = DEFAULT_ZNEAR,
							   zFar			: Number = DEFAULT_ZFAR)
		{
			super();
			
			_fieldOfView	= fieldOfView;
			_zNear			= zNear;
			_zFar			= zFar;
			
			localToWorld.changed.add(localToWorldChangedHandler);
		}
		
		public function unproject(x : Number, y : Number, out : Ray = null) : Ray
		{
			out ||= new Ray();
			
			if (!(root is Scene))
				throw new Error('Camera must be addchilded to unproject vectors');
			
			var sceneBindings	: DataBindings	= Scene(root).bindings;
			var width			: Number		= sceneBindings.getProperty('viewportWidth');
			var height			: Number		= sceneBindings.getProperty('viewportHeight');
			var xPercent		: Number		= 2.0 * (x / width - 0.5);
			var yPercent 		: Number		= 2.0 * (y / height - 0.5);
			var fovDiv2			: Number		= _fieldOfView * 0.5;
			var dx				: Number 		= Math.tan(fovDiv2) * xPercent * (width / height);
			var dy				: Number 		= -Math.tan(fovDiv2) * yPercent;
			
			out.origin.set(dx * _zNear, dy * _zNear, _zNear);
			out.direction.set(dx * _zNear, dy * _zNear, _zNear).normalize();
			
			localToWorld.transformVector(out.origin, out.origin);
			localToWorld.deltaTransformVector(out.direction, out.direction);
			
			return out;
		}
		
		override protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.addedToSceneHandler(child, scene);
			
			var sceneBindings : DataBindings = scene.bindings;
			
			sceneBindings.addProvider(_dataProvider);
			sceneBindings.addCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.addCallback('viewportHeight', viewportSizeChanged);
			
			updateProjection();
			transformChangedHandler(transform, null);
			localToWorldChangedHandler(localToWorld, null);
		}
		
		override protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.removedFromSceneHandler(child, scene);

			var sceneBindings : DataBindings = scene.bindings;
			
			sceneBindings.removeProvider(_dataProvider);
			sceneBindings.removeCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.removeCallback('viewportHeight', viewportSizeChanged);
		}
		
		override protected function transformChangedHandler(transform : Matrix4x4, propertyName : String) : void
		{
			super.transformChangedHandler(transform, propertyName);
			
			transform.transformVector(Vector4.ZERO,		_cameraPosition);
			transform.transformVector(Vector4.Z_AXIS,	_cameraLookAt);
			transform.transformVector(Vector4.Y_AXIS,	_cameraUp);
		}
		
		private function localToWorldChangedHandler(localToWorld : Matrix4x4, propertyName : String) : void
		{
			localToWorld.transformVector(Vector4.ZERO,		_cameraWorldPosition);
			localToWorld.transformVector(Vector4.Z_AXIS,	_cameraWorldLookAt);
			localToWorld.transformVector(Vector4.Y_AXIS,	_cameraWorldUp);
			
			_screenToWorld.lock().copyFrom(_screenToView).append(localToWorld).unlock();
			_worldToScreen.lock().copyFrom(worldToLocal).append(_projection).unlock();
		}
		
		private function viewportSizeChanged(bindings : DataBindings, key : String, newValue : Object) : void
		{
			updateProjection();
		}
		
		private function updateProjection() : void
		{
			if (root is Scene)
			{
				var sceneBindings	: DataBindings	= Scene(root).bindings;
				var ratio			: Number		= sceneBindings.getProperty('viewportWidth') / sceneBindings.getProperty('viewportHeight');
				
				_projection.perspectiveFoV(_fieldOfView, ratio, _zNear, _zFar);
				_screenToView.lock().copyFrom(_projection).invert().unlock();
				
				_screenToWorld.lock().copyFrom(_screenToView).append(localToWorld).unlock();
				_worldToScreen.lock().copyFrom(worldToLocal).append(_projection).unlock();
			}
		}
	}
}