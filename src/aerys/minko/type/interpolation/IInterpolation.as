package aerys.minko.type.interpolation
{
	import aerys.minko.type.animation.timeline.ITimeline;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public interface IInterpolation
	{
		function get checkpoints() : Vector.<Vector4>;
		
		function get start() : Vector4;
		function get end() : Vector4;
		function get length() : Number;
		
		function set start(value : Vector4) : void;
		function set end(value : Vector4) : void;
		
		function position(t : Number, out : Vector4 = null) : Vector4;
		function tangent(t : Number, out : Vector4 = null) : Vector4;
		function pointAt(t : Number, out : Vector4 = null) : Vector4;
		
		/**
		 * Alias for position
		 */
		function translation(t : Number, out : Vector4 = null) : Vector4;
		
		/**
		 * Alias for tangent
		 */
		function lookAt(t : Number, out : Vector4 = null) : Vector4;
		
		function updateMatrix(t : Number, matrix : Matrix4x4 = null) : Matrix4x4;
		
		function clone() : IInterpolation;
	}
}