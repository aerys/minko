package aerys.minko.render.visitor
{
	import aerys.minko.scene.IScene3D;
	import aerys.minko.scene.camera.ICamera3D;
	import aerys.minko.render.renderer.IRenderer3D;
	
	public class Scene3DVisitorModifier implements IScene3DVisitorModifier
	{
		private var _target	: IScene3DVisitor	= null;
		
		public function get target() : IScene3DVisitor	{ return _target; }
		
		public function set target(value : IScene3DVisitor) : void
		{
			_target = value;
		}
		
		public function Scene3DVisitorModifier(target : IScene3DVisitor = null)
		{
			if (target)
				this.target = target;
		}
		
		public function visit(scene : IScene3D, visitor : IScene3DVisitor = null) : void
		{
			_target.visit(scene, this);
		}
		
		public function get renderer() : IRenderer3D
		{
			return _target.renderer;
		}
		
		public function get parent() : IScene3D
		{
			return _target.parent;
		}
		
		public function get camera() : ICamera3D
		{
			return _target.camera;
		}
		
		public function set renderer(value : IRenderer3D) : void
		{
			_target.renderer = value;
		}
	}
}