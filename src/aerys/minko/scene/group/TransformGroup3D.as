package aerys.minko.scene.group 
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.scene.IObject3D;
	import aerys.minko.transform.Transform3D;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.transform.TransformType;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public class TransformGroup3D extends Group3D implements IObject3D
	{
		private static var _id			: uint			= 0;
		
		private var _transform			: Transform3D	= new Transform3D();
		private var _visible			: Boolean		= true;
		
		public function TransformGroup3D(...children) 
		{
			super(children);
		}
		
		override protected function acceptRenderingQuery(query:RenderingQuery):void
		{
			if (!_visible)
				return ;
			
			var t : TransformManager	= (query as RenderingQuery).transform;
			
			//t.push(TransformType.WORLD);
			t.world.push()
				   .multiply(_transform);
			
			super.acceptRenderingQuery(query);
			
			t.world.pop();
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