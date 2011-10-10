package aerys.minko.scene.node
{
	import aerys.minko.type.math.Matrix4x4;

	public interface ITransformableScene extends IScene
	{
		function get transform() : Matrix4x4;
	}
}