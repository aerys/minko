package aerys.minko.scene.controller
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.animation.TimeLabel;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;

	public class AnimationController extends AbstractController
	{
		public static const DEFAULT_TIME_FUNCTION	: Function = getTimer;
		
		private var _timelines		: Vector.<ITimeline>	= new Vector.<ITimeline>();
		private var _isPlaying		: Boolean				= false;
		private var _loopBeginTime	: int					= 0;
		private var _loopEndTime	: int					= 0;
		private var _looping		: Boolean				= true;
		private var _currentTime	: int					= 0;
		private var _totalTime		: int					= 0;
		
		private var _timeFunction	: Function				= DEFAULT_TIME_FUNCTION;
		private var _labels			: Vector.<TimeLabel>	= null;
		
		private var _lastTime		: Number				= 0;
		
		private var _looped			: Signal 				= new Signal();
		private var _started		: Signal				= new Signal();
		private var _stopped		: Signal				= new Signal();

		public function get started():Signal
		{
			return _started;
		}
		
		public function get stopped() : Signal
		{
			return _stopped;
		}

		public function get looped():Signal
		{
			return _looped;
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

		public function AnimationController(timelines	: Vector.<ITimeline>)
		{
			super();
			
			_timelines = timelines;
			
			initialize();
		}
		
		public function getTimeline(index : uint = 0) : ITimeline
		{
			return _timelines[index];
		}
		
		private function initialize() : void
		{
			var numTimelines : uint = _timelines.length;
			
			for (var timelineId : uint = 0; timelineId < numTimelines; ++timelineId)
				if (_totalTime < _timelines[timelineId].duration)
					_totalTime = _timelines[timelineId].duration;
			
			setPlaybackWindow(0, _totalTime);
			gotoAndPlay(0);
		}
		
		public function gotoAndPlay(time : Object) : void
		{
			var timeValue : uint = getAnimationTime(time);
			
			if (timeValue < _loopBeginTime || timeValue > _loopEndTime)
				throw new Error('Time value is outside of playback window. To reset playback window, call resetPlaybackWindow.');
			
			_currentTime = timeValue;
			_lastTime = _timeFunction != null ? _timeFunction() : getTimer();
			play();
		}
		
		public function gotoAndStop(time : Object) : void
		{
			var timeValue : uint = getAnimationTime(time);
			
			if (timeValue < _loopBeginTime || timeValue > _loopEndTime)
				throw new Error('Time value is outside of playback window. To reset playback window, call resetPlaybackWindow.');
			
			_currentTime = timeValue;
			_lastTime = _timeFunction != null ? _timeFunction() : getTimer();
			stop();
		}
		
		public function play() : void
		{
			_isPlaying = true;
			_started.execute(this);
		}
		
		public function stop() : void
		{
			_isPlaying = false;
			_stopped.execute(this);
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : void
		{
			_loopBeginTime	= beginTime != null ? getAnimationTime(beginTime) : 0;
			_loopEndTime	= endTime != null ? getAnimationTime(endTime) : _totalTime;
			
			if (_currentTime < _loopBeginTime || _currentTime > _loopEndTime)
				_currentTime = _loopBeginTime;
		}
		
		public function resetPlaybackWindow() : void
		{
			setPlaybackWindow();
		}
		
		private function getAnimationTime(time : Object) : uint
		{
			var timeValue : uint;
			
			if (time is uint || time is int || time is Number)
			{
				timeValue = uint(time);
			}
			else if (time is String)
			{
				var labelCount : uint = _labels.length;
				
				for (var labelId : uint = 0; labelId < labelCount; ++labelId)
					if (_labels[labelId].name == time)
						timeValue = _labels[labelId].time;
			}
			else
			{
				throw new Error('Invalid argument type. Must be uint or String');
			}
			
			return timeValue;
		}
		
		override protected function updateOnTime(time : Number) : Boolean
		{
			if (_isPlaying)
			{
				if (_timeFunction != null)
					time = _timeFunction();
				
				var deltaT 			: Number = time - _lastTime;
				var lastCurrentTime : Number = _currentTime;
				
				_currentTime = _loopBeginTime
					+ (_currentTime + deltaT - _loopBeginTime)
					% (_loopEndTime - _loopBeginTime);
				
				if (lastCurrentTime > _currentTime)
				{
					if (_looping)
						_looped.execute(this);
					else
					{
						_currentTime = _totalTime;
						stop();
						
						return true;
					}
				}
			}				
			
			_lastTime = time;
			
			return _isPlaying;
		}
		
		override protected function updateTarget(target : ISceneNode) : void
		{
			var numTimelines 	: int 	= _timelines.length;
			var group			: Group	= target as Group;
			
			for (var i : int = 0; i < numTimelines; ++i)
			{
				var timeline : ITimeline = _timelines[i] as ITimeline;
				
				timeline.updateAt(
					_currentTime % (timeline.duration + 1),
					target
				);
			}
		}
	}
}