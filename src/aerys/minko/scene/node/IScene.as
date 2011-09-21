package aerys.minko.scene.node
{
	import aerys.minko.scene.action.IAction;
	
	import flash.events.IEventDispatcher;

	/**
	 * The IScene is the the most basic definition of a scene graph node.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface IScene extends IEventDispatcher
	{
		function get name()		: String;
		function get parents()	: Vector.<IScene>;
		function get actions()	: Vector.<IAction>;
	}
}