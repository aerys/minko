package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;

	/**
	 * IAction objects define what actions should be executed when
	 * a scene graph node is traversed.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface IAction
	{
		function get type() : uint;
		
		function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean;
	}
}