package aerys.minko.scene.node.group 
{
	import aerys.minko.scene.visitor.data.TransformManager;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.scene.node.ITransformable;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Matrix4x4;
	
	/**
	 * TransformGroup3D apply a 3D transform to their children.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public class TransformGroup extends Group implements ITransformable
	{
		private static var _id	: uint			= 0;
		
		private var _transform	: Transform3D	= new Transform3D();
		
		public function TransformGroup(...children) 
		{
			super(children);
		}
		
//		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
//		{
//			var worldTransform : Matrix4x4	= query.transform.world;
//			
//			worldTransform.push()
//				   	 	  .multiply(_transform);
//			
//			super.visitedByRenderingVisitor(query);
//			
//			worldTransform.pop();
//		}
		
		/**
		 * The Transform3D object defining the transform of the object into world-space.
		 */
		public function get transform() : Transform3D
		{
			return _transform;
		}

	}

}