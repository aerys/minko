package aerys.minko.scene.controller.animation
{
	import flash.utils.getTimer;
	
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	use namespace minko_animation;
	
	public class AbstractAnimationController extends EnterFrameController implements IAnimationController
	{
		
		protected var _isPlaying  				: Boolean;
		
		protected var _updateOneTime			: Boolean;
		
		protected var _loopBeginTime			: int;
		protected var _loopEndTime				: int;
		protected var _looping					: Boolean;
		protected var _currentTime				: int;
		protected var _previousTime				: int;
		protected var _totalTime				: int;
		
		protected var _timeFunction				: Function;
		
		protected var _lastTime					: int;
		
		protected var _labelNames				: Vector.<String>;
		protected var _labelTimes				: Vector.<Number>;
		
		protected var _looped					: Signal;
		protected var _started					: Signal;
		protected var _stopped					: Signal;
		protected var _labelHit					: Signal;
		
		private var _nextLabelIds				: Array;
		
		
		public function get numLabels() : uint
		{
			return _labelNames.length;
		}
		
		public function get timeFunction() : Function
		{
			return _timeFunction;
		}
		
		public function set timeFunction(value : Function) : void
		{
			_timeFunction = value;
		}
		
		public function get started() : Signal
		{
			return _started;
		}
		
		public function get stopped() : Signal
		{
			return _stopped;
		}
		
		public function get looped() : Signal
		{
			return _looped;
		}
		
		public function get labelHit():Signal
		{
			return _labelHit;
		}
		
		public function get totalTime() : int
		{
			return _totalTime;
		}
		
		public function get looping() : Boolean
		{
			return _looping;
		}
		public function set looping(value : Boolean) : void
		{
			_looping = value;
		}
		
		public function get isPlaying() : Boolean
		{
			return _isPlaying;
		}
		public function set isPlaying(value : Boolean) : void
		{
			_isPlaying = value;
		}
		
		public function get currentTime() : int
		{
			return _currentTime;
		}
		
		minko_animation function setCurrentTime(v : int) : void
		{
			_currentTime = v;
		}
		
		public function AbstractAnimationController(loop : Boolean = true)
		{
			super();
			
			_looping = true;
			
			_labelNames = new <String>[];
			_labelTimes = new <Number>[];
			
			_looped	= new Signal('AnimationController.looped');
			_started = new Signal('AnimationController.started');
			_stopped = new Signal('AnimationController.stopped');
			_labelHit = new Signal('AnimationController.labelHit');
		}
		
		public function seek(time : Object) : IAnimationController
		{
			var timeValue : uint = getAnimationTime(time);
			
			if (timeValue < _loopBeginTime || timeValue > _loopEndTime)
				throw new Error(
					'Time value is outside of playback window. '
					+'To reset playback window, call resetPlaybackWindow.'
				);
			
			_currentTime = timeValue;
			
			updateNextLabel();
			checkLabelHit(_currentTime, _currentTime);
			
			return this;
		}
		
		protected function updateNextLabel(currentTime : int = -1) : void
		{
			_nextLabelIds = [];
			
			if (!_labelTimes.length)
				return;
			
			if (currentTime == -1)
				currentTime = _currentTime;
			
			var min : int = int.MAX_VALUE;
			
			for(var i : int = 0; i < _labelTimes.length; ++i)
			{
				var time : int = _labelTimes[i];
				
				if (time < _loopBeginTime || time > _loopEndTime)
					continue;
				
				if (time >= currentTime)
				{
					if (time < min)
					{
						min = time;
						_nextLabelIds = [i];
					}
					else if (time == min)
					{
						_nextLabelIds.push(i);
					}
				}
			}
			
			if (!_nextLabelIds.length)
			{
				updateNextLabel(_loopBeginTime);
			}
			else if (min == _loopEndTime)
			{
				for(i = 0; i < _labelTimes.length; ++i)
				{
					if (_labelTimes[i] == _loopBeginTime && _nextLabelIds.indexOf(i) == -1)
						_nextLabelIds.push(i);
				}
			}
		}
		
		public function play() : IAnimationController
		{
			_isPlaying = true;
			_lastTime = _timeFunction != null ? _timeFunction(getTimer()) : getTimer();
			_started.execute(this);
			
			return this;
		}
		
		public function stop() : IAnimationController
		{
			_isPlaying = false;
			_updateOneTime 	= true;
			_lastTime = _timeFunction != null ? _timeFunction(getTimer()) : getTimer();
			_stopped.execute(this);
			
			return this;
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : IAnimationController
		{
			_loopBeginTime	= beginTime != null ? getAnimationTime(beginTime) : 0;
			_loopEndTime	= endTime != null ? getAnimationTime(endTime) : _totalTime;
			
			if (_currentTime < _loopBeginTime || _currentTime > _loopEndTime)
				_currentTime = _loopBeginTime;
			
			updateNextLabel();
			checkLabelHit(_currentTime, _currentTime);
			
			return this;
		}
		
		public function resetPlaybackWindow() : IAnimationController
		{
			setPlaybackWindow();
			
			return this;
		}
		
		protected function getAnimationTime(time : Object) : uint
		{
			var timeValue : uint;
			
			if (time is uint || time is int || time is Number)
			{
				timeValue = uint(time);
			}
			else if (time is String)
			{
				var labelCount : uint = _labelNames.length;
				
				for (var labelId : uint = 0; labelId < labelCount; ++labelId)
					if (_labelNames[labelId] == time)
						timeValue = _labelTimes[labelId];
			}
			else
			{
				throw new Error('Invalid argument type: time must be Number or String');
			}
			
			return timeValue;
		}
		
		protected function updateOnTime(time : Number) : Boolean
		{
			if (_isPlaying || _updateOneTime)
			{
				_previousTime = _currentTime;
				if (_timeFunction != null)
					time = _timeFunction(time);
				
				var deltaT 			: Number = time - _lastTime;
				var lastCurrentTime : Number = _currentTime;
				
				if (_isPlaying)
				{
					_currentTime = _loopBeginTime
						+ (_currentTime + deltaT - _loopBeginTime)
						% (_loopEndTime - _loopBeginTime);
				}
				
				if ((deltaT > 0 && lastCurrentTime > _currentTime)
					|| (deltaT < 0 && (lastCurrentTime < _currentTime
						|| _currentTime * lastCurrentTime < 0)))
				{
					if (_looping)
						_looped.execute(this);
					else
					{
						_currentTime = deltaT > 0 ? _totalTime : 0;
						stop();
						
						return true;
					}
				}
			}				
			
			_lastTime = time;
			
			return _isPlaying || _updateOneTime;
		}
		
		public function addLabel(name : String, time : Number) : IAnimationController
		{
			if (_labelNames.indexOf(name) >= 0)
				throw new Error('A label with the same name already exists.');
			
			_labelNames.push(name);
			_labelTimes.push(time);
			
			updateNextLabel();
			
			return this;
		}
		
		protected function checkLabelHit(previousTime : int, newTime : int) : void
		{
			if (!_isPlaying || !_nextLabelIds || !_nextLabelIds.length || !_labelHit.numCallbacks)
				return;
			
			var nextLabelTime	: Number	= _labelTimes[_nextLabelIds[0]];
			var trigger			: Boolean	= false;
			
			if (newTime < previousTime)
			{
				if (previousTime < nextLabelTime && nextLabelTime <= _loopEndTime)
					trigger = true;
				else if (_loopBeginTime <= nextLabelTime && nextLabelTime <= newTime)
					trigger = true;
			}
			else
			{
				if ((previousTime < nextLabelTime && nextLabelTime <= newTime) || newTime == nextLabelTime)
					trigger = true;
			}
			if (trigger)
			{
				for each(var i : int in _nextLabelIds)
					triggerLabelHit(_labelNames[i], _labelTimes[i]);
					
				updateNextLabel((newTime + 1) % _loopEndTime);
			}
		}
		
		public function changeLabel(oldName : String, newName : String) : IAnimationController
		{
			var index : int = _labelNames.indexOf(oldName);
			
			if (index < 0)
				throw new Error('The time label named \'' + oldName + '\' does not exist.');
			
			_labelNames[index] = newName;
			
			return this;
		}
		
		public function setTimeForLabel(name : String, newTime : Number) : IAnimationController
		{
			var index : int = _labelNames.indexOf(name);
			
			if (index < 0)
				throw new Error('The time label named \'' + name + '\' does not exist.');
			
			_labelTimes[index] = newTime;
			
			updateNextLabel();
			
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
		
		minko_animation function triggerLabelHit(labelName : String, labelTime : int) : void
		{
			if (_labelHit.numCallbacks)
				_labelHit.execute(this, labelName, labelTime);
		}
		
		override protected function targetAddedHandler(ctrl:EnterFrameController, target:ISceneNode):void
		{
			super.targetAddedHandler(ctrl,target);
			invalidate(target);
		}
		
		override protected function targetRemovedHandler(ctrl:EnterFrameController, target:ISceneNode):void
		{
			super.targetRemovedHandler(ctrl,target);
			invalidate(target);
		}

		public function invalidate(target : Object = null) : void
		{
		}

	}
}