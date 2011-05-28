package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;

	public interface IAction
	{
		function get name() : String;
		
		function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean;
		function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean;
		function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean;
	}
}