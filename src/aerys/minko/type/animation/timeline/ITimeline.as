package aerys.minko.type.animation.timeline
{
	import aerys.minko.type.math.Matrix4x4;

	public interface ITimeline
	{
		function get duration() : uint;
		function get target() : String;
		function setMatrixAt(t : uint, out : Matrix4x4) : void;
	}
}
