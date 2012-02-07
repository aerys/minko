package aerys.minko.type.controller
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.animation.TimeLabel;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;

	public class AnimationController implements IController
	{
		public static const DEFAULT_TIME_FUNCTION	: Function = getTimer;
		
		private var _enabled		: Boolean				= true;
		private var _timelines		: Vector.<ITimeline>	= null;
		private var _targets		: Vector.<ISceneNode>		= null;
		private var _isPlaying		: Boolean				= false;
		private var _loopBeginTime	: uint					= 0;
		private var _loopEndTime	: uint					= 0;
		private var _currentTime	: uint					= 0;
		
		private var _totalTime		: uint					= 0;
		private var _timeFunction	: Function				= DEFAULT_TIME_FUNCTION;
		private var _labels			: Vector.<TimeLabel>	= null;
		
		public function get targets() : Vector.<ISceneNode>
		{
			return _targets;
		}
		
		public function AnimationController(timelines	: Vector.<ITimeline>,
											targets		: Vector.<ISceneNode>)
		{
			initialize(timelines, targets);
		}
		
		private function initialize(timelines	: Vector.<ITimeline>,
									targets		: Vector.<ISceneNode>) : void
		{
			_timelines = timelines;
			_targets = targets;
			
			var numTimelines : uint = _timelines.length;
			for (var timelineId : uint = 0; timelineId < numTimelines; ++timelineId)
				if (_totalTime < _timelines[timelineId].duration)
					_totalTime = _timelines[timelineId].duration;
			
			setPlaybackWindow(0, _totalTime);
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
		}
		
		public function stop() : void
		{
			_isPlaying = false;
		}
		
		public function setPlaybackWindow(beginTime	: Object = null,
										  endTime	: Object = null) : void
		{
			_loopBeginTime	= beginTime != null ? getTime(beginTime) : 0;
			_loopEndTime	= endTime != null ? getTime(endTime) : _totalTime;
			
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
		
		public function step(deltaT : uint) : void
		{
			if (!_enabled)
				return ;
			
			if (_isPlaying)
			{
				_currentTime = _loopBeginTime
							   + (_currentTime + deltaT - _loopBeginTime)
							   	  %	(_loopEndTime - _loopBeginTime);
			}
			
			var timelines		: Vector.<ITimeline> 	= _timelines;
			var timelinesCount	: uint					= timelines.length;
			
			for (var i : uint = 0; i < timelinesCount; ++i)
			{
				var timeline	: ITimeline	= timelines[i];
				var target		: ISceneNode	= _targets[i];
				
				if (target != null)
					timeline.updateAt(_currentTime, target);
			}
		}
	}
}