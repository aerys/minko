package aerys.minko.scene.camera
{
	import aerys.common.Factory;
	import aerys.minko.render.visitor.IScene3DVisitor;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix3D;
	
	public class ArcBallCamera3D extends AbstractCamera3D
	{
		private static const MATRIX3D		: Factory	= Factory.getFactory(Matrix3D);
		private static const PI_DIV_2		: Number	= Math.PI / 2.;
		private static const PI_2			: Number	= Math.PI * 2.;
		private static const EPSILON		: Number	= 0.01;
		
		private var _distance	: Number	= 1.;
		private var _rotation	: Vector4	= new Vector4();
		
		private var _rv			: uint		= 0;
		
		public function ArcBallCamera3D()
		{
			super();
		}
		
		//{ region getters/setters
		public function get distance() : Number { return _distance; }
		
		public function set distance(value : Number) : void
		{
			if (value != _distance)
			{
				_distance = value;
				invalidate();
			}
		}
		
		public function get rotation() : Vector4
		{
			return _rotation;
		}
		//} endregion
		
		//{ region methods
		override protected function invalidateTransform(visitor : IScene3DVisitor = null) : void
		{
			if (_rotation.x >= PI_DIV_2)
				_rotation.x = PI_DIV_2 - EPSILON;
			else if (_rotation.x <= -PI_DIV_2)
				_rotation.x = -PI_DIV_2 + EPSILON;
				
			if (_distance <= 0.)
				_distance = EPSILON;

			position.x = lookAt.x - _distance * Math.sin(_rotation.y) * Math.cos(_rotation.x);
			position.y = lookAt.y - _distance * Math.sin(_rotation.x);
			position.z = lookAt.z - _distance * Math.cos(_rotation.y) * Math.cos(_rotation.x);
			
			super.invalidateTransform(visitor);
		}
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			if (_rotation.version != _rv)
			{
				invalidate();
				_rv = _rotation.version;
			}
			
			super.visited(visitor);
		}
		//} endregion
	}
}