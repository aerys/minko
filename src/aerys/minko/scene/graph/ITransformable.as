package aerys.minko.scene.graph
{
	import aerys.minko.type.Transform3D;

	public interface ITransformable extends IScene
	{
		function get transform() : Transform3D;
	}
}