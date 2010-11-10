package aerys.minko.scene.group
{
	import aerys.common.Factory;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.Scene3DVisitor;
	import aerys.minko.render.transform.TransformType;
	import aerys.minko.type.math.Transform3D;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public class BillboardGroup3D extends Group3D
	{
		private static const TRANSFORM3D	: Factory	= Factory.getFactory(Transform3D);
		private static const UP				: Vector3D	= new Vector3D(0., -1., 0.);
		
		private static var _id				: uint		= 0;
		
		private var _transform	: Transform3D	= new Transform3D();
		
		public function BillboardGroup3D(...children)
		{
			super(children);
			
			name = "BillboardContainer3D_" + ++_id;
		}
		
		override public function visited(myVisitor : IScene3DVisitor) : void
		{
			var t 		 : Transform3D = _transform.clone(true);
			var renderer : IRenderer3D = myVisitor.renderer;
			
			/*t.pointAt(myVisitor.transform.localCameraPosition,
				      Vector3D.Z_AXIS,
					  UP);*/
			
			renderer.transform.push(TransformType.WORLD);
			renderer.transform.world.append(t);
			
			super.visited(myVisitor);
			
			renderer.transform.pop();
		}
	}
}