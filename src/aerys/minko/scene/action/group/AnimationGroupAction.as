package aerys.minko.scene.action.group
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.AnimationGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.animation.timeline.ITimeline;
	
	import flash.utils.getTimer;
	
	use namespace minko;
	
	public class AnimationGroupAction implements IAction
	{
		public static const animationGroupAction : AnimationGroupAction = new AnimationGroupAction();
		
		private static const TYPE : uint = ActionType.UPDATE_LOCAL_DATA; // more or less
		
		public function get type():uint
		{
			return TYPE;
		}
		
		public function run(scene		: IScene,
							visitor		: ISceneVisitor,
							renderer	: IRenderer) : Boolean
		{
			var animationGroup	: AnimationGroup = AnimationGroup(scene);
			
			var timer : uint = getTimer();
			if (animationGroup._isPlaying)
			{
				animationGroup._currentTime = animationGroup._loopBeginTime + (
					animationGroup._currentTime + timer
					- animationGroup._lastTimerTick - animationGroup._loopBeginTime) %
					(animationGroup._loopEndTime - animationGroup._loopBeginTime);
			}
			
			animationGroup._lastTimerTick	= timer;
			
			var timelines		: Vector.<ITimeline> 	= animationGroup._timelines;
			var timelinesCount	: uint					= timelines.length;
			
			for (var i : uint = 0; i < timelinesCount; ++i)
			{
				var timeline	: ITimeline	= timelines[i];
				var targetName	: String	= timeline.targetName;
				var target		: IScene	= animationGroup.getDescendantByName(targetName);
				
				if (target != null)
					timeline.updateAt(animationGroup._currentTime, target);
			}
			
			return true;
		}
	}
}
