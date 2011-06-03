package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.camera.ICamera;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.bounding.IBoundingVolume;
	import aerys.minko.type.math.ConstVector4;

	public class BVHGroup extends Group implements IBoundingVolume
	{
		private var _box	: BoundingBox		= new BoundingBox(ConstVector4.ZERO, ConstVector4.ONE);
		private var _sphere	: BoundingSphere	= new BoundingSphere(ConstVector4.ZERO, 1.);
		
		public function get boundingBox()		: BoundingBox		{ return _box; }
		public function get boundingSphere()	: BoundingSphere	{ return _sphere; }
		
		public function BVHGroup(...children)
		{
			super (children);
		}
		
//		override public function visited(query : ISceneVisitor) : void
//		{
//			if (query is RenderingVisitor)
//			{
//				var renderingQuery 	: RenderingVisitor 	= query as RenderingVisitor;
//				var camera			: ICamera			= renderingQuery.camera;
//				
//				if (renderingQuery && camera
//					&& camera.frustum.testBoundedVolume(this, renderingQuery.transform.world) == 0)
//				{
//					return ;
//				}
//			}
//		}
	}
}