package aerys.minko.scene.node
{
	import aerys.minko.type.Signal;

	public interface ITaggable
	{
		function get tag() : uint;
		
		/**
		 * function tagChangedHandler(node : ISceneNode, oldTag : uint, oldTag : uint)
		 */
		function get tagChanged() : Signal;
	}
}