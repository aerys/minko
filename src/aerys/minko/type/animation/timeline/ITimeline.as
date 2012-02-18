package aerys.minko.type.animation.timeline
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.math.Matrix4x4;

	public interface ITimeline
	{
		function get duration()		: uint;
		function get propertyPath()	: String;

		function updateAt(time : int, target : Object) : void;
		function clone() : ITimeline;
	}
}
