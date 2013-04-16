package aerys.minko.type.animation.timeline
{
	

	public interface ITimeline
	{
		function get duration()		: uint;
		function get propertyPath()	: String;

		function updateAt(time : int, target : Object) : void;
		function clone() : ITimeline;
	}
}
