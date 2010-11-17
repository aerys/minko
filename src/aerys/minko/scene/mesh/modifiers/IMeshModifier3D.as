package aerys.minko.scene.mesh.modifiers
{
	import aerys.minko.scene.mesh.IMesh3D;

	public interface IMeshModifier3D extends IMesh3D
	{
		function get target() : IMesh3D;
	}
}