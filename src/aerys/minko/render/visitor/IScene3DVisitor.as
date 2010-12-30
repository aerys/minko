package aerys.minko.render.visitor
{
	import aerys.minko.scene.IScene3D;
	import aerys.minko.scene.camera.ICamera3D;
	import aerys.minko.render.renderer.IRenderer3D;

	public interface IScene3DVisitor
	{
		function visit(scene : IScene3D, visitor : IScene3DVisitor = null) : void;
		
		function get renderer()	: IRenderer3D;
		function get parent() : IScene3D;
		function get camera() : ICamera3D;
		
		function set renderer(value : IRenderer3D) : void;
	}
}