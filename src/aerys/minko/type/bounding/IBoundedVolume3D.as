package aerys.minko.type.bounding
{
	public interface IBoundedVolume3D
	{
		function get boundingSphere() : BoundingSphere3D;
		function get boundingBox() : BoundingBox3D;
	}
}