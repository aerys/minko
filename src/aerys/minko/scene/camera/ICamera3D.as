package aerys.minko.scene.camera 
{
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public interface ICamera3D extends IScene3D
	{
		function get enabled() 	: Boolean;
		function get position()	: Vector4;
		function get lookAt()	: Vector4;
	}
	
}