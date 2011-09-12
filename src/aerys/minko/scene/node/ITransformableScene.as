package aerys.minko.scene.node
{
	import aerys.minko.type.math.Matrix3D;

	public interface ITransformableScene extends IScene
	{
		function get transform() : Matrix3D;
	}
}