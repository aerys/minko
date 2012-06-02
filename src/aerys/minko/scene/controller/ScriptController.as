package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	
	import flash.display.BitmapData;

	public class ScriptController extends EnterFrameController
	{
		private var _lastTime		: Number		= 0.0;
		private var _deltaTime		: Number		= 0.0;
		private var _currentTarget	: ISceneNode	= null;
		
		protected function get deltaTime() : Number
		{
			return _deltaTime;
		}
		
		public function ScriptController()
		{
			super();
			
			initialize();
		}
		
		private function initialize() : void
		{
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
				start(getTarget(i));
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			_deltaTime = _lastTime - time;
			_lastTime = time;
			
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
				update(getTarget(i));
		}
		
		protected function start(target : ISceneNode) : void
		{
			// nothing
		}
		
		protected function update(target : ISceneNode) : void
		{
			// nothing
		}
	}
}