package aerys.minko.render
{
	import aerys.minko.scene.IScene3D;
	import aerys.minko.scene.camera.ICamera3D;

	public interface IScene3DVisitor
	{
		function visit(scene : IScene3D) : void;
		
		function get renderer()	: IRenderer3D;
		function get parent() : IScene3D;
		function get camera() : ICamera3D;
		
		function set renderer(value : IRenderer3D) : void;
	}
}