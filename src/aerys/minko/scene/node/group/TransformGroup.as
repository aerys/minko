package aerys.minko.scene.node.group 
{
	import aerys.minko.scene.visitor.data.TransformManager;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.scene.node.ITransformable;
	import aerys.minko.type.math.Transform3D;
	import aerys.minko.type.math.Matrix4x4;
	
	/**
	 * TransformGroup apply a 3D transform to their children.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public class TransformGroup extends Group implements ITransformable
	{
		private var _transform	: Transform3D	= new Transform3D();
		
		public function TransformGroup(...children) 
		{
			super(children);
		}
		
		/**
		 * The Transform3D object defining the transform of the object into world-space.
		 */
		public function get transform() : Transform3D
		{
			return _transform;
		}

	}

}