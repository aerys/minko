package aerys.minko.scene.node
{
	import aerys.minko.scene.action.IActionTarget;
	import aerys.minko.scene.visitor.ISceneVisitor;

	/**
	 * The IScene is the the most basic definition of a scene graph node.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface IScene extends IActionTarget
	{
		function get name() : String;
	}
}