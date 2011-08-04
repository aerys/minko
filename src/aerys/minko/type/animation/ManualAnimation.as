package aerys.minko.type.animation
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	public class ManualAnimation extends AbstractAnimation
	{
		private var _lastTime		: uint;
		
		public function ManualAnimation(id			: String, 
										timelines	: Vector.<ITimeline>)
		{
			super(id, timelines);
			
			_lastTime = 0;
		}
		
		override public function playOn(node : IScene) : void
		{
			_lastTime = 0;
			
			super.playOn(node);
		}
		
		public function seek(time : int = 0) : void
		{
			_lastTime = time % _duration;
			transformNodes(time);
		}
		
		public function step(deltaTime : int = 80) : void
		{
			stepSegment(0, _duration, deltaTime);
		}
		
		public function stepSegment(segmentBegin	: uint, 
									segmentEnd		: uint,
									deltaTime		: int = 80) : void
		{
			if (segmentBegin > segmentEnd)
				throw new Error();
			
			var time : int = (_lastTime + deltaTime - segmentBegin)
			if (time < 0)
				while (time < 0)
					time += segmentEnd - segmentBegin;
			else
				time %= (segmentEnd - segmentBegin);
			
			time += segmentBegin;
			
			_lastTime = time;
			transformNodes(time);
		}
		
	}
}