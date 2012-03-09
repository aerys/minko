package aerys.minko.scene.controller
{
	/**
	 * The SkeletonController aggregates AnimationController objects
	 * and control them in order to start/stop playing animations on
	 * multiple scene nodes at the same time and control a whole
	 * skeleton.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class SkeletonController
	{
		private var _animations		: Vector.<AnimationController>	= null;
		
		public function SkeletonController(animations	: Vector.<AnimationController>)
		{
			_animations = animations;
		}
		
		public function get animations():Vector.<AnimationController>
		{
			return _animations;
		}

		public function gotoAndPlay(time : Object) : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).gotoAndPlay(time);
		}
		
		public function gotoAndStop(time : Object) : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).gotoAndStop(time);
		}
		
		public function play() : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).play();
		}
		
		public function stop() : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).stop();
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).setPlaybackWindow(beginTime, endTime);
		}
		
		public function resetPlaybackWindow() : void
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).resetPlaybackWindow();
		}
	}
}