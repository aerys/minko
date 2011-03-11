package aerys.minko.scene.camera 
{
	import aerys.common.IVersionnable;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.math.Frustum3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public interface ICamera3D extends IScene3D, IVersionnable
	{
		function get enabled() 		: Boolean;
		
		function get position()		: Vector4;
		function get lookAt()		: Vector4;
		function get up()			: Vector4;
		
		function get fieldOfView()	: Number;
		function get nearClipping()	: Number;
		function get farClipping()	: Number;
		
		function get frustum()		: Frustum3D;
	}
	
}