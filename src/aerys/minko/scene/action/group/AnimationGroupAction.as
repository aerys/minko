package aerys.minko.scene.action.group
{
	import aerys.minko.ns.minko_animation;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.AnimationGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.animation.timeline.ITimeline;


	public final class AnimationGroupAction implements IAction
	{
		use namespace minko_animation;
	
		public static const animationGroupAction : AnimationGroupAction = new AnimationGroupAction();

		private static const TYPE : uint = ActionType.UPDATE_TRANSFORM_DATA; // more or less

		private var _loop			: Boolean	= true;

		public function get type() : uint
		{
			return TYPE;
		}

		public function get loop() : Boolean
		{
			return _loop;
		}

		public function set loop(value : Boolean) : void
		{
			_loop = value;
		}

		public function run(scene		: IScene,
							visitor		: ISceneVisitor,
							renderer	: IRenderer) : Boolean
		{
			var animationGroup	: AnimationGroup 	= AnimationGroup(scene);
			var timer 			: uint 				= animationGroup.timeFunction();
			
			if (animationGroup._isPlaying)
			{
				var previousTime	: uint	= animationGroup._currentTime;

				animationGroup._currentTime = animationGroup._loopBeginTime + (
					animationGroup._currentTime + timer
					- animationGroup._lastTimerTick - animationGroup._loopBeginTime) %
					(animationGroup._loopEndTime - animationGroup._loopBeginTime);

				if (animationGroup._onComplete != null && previousTime > animationGroup._currentTime)
				{
					animationGroup._currentTime = previousTime;
					animationGroup._isPlaying = false;
					animationGroup._onComplete(animationGroup);

					return true;
				}
			}

			animationGroup._lastTimerTick	= timer;

			var timelines		: Vector.<ITimeline> 	= animationGroup._timelines;
			var timelinesCount	: uint					= timelines.length;

			for (var i : uint = 0; i < timelinesCount; ++i)
			{
				var timeline	: ITimeline	= timelines[i];
				var target		: IScene	= animationGroup._targets[i];

				if (target != null)
					timeline.updateAt(animationGroup._currentTime, target);
			}

			return true;
		}
	}
}
