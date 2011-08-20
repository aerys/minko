package aerys.minko.type.animation
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;
	
	public class AbstractAnimation
	{
		protected var _id				: String;
		protected var _playingOn		: IScene;
		protected var _timelines		: Vector.<ITimeline>;
		protected var _duration			: uint;
		
		public function get id() 		: String 	{ return _id; }
		public function get duration() 	: uint 		{ return _duration; }
		
		public function AbstractAnimation(id		: String,
								 		  timelines : Vector.<ITimeline>)
		{
			_id			= id;
			_timelines	= timelines;
			_duration	= 0;
			
			for each (var timeline : ITimeline in timelines)
				if (_duration < timeline.duration)
					_duration = timeline.duration;
		}
		
		protected function transformNodes(time : uint) : void
		{
			var timelinesCount : uint = _timelines.length;
			for (var i : uint = 0; i < timelinesCount; ++i)
			{
				var timeline		: ITimeline	= _timelines[i];
				var timelineTarget	: String	= timeline.target;
				
				var target			: ITransformableScene; 
				if (_playingOn.name == timelineTarget)
					target = ITransformableScene(_playingOn);
				else if (_playingOn is Group)
					target = ITransformableScene(Group(_playingOn).getDescendantByName(timelineTarget));
				
				if (!target)
					continue;
				
				timeline.updateAt(time, target);
			}
		}
		
		public function playOn(node : IScene) : void
		{
			_playingOn	= node;
		}
		
		public function stop() : void
		{
			_playingOn = null;
		}
	}
}
