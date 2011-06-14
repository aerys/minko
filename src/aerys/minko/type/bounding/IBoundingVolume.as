package aerys.minko.type.bounding
{
	public interface IBoundingVolume
	{
		function get frustumCulling()	: uint;
		function get boundingSphere() 	: BoundingSphere;
		function get boundingBox() 		: BoundingBox;
	}
}