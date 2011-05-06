package aerys.minko.scene.graph.mesh.modifier
{
	import aerys.minko.scene.graph.mesh.IMesh;

	public interface IMeshModifier extends IMesh
	{
		function get target() : IMesh;
	}
}