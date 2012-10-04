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
		private var _animations	: Vector.<AnimationController>	= null;
		
		public function SkeletonController(animations : Vector.<AnimationController>)
		{
			_animations = animations.slice();
		}
		
		public function get animations() : Vector.<AnimationController>
		{
			return _animations;
		}

		public function seek(time : Object) : SkeletonController
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).seek(time);
			
			return this;
		}
		
		public function stop() : SkeletonController
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).stop();
			
			return this;
		}
		
		public function play() : SkeletonController
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).play();
			
			return this;
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : SkeletonController
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).setPlaybackWindow(beginTime, endTime);
			
			return this;
		}
		
		public function resetPlaybackWindow() : SkeletonController
		{
			var numAnimations	: uint	= _animations.length;
			
			for (var animId : uint = 0; animId < numAnimations; ++animId)
				(_animations[animId] as AnimationController).resetPlaybackWindow();
			
			return this;
		}
	}
}