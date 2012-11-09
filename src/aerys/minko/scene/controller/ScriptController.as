package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.KeyboardManager;
	import aerys.minko.type.MouseManager;
	
	import flash.display.BitmapData;

	public class ScriptController extends EnterFrameController
	{
		private var _lastTime		: Number;
		private var _deltaTime		: Number;
		private var _currentTarget	: ISceneNode;
		private var _viewport		: Viewport;
		
		protected function get deltaTime() : Number
		{
			return _deltaTime;
		}
		
		protected function get keyboard() : KeyboardManager
		{
			return _viewport.keyboardManager;
		}
		
		protected function get mouse() : MouseManager
		{
			return _viewport.mouseManager;
		}
		
		protected function get viewport() : Viewport
		{
			return _viewport;
		}
		
		public function ScriptController(targetType	: Class = null)
		{
			super(targetType);
			
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
				initialize(getTarget(i));
		}
        
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			_viewport = viewport;
			_deltaTime = _lastTime - time;
			_lastTime = time;
			
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
            {
                var target : ISceneNode = getTarget(i);
                
                if (target.scene)
    				update(target);
            }
		}
        
        override protected function targetAddedToSceneHandler(target    : ISceneNode,
                                                              scene     : Scene) : void
        {
            super.targetAddedToSceneHandler(target, scene);
            
            var numTargets : uint = this.numTargets;
            
            for (var i : uint = 0; i < numTargets; ++i)
                start(getTarget(i));
        }
		
		override protected function targetRemovedFromSceneHandler(target	: ISceneNode,
																  scene		: Scene) : void
		{
			super.targetRemovedFromSceneHandler(target, scene);
			
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
				stop(getTarget(i));
		}
		
        protected function initialize(target : ISceneNode) : void
        {
            // nothing
        }
        
		protected function start(target : ISceneNode) : void
		{
			// nothing
		}
		
		protected function update(target : ISceneNode) : void
		{
			// nothing
		}
		
		protected function stop(target : ISceneNode) : void
		{
			// nothing
		}
	}
}