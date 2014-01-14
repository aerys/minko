package aerys.minko.scene.controller.animation
{
	import aerys.minko.type.Signal;

    public interface IAnimationController
    {
        function get numLabels() : uint;
        function get isPlaying() : Boolean;
		function get labelHit() : Signal;

        function seek(time : Object) : IAnimationController;
        function play() : IAnimationController;
        function stop() : IAnimationController;
        function setPlaybackWindow(beginTime	: Object = null,
                                   endTime	    : Object = null) : IAnimationController;
        function resetPlaybackWindow() : IAnimationController;

		function addLabel(name : String, time : Number) : IAnimationController;
		function removeLabel(name : String) : IAnimationController;
		function hasLabel(name : String) : Boolean;
        function getLabelName(index : uint) : String;
        function getLabelTime(index : uint) : Number;
		function getLabelTimeByName(name : String) : Number;
		function setTimeForLabel(name : String, newTime : Number) : IAnimationController;
		function changeLabel(oldName : String, newName : String) : IAnimationController;
		function updateNow() : IAnimationController;
    }
}
