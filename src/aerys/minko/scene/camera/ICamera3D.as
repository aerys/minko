package aerys.minko.scene.camera 
{
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.math.Transform3D;
	
	import flash.geom.Vector3D;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public interface ICamera3D extends IScene3D
	{
		function get enabled() : Boolean;
		function get x() : Number;
		function get y() : Number;
		function get z() : Number;
		function get lookAtX() : Number;
		function get lookAtY() : Number;
		function get lookAtZ() : Number;
		function get upX() : Number;
		function get upY() : Number;
		function get upZ() : Number;
		
		function getPosition() : Vector3D;
		function getLocalPosition(worldTransform : Transform3D) : Vector3D;
	}
	
}