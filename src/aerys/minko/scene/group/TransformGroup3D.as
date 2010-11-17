package aerys.minko.scene.group 
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.render.transform.TransformType;
	import aerys.minko.scene.IObject3D;
	import aerys.minko.type.math.Transform3D;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public class TransformGroup3D extends Group3D implements IObject3D
	{
		use namespace minko;
		
		private static var _id			: uint			= 0;
		
		private var _transform			: Transform3D	= new Transform3D();
		
		private var _visible			: Boolean		= true;
		
		public function TransformGroup3D(...children) 
		{
			super(children);
			
			name = "TransformContainer3D_" + _id++;
		}
		
		/**
		 * @param myContext The FrameContext object that describes the frame being rendered.
		 * @return The number of triangles that have been/will be rendered.
		 * @see aerys.minko.frame.FrameContext
		 */
		override public function visited(myVisitor : IScene3DVisitor) : void
		{
			if (!_visible)
				return ;
			
			var t : TransformManager	= myVisitor.renderer.transform;
			
			t.push(TransformType.WORLD);
			t.world.append(_transform);
			
			super.visited(myVisitor);
			
			t.pop();
		}
		
		public function set transform(value : Transform3D) : void
		{
			_transform = value;
		}
		
		/**
		 * The Transform3D object defining the transform of the object into world-space.
		 * When setting the transform attribute the position, rotation and scale are
		 * updated according to the specified transform.
		 */
		public function get transform() : Transform3D
		{
			return _transform;
		}

	}

}