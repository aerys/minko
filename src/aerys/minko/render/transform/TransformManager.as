package aerys.minko.render.transform
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.type.AbstractStatesManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public final class TransformManager extends AbstractStatesManager
	{
		use namespace minko;
		
		private static const MATRIX4X4				: Factory	= Factory.getFactory(Matrix4x4);
		private static const VECTOR4				: Factory	= Factory.getFactory(Vector4);
		
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
		
		private var _cameraPos			: Vector4				= new Vector4();
		private var _localCameraPos		: Vector4				= new Vector4();
		
		private var _view				: Matrix4x4				= new Matrix4x4();
		private var _world				: Matrix4x4				= new Matrix4x4();
		private var _projection			: Matrix4x4				= new Matrix4x4();
		
		private var _worldStack			: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>();
		private var _viewStack			: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>();
		private var _projectionStack	: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>();
		
		private var _pushStack			: Vector.<uint>		= new Vector.<uint>();
		
		//{ region getters/setters
		public function get world() : Matrix4x4
		{
			return _world;
		}

		public function get view() : Matrix4x4
		{
			return _view;
		}
		
		public function get projection() : Matrix4x4
		{
			return _projection;
		}

		public function set view(value : Matrix4x4) : void
		{
			if (!(lockedStates & TransformType.VIEW))
			{
				_view = Matrix4x4.clone(value, _view);
				_update |= UPDATE_CAMERA_POSITION
						   | UPDATE_LOCAL_TO_VIEW
						   | UPDATE_LOCAL_TO_SCREEN;
			}
		}
		
		public function set projection(value : Matrix4x4) : void
		{
			if (!(lockedStates & TransformType.PROJECTION))
			{
				_projection = Matrix4x4.clone(value, _projection);
				_update |= UPDATE_LOCAL_TO_SCREEN;
			}
		}
		
		public function set world(value : Matrix4x4) : void
		{
			if (!(lockedStates & TransformType.WORLD))
			{
				_world = Matrix4x4.clone(value, _world);
				_update = UPDATE_ALL;
			}
		}
		
		public function getLocalToScreenMatrix() : Matrix4x4
		{
			return Matrix4x4.multiplyInverse(_world, _view, MATRIX4X4.create(true))
							.multiplyInverse(_projection);
		}
		
		public function getLocalToViewMatrix3D() : Matrix3D
		{
			return Matrix4x4.multiplyInverse(_world, _view, MATRIX4X4.create(true))
							._matrix;
		}
		//} endregion
		
		public function TransformManager()
		{
			super();
			
			registerState(TransformType.WORLD, "world", new Vector.<Matrix4x4>());
			registerState(TransformType.VIEW, "view", new Vector.<Matrix4x4>());
			registerState(TransformType.PROJECTION, "projection", new Vector.<Matrix4x4>());
		}
		
		override protected function pushState(mask 	: uint,
											  value : Object,
											  stack : Object) : void
		{
			super.pushState(mask,
							Matrix4x4.clone(value as Matrix4x4, MATRIX4X4.create(true)),
							stack);
		}
		
		override protected function popState(mask		: uint,
											 property	: String,
											 stack		: Object) : void
		{
			var value : Vector.<Number> = stack[int(stack.length - 1)].getRawData();
			
			stack.length--;
			if (property == "world")
				world.setRawData(value);
			else if (property == "view")
				view.setRawData(value);
			else if (property == "projection")
				projection.setRawData(value);
		}
		
		//{ region methods
		public function reset() : void
		{
			_world.identity();
			_view.identity();
			_projection.identity();
		}
	}
}