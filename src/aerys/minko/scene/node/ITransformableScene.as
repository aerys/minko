package aerys.minko.scene.node
{
	import aerys.minko.type.math.Transform3D;

	public interface ITransformableScene extends IScene
	{
		function get transform() : Transform3D;
	}
}