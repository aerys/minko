package aerys.minko.scene.group 
{
	import aerys.minko.query.rendering.RenderingQuery;
	import aerys.minko.query.rendering.TransformManager;
	import aerys.minko.scene.IObject3D;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Matrix4x4;
	
	/**
	 * TransformGroup3D apply a 3D transform to their children.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public class TransformGroup3D extends Group3D implements IObject3D
	{
		private static var _id	: uint			= 0;
		
		private var _transform	: Transform3D	= new Transform3D();
		private var _visible	: Boolean		= true;
		
		public function TransformGroup3D(...children) 
		{
			super(children);
		}
		
		override protected function acceptRenderingQuery(query:RenderingQuery):void
		{
			if (!_visible)
				return ;
			
			var worldTransform : Matrix4x4	= query.transform.world;
			
			worldTransform.push()
				   	 	  .multiply(_transform);
			
			super.acceptRenderingQuery(query);
			
			worldTransform.pop();
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