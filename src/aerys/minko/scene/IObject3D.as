package aerys.minko.scene
{
	import aerys.minko.transform.Transform3D;

	public interface IObject3D extends IScene3D
	{
		function get transform() : Transform3D;
	}
}