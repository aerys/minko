package aerys.minko.scene.node.group
{
	import aerys.minko.ns.minko;
	import aerys.minko.scene.visitor.data.TransformManager;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public class BillboardGroup extends Group
	{
		use namespace minko;
		
		private static const UP				: Vector4	= new Vector4(0., -1., 0.);
		
		private static var _id				: uint		= 0;
		
		private var _local			: Transform3D	= new Transform3D();
		private var _localCamera	: Vector4		= new Vector4();
		
		public function BillboardGroup(...children)
		{
			super(children);
		}
		
//		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
//		{
//			var transform	: TransformManager	= query.transform;
//			
//			Matrix4x4.multiply(transform.world, transform.view, _local)
//					 .invert()
//					 .multiplyVector(ConstVector4.ZERO, _localCamera);
//			
//			_local.identity();
//			_local.pointAt(_localCamera,
//						   ConstVector4.Z_AXIS,
//						   UP);
//			
//			//transform.push(TransformType.WORLD);
//			transform.world.push()
//						   .multiply(_local);	
//			
//			super.visitedByRenderingVisitor(query);
//			
//			transform.world.pop();
//		}
	}
}