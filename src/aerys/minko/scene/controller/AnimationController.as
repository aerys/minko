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
		
		private var _timeline		: ITimeline				= null;
		private var _isPlaying		: Boolean				= false;
		private var _loopBeginTime	: uint					= 0;
		private var _loopEndTime	: uint					= 0;
		private var _currentTime	: uint					= 0;
		
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

		public function AnimationController(timeline	: ITimeline)
		{
			initialize(timeline);
		}
		
		private function initialize(timeline	: ITimeline) : void
		{
			_timeline = timeline;
			
			setPlaybackWindow(0, timeline.duration);
			gotoAndPlay(0);
		}
		
		public function gotoAndPlay(time : Object) : void
		{
			var timeValue : uint = getTime(time);
			
			if (timeValue < _loopBeginTime || timeValue > _loopEndTime)
				throw new Error('Time value is outside of playback window. To reset playback window, call resetPlaybackWindow.');
			
			_currentTime = timeValue;
			play();
		}
		
		public function gotoAndStop(time : Object) : void
		{
			var timeValue : uint = getTime(time);
			
			if (timeValue < _loopBeginTime || timeValue > _loopEndTime)
				throw new Error('Time value is outside of playback window. To reset playback window, call resetPlaybackWindow.');
			
			_currentTime = timeValue;
			stop();
		}
		
		public function play() : void
		{
			_isPlaying		= true;
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
			_loopBeginTime	= beginTime != null ? getTime(beginTime) : 0;
			_loopEndTime	= endTime != null ? getTime(endTime) : _timeline.duration;
			
			if (_currentTime < _loopBeginTime || _currentTime > _loopEndTime)
				_currentTime = _loopBeginTime;
		}
		
		public function resetPlaybackWindow() : void
		{
			setPlaybackWindow();
		}
		
		private function getTime(time : Object) : uint
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
				var deltaT : Number = time - _lastTime;
				var lastCurrentTime : Number = _currentTime;
				
				_currentTime = _loopBeginTime
					+ (_currentTime + deltaT - _loopBeginTime)
					%	(_loopEndTime - _loopBeginTime);
				
				if (lastCurrentTime > _currentTime)
					_looped.execute(this);
			}				
			
			_lastTime = time;
			
			return _isPlaying;
		}
		
		override protected function updateTarget(target:Group):void
		{
			_timeline.updateAt(_currentTime, target);
		}
	}
}