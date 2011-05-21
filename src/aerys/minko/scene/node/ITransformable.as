package aerys.minko.scene.node
{
	import aerys.minko.type.math.Transform3D;

	public interface ITransformable extends IScene
	{
		function get transform() : Transform3D;
	}
}