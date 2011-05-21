package aerys.minko.type.interpolation
{
	import aerys.minko.type.math.Transform3D;
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
		
		function position(t : Number, out : Vector4) : Vector4;
		function tangent(t : Number, out : Vector4) : Vector4;
		function pointAt(t : Number, out : Vector4) : Vector4;
		
		/**
		 * Alias for position
		 */
		function translation(t : Number, out : Vector4) : Vector4;
		
		/**
		 * Alias for tangent
		 */
		function lookAt(t : Number, out : Vector4) : Vector4;
		
		function updateMatrix(matrix : Matrix4x4, t : Number) : void;
		function updateTransform(transform : Transform3D, t : Number) : void;

	}
}