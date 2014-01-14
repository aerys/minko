package aerys.minko.scene.controller.animation
{
	import flash.display.BitmapData;
	
	import aerys.minko.ns.minko_animation;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	use namespace minko_animation;
	
	/**
	 * The AnimationController uses timelines to animate properties of scene nodes.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AnimationController extends AbstractAnimationController
	{
		private var _timelines		: Vector.<ITimeline>;
		
		private var _master			: MasterAnimationController;
		
		private var _isTimelineUpdateLocked	: Vector.<Boolean>;
		
		public function get numTimelines() : uint
		{
			return _timelines.length;
		}
		
		public function get master() : MasterAnimationController
		{
			return _master;
		}
		
		minko_animation function setMaster(v : MasterAnimationController) : void
		{
			_master = v;
		}
		
		public function AnimationController(timelines 	: Vector.<ITimeline>,
											loop		: Boolean	= true)
		{
			super(loop);
			
			initialize(timelines);
		}
		
		private function initialize(timelines 	: Vector.<ITimeline>) : void
		{
			_timelines = timelines.concat();
			
			var numTimelines : uint = _timelines.length;
			
			_isTimelineUpdateLocked	= new Vector.<Boolean>(numTimelines, true);
			for (var timelineId:uint = 0; timelineId < numTimelines; ++timelineId)
			{
				if (_totalTime < _timelines[timelineId].duration)
					_totalTime = _timelines[timelineId].duration;
				
				_isTimelineUpdateLocked[timelineId] = false;				
			}
			
			setPlaybackWindow(0, _totalTime);
			seek(0).play();
		}
		
		public function lockTimelineUpdate(propertyPath	: String)	: void
		{
			changeTimelineUpdateFlag(propertyPath, true);
		}
		
		public function unlockTimelineUpdate(propertyPath	: String)	: void
		{
			changeTimelineUpdateFlag(propertyPath, false);
		}
		
		private function changeTimelineUpdateFlag(propertyPath	: String,
												  isLocked		: Boolean)	: void
		{
			var numTimelines	: uint = _timelines.length;
			for (var timelineId:uint = 0; timelineId < numTimelines; ++timelineId)
				if (_timelines[timelineId].propertyPath == propertyPath)
					_isTimelineUpdateLocked[timelineId] = isLocked ? 1 : 0;
		}
		
		override public function clone() : AbstractController
		{
			var clone : AnimationController = new AnimationController(_timelines, _looping);
			clone._isTimelineUpdateLocked	= _isTimelineUpdateLocked.concat();
			
			clone._labelNames	= _labelNames.concat();
			clone._labelTimes	= _labelTimes.concat();
			
			return clone;
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
		
		override protected function sceneEnterFrameHandler(scene	: Scene,
														   viewport	: Viewport,
														   target	: BitmapData,
														   time		: Number) : void
		{
			if (_master)
				return;
			
			if (updateOnTime(time))
				update(time, scene);
		}
		
		override protected function targetAddedToScene(target:ISceneNode, scene:Scene):void
		{
			super.targetAddedToScene(target, scene);
			update(_lastTime, scene);
		}
		
		minko_animation function update(time		: Number,
										scene		: Scene = null) : void
		{
			_updateOneTime = false;
			
			for (var j : uint = 0; j < numTargets; ++j)
			{
				var ctrlTarget		: ISceneNode	= getTarget(j);
				
				if (scene && ctrlTarget.root != scene)
					continue;
				
				var numTimelines 	: int 			= _timelines.length;
				var group			: Group			= ctrlTarget as Group;
				
				
				for (var i : int = 0; i < numTimelines; ++i)
					if (!_isTimelineUpdateLocked[i])
					{
						var timeline : ITimeline = _timelines[i] as ITimeline;
						
						timeline.updateAt(
							_currentTime % (timeline.duration + 1),
							ctrlTarget
						);
					}
			}
			
			checkLabelHit(_previousTime, _currentTime);
		}
		
		override protected function updateNextLabel(currentTime:int=-1):void
		{
			if (_master)
				return;
			
			super.updateNextLabel(currentTime);
		}
		
		override protected function checkLabelHit(previousTime:int, newTime:int):void
		{
			if (_master)
				return;
			
			super.checkLabelHit(previousTime, newTime);
		}
		
		override public function invalidate(target : Object = null) : void
		{
			for each(var timeline : ITimeline in _timelines)
				timeline.invalidate(target);
		}
	}
}