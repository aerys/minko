package aerys.minko.scene.controller.animation
{
	import flash.utils.getTimer;
	
	import aerys.minko.ns.minko_animation;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	
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
		
		minko_animation function get loopBeginTime():int
		{
			if (!_reverse)
				return _loopBeginTime;
			else
				return _loopEndTime;
		}
		
		minko_animation function get loopEndTime():int
		{
			if (!_reverse)
				return _loopEndTime;
			else
				return _loopBeginTime;
		}

		protected var _timeFunction				: Function;
		
		protected var _lastTime					: int;
		protected var _lastTimeUntransformed	: int;
		
		protected var _labelNames				: Vector.<String>;
		protected var _labelTimes				: Vector.<Number>;
		
		protected var _looped					: Signal;
		protected var _started					: Signal;
		protected var _stopped					: Signal;
		protected var _labelHit					: Signal;
		
		private var _nextLabelIds				: Array;
		
		protected var _reverse					: Boolean;
		
		public function get reverse():Boolean
		{
			return _reverse;
		}
		
		public function set reverse(value:Boolean):void
		{
			if (_reverse != value)
			{
				_lastTime = -_lastTime;
				_lastTimeUntransformed = -_lastTimeUntransformed;
			}
			
			_reverse = value;
			updateNextLabel(_currentTime);
		}
		
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
			if (value != null && _timeFunction == null)
			{
				_lastTime = value(_lastTime);
			}
			else if (_timeFunction != null)
			{
				var time		: int = _reverse ? -getTimer() : getTimer();
				var prevTime	: int = _timeFunction(time);
				var prevDeltaT	: int = prevTime - _lastTime;
				
				if (value != null)
					time = value(time);
				
				_lastTime = time - prevDeltaT;
			}
			
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
			
			_looping = loop;
			
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
			
			return this;
		}
		
		protected function updateNextLabelOld(currentTime : int = -1) : void
		{
			if (reverse)
				return updateNextLabelReverse(currentTime);
			
			_nextLabelIds = [];
			
			if (!_labelTimes.length)
				return;
			
			if (currentTime == -1)
				currentTime = _currentTime;
			
			var min : int = int.MAX_VALUE;
			
			for(var i : int = 0; i < _labelTimes.length; ++i)
			{
				var time : int = _labelTimes[i];
				
				if (time < loopBeginTime || time > loopEndTime)
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
				updateNextLabel(loopBeginTime);
			}
			else if (min == loopEndTime && _looping)
			{
				for(i = 0; i < _labelTimes.length; ++i)
				{
					if (_labelTimes[i] == loopBeginTime && _nextLabelIds.indexOf(i) == -1)
						_nextLabelIds.push(i);
				}
			}
		}
		
		protected function updateNextLabel(currentTime: int = -1) : void
		{
			_nextLabelIds = [];
			
			if (!_labelTimes.length)
				return;
			
			if (currentTime == -1)
				currentTime = _currentTime;
			
			var comp : int = _reverse ? int.MIN_VALUE : int.MAX_VALUE;
			
			for(var i : int = 0; i < _labelTimes.length; ++i)
			{
				var time : int = _labelTimes[i];
				
				if (time < _loopBeginTime || time > _loopEndTime)
					continue;
				
				if ((_reverse && time <= currentTime) || (!_reverse && time >= currentTime))
				{
					if ((_reverse && time > comp) || (!_reverse && time < comp))
					{
						comp = time;
						_nextLabelIds = [i];
					}
					else if (time == comp)
					{
						_nextLabelIds.push(i);
					}
				}
			}
			
			if (!_nextLabelIds.length)
			{
				updateNextLabel(loopBeginTime);
			}
			else if (comp == loopEndTime && _looping)
			{
				for(i = 0; i < _labelTimes.length; ++i)
				{
					if (_labelTimes[i] == loopBeginTime && _nextLabelIds.indexOf(i) == -1)
						_nextLabelIds.push(i);
				}
			}
		}
		
		private function updateNextLabelReverse(currentTime : int = -1) : void
		{
			_nextLabelIds = [];
			
			if (!_labelTimes.length)
				return;
			
			if (currentTime == -1)
				currentTime = _currentTime;
			
			var max : int = int.MIN_VALUE;
			
			for(var i : int = 0; i < _labelTimes.length; ++i)
			{
				var time : int = _labelTimes[i];
				
				if (time > loopBeginTime || time < loopEndTime)
					continue;
				
				if (time <= currentTime)
				{
					if (time > max)
					{
						max = time;
						_nextLabelIds = [i];
					}
					else if (time == max)
					{
						_nextLabelIds.push(i);
					}
				}
			}
			
			if (!_nextLabelIds.length)
			{
				updateNextLabel(loopBeginTime);
			}
			else if (max == loopEndTime && _looping)
			{
				for(i = 0; i < _labelTimes.length; ++i)
				{
					if (_labelTimes[i] == loopBeginTime && _nextLabelIds.indexOf(i) == -1)
						_nextLabelIds.push(i);
				}
			}
		}
		
		private function updateLastTime() : void
		{
			_lastTime = _reverse ? -getTimer() : getTimer();
			if (_timeFunction != null)
				_lastTime = _timeFunction(_lastTime);
		}
		
		public function play() : IAnimationController
		{
			_isPlaying = true;
			updateLastTime();
			_started.execute(this);
			
			checkLabelHit(_currentTime, _currentTime);
			
			return this;
		}
		
		public function stop() : IAnimationController
		{
			if (_isPlaying)
			{
				updateNextLabel(_currentTime);
				checkLabelHit(_currentTime, _currentTime);
			}
			
			_isPlaying = false;
			//_updateOneTime 	= true;
			updateLastTime();
			_stopped.execute(this);
			
			return this;
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : IAnimationController
		{			
			_loopBeginTime	= beginTime != null ? getAnimationTime(beginTime) : 0;
			_loopEndTime	= endTime != null ? getAnimationTime(endTime) : _totalTime;
			
			if (_loopBeginTime > _loopEndTime)
			{
				var begin : int = _loopEndTime;
				_loopEndTime	= _loopBeginTime;
				_loopBeginTime	= begin;
			}
			
			if (_currentTime < _loopBeginTime || _currentTime > _loopEndTime)
				_currentTime = loopBeginTime;
			
			updateNextLabel();
			
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
				if (_reverse)
					time = -time;
				if (_timeFunction != null)
					time = _timeFunction(time);
				
				var deltaT 			: Number = time - _lastTime;
				var lastCurrentTime : Number = _currentTime;
				
				if (_isPlaying)
				{
					_currentTime = loopBeginTime
						+ (_currentTime + deltaT - loopBeginTime)
						% (loopEndTime - loopBeginTime);
				}
				
				if ((deltaT > 0 && lastCurrentTime > _currentTime)
					|| (deltaT < 0 && (lastCurrentTime < _currentTime
						|| _currentTime * lastCurrentTime < 0)))
				{
					if (_looping)
						_looped.execute(this);
					else
					{
						if (_reverse)
							_currentTime = deltaT < 0 ? loopEndTime : 0;
						else
							_currentTime = deltaT > 0 ? loopEndTime : 0;
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
			
			if (!_reverse && newTime < previousTime)
			{
				if (previousTime < nextLabelTime && nextLabelTime <= loopEndTime)
					trigger = true;
				else if (loopBeginTime <= nextLabelTime && nextLabelTime <= newTime)
					trigger = true;
			}
			else if (_reverse && newTime > previousTime)
			{
				if (previousTime > nextLabelTime && nextLabelTime >= loopEndTime)
					trigger = true;
				else if (loopBeginTime >= nextLabelTime && nextLabelTime >= newTime)
					trigger = true;
			}
			else
			{
				if (_reverse && ((previousTime > nextLabelTime && nextLabelTime >= newTime) || newTime == nextLabelTime))
					trigger = true;
				else if (!_reverse && ((previousTime < nextLabelTime && nextLabelTime <= newTime) || newTime == nextLabelTime))
					trigger = true;
			}
			if (trigger)
			{
				for each(var i : int in _nextLabelIds)
				triggerLabelHit(_labelNames[i], _labelTimes[i]);
				
				if (_reverse)
					newTime--;
				else
					newTime++;
				
				updateNextLabel(newTime % _loopEndTime);
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
		
		override public function deepClone() : AbstractController
		{
			var clone : AbstractAnimationController = this.clone() as AbstractAnimationController;
			return clone.clonePlayingState(this);
		}
		
		minko_animation function clonePlayingState(from : AbstractAnimationController) : AbstractAnimationController
		{
			_isPlaying		= from._isPlaying;
			_loopBeginTime	= from._loopBeginTime;
			_loopEndTime	= from._loopEndTime;
			_currentTime	= from._currentTime;
			_lastTime		= from._lastTime;
			_previousTime	= from._previousTime;
			_looping		= from._looping;
			_nextLabelIds	= from._nextLabelIds.concat();
			_updateOneTime	= from._updateOneTime;
			return this;
		}
		
		public function updateNow() : IAnimationController
		{
			_updateOneTime = true;
			sceneEnterFrameHandler(null, null, null, getTimer());
			return this;
		}
	}
}