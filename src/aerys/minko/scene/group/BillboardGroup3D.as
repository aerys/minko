package aerys.minko.scene.group
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.transform.TransformType;
	import aerys.minko.transform.Transform3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public class BillboardGroup3D extends Group3D
	{
		use namespace minko;
		
		private static const TRANSFORM3D	: Factory	= Factory.getFactory(Transform3D);
		private static const UP				: Vector4	= new Vector4(0., -1., 0.);
		
		private static var _id				: uint		= 0;
		
		private var _local	: Transform3D	= new Transform3D();
		private var _camera	: Vector4		= new Vector4();
		
		public function BillboardGroup3D(...children)
		{
			super(children);
			
			name = "BillboardContainer3D_" + ++_id;
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			if (query is RenderingQuery)
			{
				var q			: RenderingQuery	= query as RenderingQuery;
				var transform	: TransformManager	= q.transform;
				
				Matrix4x4.invert(transform.world, _local);
				
				//_local.multiplyVector(transform.cameraPosition, _camera);
				_local.pointAt(_camera,
							   Vector4.Z_AXIS,
							   UP);
				
				transform.push(TransformType.WORLD);
				transform.world = _local;			
				
				super.accept(query);
				
				transform.pop();
			}
		}
	}
}