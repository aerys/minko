package aerys.minko.scene.controller
{
	import flash.utils.Dictionary;

	public interface IRebindableController
	{
		function rebindDependencies(nodeMap 		: Dictionary,
									controllerMap	: Dictionary) : void;
	}
}
