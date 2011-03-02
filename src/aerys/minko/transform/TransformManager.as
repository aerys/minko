package aerys.minko.transform
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public final class TransformManager
	{
		use namespace minko;
		
		private static const UPDATE_FLAGS_STATE		: uint		= 8;
		
		private static const MATRIX4X4				: Factory	= Factory.getFactory(Matrix4x4);
		private static const VECTOR4				: Factory	= Factory.getFactory(Vector4);
		
		private static const UPDATE_NONE			: uint		= 0;
		private static const UPDATE_WORLD_TO_LOCAL	: uint		= 1;
		private static const UPDATE_CAMERA_POSITION	: uint		= 2;
		private static const UPDATE_LOCAL_TO_VIEW	: uint		= 4;
		private static const UPDATE_LOCAL_TO_SCREEN	: uint		= 8;
		private static const UPDATE_VIEW_TO_LOCAL	: uint		= 16;
		private static const UPDATE_ALL				: uint		= UPDATE_WORLD_TO_LOCAL
																  | UPDATE_CAMERA_POSITION
																  | UPDATE_LOCAL_TO_VIEW
																  | UPDATE_LOCAL_TO_SCREEN
																  | UPDATE_VIEW_TO_LOCAL;
		
		private var _updateFlags		: uint					= UPDATE_NONE;
		private var _updateFlagsStack	: Vector.<uint>			= new Vector.<uint>();
		private var _worldVersion		: uint					= 0;
		private var _viewVersion		: uint					= 0;
		
		private var _view				: Matrix4x4				= new Matrix4x4();
		private var _world				: Matrix4x4				= new Matrix4x4();
		private var _projection			: Matrix4x4				= new Matrix4x4();
		
		protected function get updateFlags() : uint
		{
			return _updateFlags;
		}
		
		protected function set updateFlags(value : uint) : void
		{
			_updateFlags = value;
		}
		
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
			Matrix4x4.copy(value, _view);
				
			_updateFlags |= UPDATE_CAMERA_POSITION
					  	 	| UPDATE_LOCAL_TO_VIEW
					   		| UPDATE_LOCAL_TO_SCREEN;
		}
		
		public function set projection(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _projection);
			
			_updateFlags |= UPDATE_LOCAL_TO_SCREEN;
		}
		
		public function set world(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _world);
			
			_updateFlags = UPDATE_ALL;
		}
		
		public function TransformManager()
		{
			super();
		}
		
		public function getLocalToScreen(out : Matrix4x4 = null) : Matrix4x4
		{
			out ||= MATRIX4X4.create();
			out = Matrix4x4.copy(_world, out)
					 	   .multiplyInverse(_view)
					 	   .multiplyInverse(_projection);
			
			return out;
		}
		
		public function getLocalToView(out : Matrix4x4 = null) : Matrix4x4
		{
			out ||= MATRIX4X4.create();
			out = Matrix4x4.copy(_world, out)
						   .multiplyInverse(_view);
			
			return out;
		}
		
		public function reset() : void
		{
			_world.identity();
			_view.identity();
			_projection.identity();
			
			_updateFlags = UPDATE_ALL;
		}
	}
}