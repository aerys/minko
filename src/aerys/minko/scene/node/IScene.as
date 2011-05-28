package aerys.minko.scene.node
{
	import aerys.minko.scene.action.IActionTarget;
	import aerys.minko.scene.visitor.ISceneVisitor;

	public interface IScene extends IActionTarget
	{
		function get name() : String;
	}
}