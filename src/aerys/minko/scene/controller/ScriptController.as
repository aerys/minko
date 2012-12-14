package aerys.minko.scene.controller
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.KeyboardManager;
	import aerys.minko.type.MouseManager;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;

	public class ScriptController extends EnterFrameController
	{
        private var _scene          : Scene;
        private var _started        : Dictionary;
		private var _lastTime		: Number;
		private var _deltaTime		: Number;
		private var _currentTarget	: ISceneNode;
		private var _viewport		: Viewport;
		
        protected function get scene() : Scene
        {
            return _scene;
        }
        
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
		}
        
        protected function initialize() : void
        {
            _started = new Dictionary(true);
        }
        
        override protected function targetAddedToSceneHandler(target    : ISceneNode,
                                                              scene     : Scene) : void
        {
            if (_scene && scene != _scene)
                throw new Error(
                    'The same ScriptController instance can not be used in more than one scene ' +
                    'at a time.'
                );
            
            _scene = scene;
        }
        
        override protected function targetRemovedFromSceneHandler(target    : ISceneNode,
                                                                  scene     : Scene) : void
        {
            if (numTargetsInScene == 0)
                _scene = null;
        }
        
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			_viewport = viewport;
			_deltaTime = _lastTime - time;
			_lastTime = time;
			
            beforeUpdate();
            
			var numTargets : uint = this.numTargets;
			
			for (var i : uint = 0; i < numTargets; ++i)
            {
                var target : ISceneNode = getTarget(i);
                
                if (target.scene)
                {
                    if (!_started[target])
                    {
                        _started[target] = true;
                        start(target);
                    }
                    
    				update(target);
                }
                else if (_started[target])
                {
                    _started[target] = false;
                    stop(target);
                }
            }
            
            afterUpdate();
		}
        
        /**
         * The 'start' method is called on a script target at the first frame occuring after it
         * has been added to the scene. 
         *  
         * @param target
         * 
         */
		protected function start(target : ISceneNode) : void
		{
			// nothing
		}
        
        /**
         * The 'beforeUpdate' method is called before each target is updated via the 'update'
         * method.
         * 
         */
        protected function beforeUpdate() : void
        {
            // nothing
        }
		
		protected function update(target : ISceneNode) : void
		{
			// nothing
		}
        
        /**
         * The 'afterUpdate' method is called after each target has been updated via the 'update'
         * method.
         * 
         */
        protected function afterUpdate() : void
        {
            // nothing
        }
		
        /**
         * The 'start' method is called on a script target at the first frame occuring after it
         * has been removed from the scene. 
         *  
         * @param target
         * 
         */
		protected function stop(target : ISceneNode) : void
		{
			// nothing
		}
	}
}