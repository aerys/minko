package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	import aerys.minko.type.animation.TimeLabel;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.display.BitmapData;
	import flash.utils.getTimer;

	/**
	 * The AnimationController uses timelines to animate properties of scene nodes.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AnimationController extends EnterFrameController
	{
		private var _timelines		: Vector.<ITimeline>	= new Vector.<ITimeline>();
		private var _isPlaying		: Boolean				= false;
		private var _updateOneTime	: Boolean 				= false;
		
		private var _loopBeginTime	: int					= 0;
		private var _loopEndTime	: int					= 0;
		private var _looping		: Boolean				= true;
		private var _currentTime	: int					= 0;
		private var _totalTime		: int					= 0;
		
		private var _timeFunction	: Function				= null;
		private var _labels			: Vector.<TimeLabel>	= new Vector.<TimeLabel>();
		
		private var _lastTime		: Number				= 0;
		
		private var _looped			: Signal 				= new Signal('AnimationController.looped');
		private var _started		: Signal				= new Signal('AnimationController.started');
		private var _stopped		: Signal				= new Signal('AnimationController.stopped');

		public function get timeFunction():Function
		{
			return _timeFunction;
		}

		public function set timeFunction(value:Function):void
		{
			_timeFunction = value;
		}

		public function get labels():Vector.<TimeLabel>
		{
			return _labels;
		}

		public function get numTimelines() : uint
		{
			return _timelines.length;
		}
		
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
		
		public function get isPlaying():Boolean
		{
			return _isPlaying;
		}
		
		public function set isPlaying(value:Boolean):void
		{
			_isPlaying = value;
		}
		
		public function get currentTime():int
		{
			return _currentTime;
		}
		
		public function set currentTime(value:int):void
		{
			_currentTime = value;
		}

		public function AnimationController(timelines 	: Vector.<ITimeline>,
											loop		: Boolean	= true)
		{
			super();
			
			_timelines = timelines;
			_looping = loop;
			
			initialize();
		}
		
		override public function clone() : AbstractController
		{
			return new AnimationController(_timelines.slice());
		}
		
		public function cloneTimelines() : void
		{
			var numTimelines	: uint	= _timelines.length;
			
			for (var timelineId : uint = 0; timelineId < numTimelines; ++timelineId)
				_timelines[timelineId] = (_timelines[timelineId] as ITimeline).clone();
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
			play();
		}
		
		public function gotoAndStop(time : Object) : void
		{
			gotoAndPlay(time);
			stop();
			
			// force refresh for next frame
			_updateOneTime = true;
		}
		
		public function play() : void
		{
			_isPlaying = true;
			_lastTime = _timeFunction != null ? _timeFunction(getTimer()) : getTimer();
			_started.execute(this);
		}
		
		public function stop() : void
		{
			if (_isPlaying)
			{
				_isPlaying = false;
				_updateOneTime 	= true;
				_lastTime = _timeFunction != null ? _timeFunction(getTimer()) : getTimer();
				_stopped.execute(this);
			}
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
				throw new Error('Invalid argument type: time must be Number or String');
			}
			
			return timeValue;
		}
		
		override protected function sceneEnterFrameHandler(scene	: Scene,
														   viewport	: Viewport,
														   target	: BitmapData,
														   time		: Number) : void
		{
			if (updateOnTime(time))
			{
				_updateOneTime = false;
				
				for (var j : uint = 0; j < numTargets; ++j)
				{
					var ctrlTarget		: ISceneNode	= getTarget(j);
					var numTimelines 	: int 			= _timelines.length;
					var group			: Group			= target as Group;
					
					if (ctrlTarget.root != scene)
						continue ;
					
					for (var i : int = 0; i < numTimelines; ++i)
					{
						var timeline : ITimeline = _timelines[i] as ITimeline;
						
						timeline.updateAt(
							_currentTime % (timeline.duration + 1),
							ctrlTarget
						);
					}
				}
			}
		}
		
		private function updateOnTime(time : Number) : Boolean
		{
			if (_isPlaying || _updateOneTime)
			{
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
					|| (deltaT < 0 && (lastCurrentTime < _currentTime || _currentTime * lastCurrentTime < 0)))
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
	}
}