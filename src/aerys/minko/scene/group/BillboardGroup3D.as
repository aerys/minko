package aerys.minko.scene.group
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.IRenderer3D;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.render.transform.TransformType;
	import aerys.minko.render.visitor.IScene3DVisitor;
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
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			var renderer	: IRenderer3D 		= visitor.renderer;
			var transform	: TransformManager	= renderer.transform;
			
			Matrix4x4.invert(transform.world, _local);
			
			_local.multiplyVector(visitor.camera.position, _camera);
			_local.pointAt(_camera,
						   Vector4.Z_AXIS,
						   UP);
			
			transform.push(TransformType.WORLD);
			transform.world = _local;			
			
			super.visited(visitor);
			
			transform.pop();
		}
	}
}