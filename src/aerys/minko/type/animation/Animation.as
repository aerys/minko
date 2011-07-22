package aerys.minko.type.animation
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformable;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;
	
	import org.osmf.elements.DurationElement;

	public class Animation
	{
		private var _id				: String;
		
		private var _beginTime		: uint;
		private var _lastTime		: uint;
		
		private var _playingOn		: IScene;
		private var _timelines		: Vector.<ITimeline>;
		private var _duration		: uint;
		
		public function get id() : String { return _id; }
		
		public function Animation(id		: String,
								  timelines : Vector.<ITimeline>)
		{
			_id			= id;
			_timelines	= timelines;
			_lastTime	= 0;
			
			_duration	= 0;
			for each (var timeline : ITimeline in timelines)
				if (_duration < timeline.duration)
					_duration = timeline.duration;
		}
		
		public function tick() : void
		{
			var time : uint = (getTimer() - _beginTime) % _duration;
			transformNodes(time);
		}
		
		public function step() : void
		{
			var time : uint = (_lastTime + 2500 / 30) % _duration;
			transformNodes(time);
		}
		
		public function stepReverse() : void
		{
			var time : int = (_lastTime - 2500 / 30) % _duration;
			if (time < 0)
				time = time + _duration;
			
			transformNodes(time);
				
		}
		
		private function transformNodes(time : uint) : void
		{
			_lastTime = time;
			
			var timelinesCount : uint = _timelines.length;
			for (var i : uint = 0; i < timelinesCount; ++i)
			{
				var timeline		: ITimeline	= _timelines[i];
				var timelineTarget	: String	= timeline.target;
				
				var target			: ITransformable; 
				if (_playingOn.name == timelineTarget)
					target = ITransformable(_playingOn);
				else if (_playingOn is Group)
					target = ITransformable(Group(_playingOn).getDescendantByName(timelineTarget));
				
				if (!target)
					continue;
				
				timeline.setMatrixAt(time, target.transform);
			}
		}
		
		public function playOn(node : IScene) : void
		{
			_beginTime	= getTimer();
			_playingOn	= node;
			_lastTime	= 0;
		}
		
		public function stop() : void
		{
			_playingOn = null;
		}
		
	}
}
