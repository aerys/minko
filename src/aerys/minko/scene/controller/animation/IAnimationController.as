package aerys.minko.scene.controller.animation
{
    public interface IAnimationController
    {
        function get numLabels() : uint;
        function get isPlaying() : Boolean;

        function addLabel(name : String, time : Number) : IAnimationController;
        function removeLabel(name : String) : IAnimationController;

        function seek(time : Object) : IAnimationController;

        function play() : IAnimationController;
        function stop() : IAnimationController;
        function setPlaybackWindow(beginTime	: Object = null,
                                   endTime	    : Object = null) : IAnimationController;

        function resetPlaybackWindow() : IAnimationController;

        function getLabelName(index : uint) : String;

        function getLabelTime(index : uint) : Number;
    }
}
