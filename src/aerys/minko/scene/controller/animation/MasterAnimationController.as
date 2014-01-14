package aerys.minko.scene.controller.animation
{
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
	
	import aerys.minko.ns.minko_animation;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.IRebindableController;
	import aerys.minko.scene.node.Scene;
	
	use namespace minko_animation;
	
	public class MasterAnimationController extends AbstractAnimationController implements IRebindableController
	{		
		minko_animation var _animations : Vector.<AnimationController>	= null;
		
		public function MasterAnimationController(animations	: Vector.<AnimationController>, 
												  loop			: Boolean = true)
		{
			super(loop);

			_animations = animations.concat();
			
			var maxTime : int = 0;
			
			for each (var animation : AnimationController in _animations)
			{
				animation.setMaster(this);
				maxTime = Math.max(maxTime, animation.totalTime);
			}
			
			_totalTime		= maxTime;
			
			setPlaybackWindow(0, _totalTime);
			seek(0).play();
		}
		
		override public function addLabel(name : String, time : Number) : IAnimationController
		{
			super.addLabel(name, time);
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
			{
				if (_animations[animationId].hasLabel(name))
					_animations[animationId].removeLabel(name);
				_animations[animationId].addLabel(name,  time);
			}
			
			return this;
		}
				
		override public function removeLabel(name : String) : IAnimationController
		{
			super.removeLabel(name);
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].removeLabel(name);
			
			return this;
		}
		
		override public function play() : IAnimationController
		{
			super.play();
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].play();
			
			return this;
		}
		
		override public function stop() : IAnimationController
		{
			super.stop();
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].stop();
			
			return this;
		}
		
		
		override public function setPlaybackWindow(beginTime : Object = null, endTime : Object = null) : IAnimationController
		{
			super.setPlaybackWindow(beginTime, endTime);
						
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].setPlaybackWindow(beginTime, endTime);
			
			return this;
		}
		
		override public function resetPlaybackWindow() : IAnimationController
		{
			super.resetPlaybackWindow();
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].resetPlaybackWindow();
			
			return this;
		}
		
		override public function clone():AbstractController
		{
			var newController : MasterAnimationController = new MasterAnimationController(_animations);
			newController._labelNames = _labelNames.concat();
			newController._labelTimes = _labelTimes.concat();
			
			return newController;
		}
		
		public function rebindDependencies(nodeMap : Dictionary, controllerMap : Dictionary) : void
		{
			var newAnimations : Vector.<AnimationController> = new Vector.<AnimationController>();
			
			for(var i : int = 0; i < _animations.length; ++i)
			{
				var newController : AbstractAnimationController = controllerMap[_animations[i]] as AbstractAnimationController;
				if (newController)
					newAnimations.push(newController);
			}
			
			_animations = newAnimations;
		}
		
		override public function changeLabel(oldName : String, newName : String) : IAnimationController
		{
			super.changeLabel(oldName, newName);
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].changeLabel(oldName, newName);
			
			return this;
		}
		
		override public function setTimeForLabel(name : String, newTime : Number) : IAnimationController
		{
			super.setTimeForLabel(name, newTime);
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].setTimeForLabel(name, newTime);
			
			return this;
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport, 
														   destination	: BitmapData, 
														   time			: Number) : void
		{
			if (updateOnTime(time))
			{
				updateAnimations(scene, time);
				
				checkLabelHit(_previousTime, _currentTime);
			}
		}
		
		minko_animation function updateAnimations(scene : Scene, time : Number) : void
		{
			_updateOneTime = false;
			
			var numAnimations : uint = _animations.length;
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
			{
				_animations[animationId].setCurrentTime(currentTime);
				_animations[animationId].update(time, scene);
			}
		}
		
		override minko_animation function triggerLabelHit(labelName:String, labelTime : int) : void
		{
			var numAnimations : uint = _animations.length;
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].triggerLabelHit(labelName, labelTime);
			
			super.triggerLabelHit(labelName, labelTime);
		}
		
		override public function invalidate(target : Object = null) : void
		{
			var numAnimations : uint = _animations.length;
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].invalidate(target);
		}
	}
}
