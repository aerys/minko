package aerys.minko.scene.node.group
{
	import aerys.minko.ns.minko;
	import aerys.minko.scene.action.group.AnimationGroupAction;
	import aerys.minko.type.animation.TimeLabel;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;

	use namespace minko;
	
	public class AnimationGroup extends Group
	{
		public static var TIMER_FEED			: Function = getTimer;
		
		
		minko var _timelines					: Vector.<ITimeline>;
		
		minko var _isPlaying					: Boolean;
		
		minko var _loopBeginTime				: uint;
		minko var _loopEndTime					: uint;
			
		minko var _lastTimerTick				: uint;
		minko var _currentTime					: uint;
		
		minko var _onComplete					: Function = null;
		
		private var _totalTime					: uint;
		
		private var _labels						: Vector.<TimeLabel>;
		
		public function get isPlaying() : Boolean
		{
			return _isPlaying;
		}
		
		/**
		 * @return Specifies the number of the frame in which the playhead is located in the timeline of the AnimationGroup instance.
		 */		
		public function get currentTime() : int
		{
			return _currentTime;
		}
		
		/**
		 * @return The current label in which the playhead is located in the timeline of the AnimationGroup instance.
		 */		
		public function get currentLabel() : String
		{
			if (_labels.length == 0)
			{	
				return null;
			}
			else
			{
				var labelCount 		: uint = _labels.length;
				var bottomLabelId	: uint = 0;
				var upperLabelId	: uint = labelCount;
				while (upperLabelId - bottomLabelId > 1)
				{
					var labelId : uint = (bottomLabelId + upperLabelId) >> 1;
					if (_labels[labelId].time > _currentTime)
						upperLabelId = labelId;
					else
						bottomLabelId = labelId;
				}
				
				return _labels[bottomLabelId].name;
			}
		}
			
		/**
		 * @return Returns an array of TimeLabel objects from the current scene.
		 * A copy is made: modifying this vector will have no effect
		 */
		public function get labels() : Vector.<TimeLabel>
		{
			return _labels;//.slice();
		}
		
		public function set labels(v : Vector.<TimeLabel>) : void
		{
			_labels = v//.slice();
			_labels = _labels.sort(TimeLabel.sort);
		}
		
		/**
		 * @return The total number of frames in the AnimationGroup instance.
		 */
		public function get totalTime() : int
		{
			return _totalTime;
		}	
		
		public function AnimationGroup(timelines	: Vector.<ITimeline>,
									   labels		: Vector.<TimeLabel> = null,
									   ...children)
		{
			super(children);
			
			_timelines	= timelines;
			_labels		= labels || new Vector.<TimeLabel>();
			
			var timelineCount : uint = _timelines.length;
			for (var timelineId : uint = 0; timelineId < timelineCount; ++timelineId)
				if (_totalTime < _timelines[timelineId].duration)
					_totalTime = _timelines[timelineId].duration;
			
			actions.unshift(AnimationGroupAction.animationGroupAction);

			setPlaybackWindow(0, _totalTime);
			gotoAndPlay(0);
		}
		
		public function onComplete(f : Function) : void
		{
			_onComplete = f;
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
			_lastTimerTick	= TIMER_FEED();
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
		
		public function clone() : AnimationGroup
		{
			var clone		: AnimationGroup = new AnimationGroup(_timelines, _labels);
			var numChildren	: uint = this.numChildren;
			
			for (var childId : uint = 0; childId < numChildren; ++childId)
				clone.addChild(getChildAt(childId));
			
			return clone;
		}
	}
}