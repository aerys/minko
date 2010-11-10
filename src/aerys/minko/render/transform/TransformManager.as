package aerys.minko.render.transform
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.type.AbstractStatesManager;
	import aerys.minko.type.math.Transform3D;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public final class TransformManager extends AbstractStatesManager
	{
		use namespace minko;
		
		private static const TRANSFORM3D			: Factory	= Factory.getFactory(Transform3D);
		private static const VECTOR3D				: Factory	= Factory.getFactory(Vector3D);
		private static const ZERO3					: Vector3D	= new Vector3D();
		
		private static const UPDATE_NONE			: uint		= 0;
		private static const UPDATE_WORLD_TO_LOCAL	: uint		= 1;
		private static const UPDATE_CAMERA_POSITION	: uint		= 2;
		private static const UPDATE_LOCAL_TO_VIEW	: uint		= 4;
		private static const UPDATE_LOCAL_TO_SCREEN	: uint		= 8;
		private static const UPDATE_ALL				: uint		= UPDATE_WORLD_TO_LOCAL
																  | UPDATE_CAMERA_POSITION
																  | UPDATE_LOCAL_TO_VIEW
																  | UPDATE_LOCAL_TO_SCREEN;
		
		private var _update				: uint					= UPDATE_NONE;
		private var _size				: int					= 0;
		
		private var _cameraPos			: Vector3D				= new Vector3D();
		private var _localCameraPos		: Vector3D				= new Vector3D();
		
		private var _view				: Transform3D			= new Transform3D();
		private var _world				: Transform3D			= new Transform3D();
		private var _projection			: Transform3D			= new Transform3D();
		
		private var _worldStack			: Vector.<Transform3D>	= new Vector.<Transform3D>();
		private var _viewStack			: Vector.<Transform3D>	= new Vector.<Transform3D>();
		private var _projectionStack	: Vector.<Transform3D>	= new Vector.<Transform3D>();
		
		private var _pushStack			: Vector.<uint>		= new Vector.<uint>();
		
		//{ region getters/setters
		public function get world() : Transform3D
		{
			return _world;
		}

		public function get view() : Transform3D
		{
			return _view;
		}
		
		public function get projection() : Transform3D
		{
			return _projection;
		}

		public function set view(value : Transform3D) : void
		{
			if (!(lockedStates & TransformType.VIEW))
			{
				_view.setRawData(value.getRawData());
				_update |= UPDATE_CAMERA_POSITION
						   | UPDATE_LOCAL_TO_VIEW
						   | UPDATE_LOCAL_TO_SCREEN;
			}
		}
		
		public function set projection(value : Transform3D) : void
		{
			if (!(lockedStates & TransformType.PROJECTION))
			{
				_update |= UPDATE_LOCAL_TO_SCREEN;
				_projection.setRawData(value.getRawData());
			}
		}
		
		public function set world(value : Transform3D) : void
		{
			if (!(lockedStates & TransformType.WORLD))
			{
				_world.setRawData(value.getRawData());
				_update = UPDATE_ALL;
			}
		}
		
		public function getLocalToScreenMatrix() : Matrix3D
		{
			var mat : Transform3D = _world.clone(true);
			
			mat.append(_view);
			mat.append(_projection);
			
			return mat._matrix;
		}
		//} endregion
		
		public function TransformManager()
		{
			super();
			
			register(TransformType.WORLD, "world", new Vector.<Transform3D>());
			register(TransformType.VIEW, "view", new Vector.<Transform3D>());
			register(TransformType.PROJECTION, "projection", new Vector.<Transform3D>());
		}
		
		override public function pushState(mask : uint, value : Object, stack : Object) : void
		{
			super.pushState(mask, (value as Transform3D).clone(true), stack);
		}
		
		//{ region methods
		public function reset() : void
		{
			_world = TRANSFORM3D.create(true);
			_world.identity();
			
			_view = TRANSFORM3D.create(true);
			_view.identity();
			
			_projection = TRANSFORM3D.create(true);
			_projection.identity();
		}
	}
}