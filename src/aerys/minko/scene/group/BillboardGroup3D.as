package aerys.minko.scene.group
{
	import aerys.minko.ns.minko;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.transform.Transform3D;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.transform.TransformType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public class BillboardGroup3D extends Group3D
	{
		use namespace minko;
		
		private static const UP				: Vector4	= new Vector4(0., -1., 0.);
		
		private static var _id				: uint		= 0;
		
		private var _local			: Transform3D	= new Transform3D();
		private var _localCamera	: Vector4		= new Vector4();
		
		public function BillboardGroup3D(...children)
		{
			super(children);
		}
		
		override protected function acceptRenderingQuery(query : RenderingQuery) : void
		{
			var transform	: TransformManager	= query.transform;
			
			Matrix4x4.multiply(transform.world, transform.view, _local)
					 .invert()
					 .multiplyVector(Vector4.ZERO, _localCamera);
			
			_local.identity();
			_local.pointAt(_localCamera,
						   Vector4.Z_AXIS,
						   UP);
			
			transform.push(TransformType.WORLD);
			transform.world.multiply(_local);	
			
			super.acceptRenderingQuery(query);
			
			transform.pop();
		}
	}
}