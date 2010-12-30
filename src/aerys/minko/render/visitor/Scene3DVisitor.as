package aerys.minko.render.visitor
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.Viewport3D;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.NativeRenderer3D;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.scene.camera.ICamera3D;
	
	public class Scene3DVisitor implements IScene3DVisitor
	{
		use namespace minko;
		
		private var _renderer	: IRenderer3D		= null;
		
		private var _current	: IScene3D			= null;
		private var _parent		: IScene3D			= null;
		private var _parents	: Vector.<IScene3D>	= new Vector.<IScene3D>();
		private var _camera		: ICamera3D			= null;
		
		public function get renderer() 		: IRenderer3D	{ return _renderer; }
		public function get parent()		: IScene3D		{ return _parent; }
		public function get camera()		: ICamera3D		{ return _camera; }
		
		public function set renderer(value : IRenderer3D) : void	{ _renderer = value; }	
		
		public function Scene3DVisitor(renderer : IRenderer3D)
		{
			_renderer = renderer;
		}
				
		public function visit(scene : IScene3D, visitor : IScene3DVisitor = null) : void
		{
			_parents[_parents.length] = _parent;
			_parent = _current;
			_current = scene;
			
			var camera : ICamera3D = null;
			
			if ((camera = scene as ICamera3D) && camera.enabled)
				_camera = camera;
			
			scene.visited(visitor || this);
			
			_current = _parent;
			_parent = _parents[(_parents.length - 1)];
			_parents.length--;
		}
		
	}
}