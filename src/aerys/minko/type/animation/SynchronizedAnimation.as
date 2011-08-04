package aerys.minko.type.animation
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;
	
	public class SynchronizedAnimation extends AbstractAnimation
	{
		private var _beginTime		: uint;
		
		public function SynchronizedAnimation(id		: String, 
											  timelines	: Vector.<ITimeline>)
		{
			super(id, timelines);
		}
		
		override public function playOn(node : IScene) : void
		{
			super.playOn(node);
			_beginTime	= getTimer();
		}
		
		public function tick() : void
		{
			var time : uint = (getTimer() - _beginTime) % _duration;
			transformNodes(time);
		}
	}
}
