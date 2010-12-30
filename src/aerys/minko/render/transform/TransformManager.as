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
		
		private var _view				: Matrix4x4				= new Matrix4x4();
		private var _world				: Matrix4x4				= new Matrix4x4();
		private var _projection			: Matrix4x4				= new Matrix4x4();
		
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
				Matrix4x4.copy(value, _view);
				_update |= UPDATE_CAMERA_POSITION
						   | UPDATE_LOCAL_TO_VIEW
						   | UPDATE_LOCAL_TO_SCREEN;
			}
		}
		
		public function set projection(value : Matrix4x4) : void
		{
			if (!(lockedStates & TransformType.PROJECTION))
			{
				Matrix4x4.copy(value, _projection);
				_update |= UPDATE_LOCAL_TO_SCREEN;
			}
		}
		
		public function set world(value : Matrix4x4) : void
		{
			if (!(lockedStates & TransformType.WORLD))
			{
				Matrix4x4.copy(value, _world);
				_update = UPDATE_ALL;
			}
		}
		
		public function getLocalToScreenMatrix() : Matrix4x4
		{
			return Matrix4x4.multiplyInverse(_world, _view, MATRIX4X4.create(true))
							.multiplyInverse(_projection);
		}
		//} endregion
		
		public function TransformManager()
		{
			super();
			
			registerState(TransformType.WORLD,
						  "world",
						  new Vector.<Matrix4x4>());
			
			registerState(TransformType.VIEW,
						  "view",
						  new Vector.<Matrix4x4>());
			
			registerState(TransformType.PROJECTION,
						  "projection",
						  new Vector.<Matrix4x4>());
		}
		
		override protected function pushState(mask 	: uint,
											  value : Object,
											  stack : Object) : void
		{
			super.pushState(mask,
							Matrix4x4.copy(value as Matrix4x4, MATRIX4X4.create(true)),
							stack);
		}
		
		override protected function popState(mask		: uint,
											 property	: String,
											 stack		: Object) : void
		{
			var value : Matrix4x4 = stack[int(stack.length - 1)];
			
			stack.length--;
			
			if (property == "world")
				Matrix4x4.copy(value, world);
			else if (property == "view")
				Matrix4x4.copy(value, view);
			else if (property == "projection")
				Matrix4x4.copy(value, projection);
		}
		
		//{ region methods
		public function reset() : void
		{
			_world.identity();
			_view.identity();
			_projection.identity();
			
			_update = UPDATE_ALL;
		}
	}
}