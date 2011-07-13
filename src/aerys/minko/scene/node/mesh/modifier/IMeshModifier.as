package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.node.ISearchableScene;
	import aerys.minko.scene.node.mesh.IMesh;

	public interface IMeshModifier extends IMesh, ISearchableScene
	{
		function get target() : IMesh;
	}
}