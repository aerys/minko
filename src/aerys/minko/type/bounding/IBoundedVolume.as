package aerys.minko.type.bounding
{
	public interface IBoundedVolume
	{
		function get boundingSphere() : BoundingSphere;
		function get boundingBox() : BoundingBox;
	}
}