package aerys.minko.scene.node.camera
{
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public interface ICamera extends IScene, IVersionable
	{
		function get enabled() 		: Boolean;

		function get position()		: Vector4;
		function get lookAt()		: Vector4;
		function get up()			: Vector4;

		function get fieldOfView()	: Number;
		function get nearClipping()	: Number;
		function get farClipping()	: Number;
	}
}
