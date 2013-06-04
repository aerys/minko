/**
 * Created with IntelliJ IDEA.
 * User: promethe
 * Date: 12/04/13
 * Time: 20:03
 * To change this template use File | Settings | File Templates.
 */
package aerys.minko.scene.controller.animation
{
	import flash.utils.Dictionary;
	
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.IRebindableController;
	
	public class MasterAnimationController extends AbstractController implements IAnimationController, IRebindableController
	{
		use namespace minko_animation;
		minko_animation var _animations : Vector.<IAnimationController>;
		private var _isPlaying  		: Boolean;
		
		private var _labelNames : Vector.<String>;
		private var _labelTimes : Vector.<Number>;
		
		public function get numLabels() : uint
		{
			return _labelNames.length;
		}
		
		public function get isPlaying() : Boolean
		{
			return _isPlaying;
		}
		
		public function MasterAnimationController(animations : Vector.<IAnimationController>)
		{
			super();
			
			_animations = animations.concat();
			
			_labelNames = new <String>[];
			_labelTimes = new <Number>[];
		}
		
		public function addLabel(name : String, time : Number) : IAnimationController
		{
			if (_labelNames.indexOf(name) >= 0)
				throw new Error('A label with the same name already exists.');
			
			_labelNames.push(name);
			_labelTimes.push(time);
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
			{
				if (_animations[animationId].hasLabel(name))
					_animations[animationId].removeLabel(name);
				_animations[animationId].addLabel(name,  time);
			}
			return this;
		}
		
		public function hasLabel(name : String) : Boolean
		{
			return _labelNames.indexOf(name) >= 0;
		}
		
		public function removeLabel(name : String) : IAnimationController
		{
			var index : int = _labelNames.indexOf(name);
			
			if (index < 0)
				throw new Error('The time label named \'' + name + '\' does not exist.');
			
			var numLabels : uint = _labelNames.length - 1;
			
			_labelNames[index] = _labelNames[numLabels];
			_labelNames.length = numLabels;
			
			_labelTimes[index] = _labelTimes[numLabels];
			_labelTimes.length = numLabels;
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].removeLabel(name);
			
			return this;
		}
		
		public function seek(time : Object) : IAnimationController
		{
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].seek(time);
			
			return this;
		}
		
		public function play() : IAnimationController
		{
			var numAnimations : uint = _animations.length;
			
			_isPlaying = true;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].play();
			
			return this;
		}
		
		public function stop() : IAnimationController
		{
			var numAnimations : uint = _animations.length;
			
			_isPlaying = false;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].stop();
			
			return this;
		}
		
		public function setPlaybackWindow(beginTime : Object = null, endTime : Object = null) : IAnimationController
		{
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].setPlaybackWindow(beginTime, endTime);
			
			return this;
		}
		
		public function resetPlaybackWindow() : IAnimationController
		{
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].resetPlaybackWindow();
			
			return this;
		}
		
		public function getLabelName(index : uint) : String
		{
			return _labelNames[index];
		}
		
		public function getLabelTime(index : uint) : Number
		{
			return _labelTimes[index];
		}
		
		public function getLabelTimeByName(name : String) : Number
		{
			var index : int = _labelNames.indexOf(name);
			
			if (index < 0)
				throw new Error('The time label named \'' + name + '\' does not exist.');
			
			return _labelTimes[index];
		}
		
		override public function clone():AbstractController
		{			
			var newController : MasterAnimationController = new MasterAnimationController(_animations);
			newController._labelNames = _labelNames;
			newController._labelTimes = _labelTimes;
			
			return newController;
		}
		
		public function rebindDependencies(nodeMap : Dictionary, controllerMap : Dictionary) : void
		{
			var newAnimations : Vector.<IAnimationController> = new Vector.<IAnimationController>();
			
			for(var i : int = 0; i < _animations.length; ++i)
			{
				var newController : IAnimationController = controllerMap[_animations[i]] as IAnimationController;
				if (newController)
					newAnimations.push(newController);
			}
			
			_animations = newAnimations;
		}
		
		public function changeLabel(oldName : String, newName : String) : IAnimationController
		{
			var index : int = _labelNames.indexOf(oldName);
			
			if (index < 0)
				throw new Error('The time label named \'' + oldName + '\' does not exist.');
			
			_labelNames[index] = newName;
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
			{
				if (!_animations[animationId].hasLabel(oldName))
					_animations[animationId].addLabel(oldName, getLabelTime(index));
				_animations[animationId].changeLabel(oldName, newName);
			}
			return this;
		}
		
		public function setTimeForLabel(name : String, newTime : Number) : IAnimationController
		{
			var index : int = _labelNames.indexOf(name);
			
			if (index < 0)
				throw new Error('The time label named \'' + name + '\' does not exist.');
			
			_labelTimes[index] = newTime;
			
			var numAnimations : uint = _animations.length;
			
			for (var animationId : uint = 0; animationId < numAnimations; ++animationId)
				_animations[animationId].setTimeForLabel(name, newTime);
			
			return this;
		}
	}
}
