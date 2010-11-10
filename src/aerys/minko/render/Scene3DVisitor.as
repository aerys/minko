package aerys.minko.render
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.vertex.formats.IVertex3DFormat;
	
	import flash.display3D.Context3D;
	
	public class Scene3DVisitor implements IScene3DVisitor
	{
		use namespace minko;
		
		private var _renderer	: IRenderer3D		= null;
		
		private var _current	: IScene3D			= null;
		private var _parent		: IScene3D			= null;
		private var _parents	: Vector.<IScene3D>	= new Vector.<IScene3D>();
		
		public function get renderer() 		: IRenderer3D	{ return _renderer; }
		public function get parent()		: IScene3D		{ return _parent; }

		public function set renderer(value : IRenderer3D) : void	{ _renderer = value; }	
		
		public function Scene3DVisitor(myViewport : Viewport3D)
		{
			_renderer = new NativeRenderer3D(myViewport);
		}
				
		public function visit(scene : IScene3D):void
		{
			_parents[_parents.length] = _parent;
			_parent = _current;
			_current = scene;
			
			scene.visited(this);
			
			_current = _parent;
			_parent = _parents[(_parents.length - 1)];
			_parents.length--;
		}
		
	}
}