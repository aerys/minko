package aerys.minko.scene.controller
{
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
	
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.KeyboardManager;
	import aerys.minko.type.MouseManager;

    use namespace minko_scene;

	public class AbstractScriptController extends EnterFrameController
	{
        private var _scene              : Scene;
        private var _started            : Dictionary;
		private var _time				: Number;
		private var _lastTime		    : Number;
		private var _deltaTime		    : Number;
		private var _currentTarget	    : ISceneNode;
		private var _viewport		    : Viewport;

        private var _targetsListLocked  : Boolean;
        private var _targetsToAdd       : Vector.<ISceneNode>;
        private var _targetsToRemove    : Vector.<ISceneNode>;

		private var _updateRate			: Number;

        protected function get scene() : Scene
        {
            return _scene;
        }

		protected function get time() : Number
		{
			return _time;
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

		protected function get updateRate() : Number
		{
			return _updateRate;
		}
		protected function set updateRate(value : Number) : void
		{
			_updateRate = value;
		}

		public function AbstractScriptController(targetType	: Class = null)
		{
			super(targetType);

            initialize();
		}

        protected function initialize() : void
        {
            _started = new Dictionary(true);
        }

		override protected function targetAddedToScene(target	: ISceneNode,
													   scene	: Scene) : void
		{
            super.targetAddedToScene(target, scene);

            if (_scene && scene != _scene)
                throw new Error(
                    'The same ScriptController instance can not be used in more than one scene ' +
                    'at a time.'
                );

			if (!_started[target])
			{
				_started[target] = true;
				start(target);
			}

			_scene = scene;
        }

		override protected function targetRemovedFromScene(target 	: ISceneNode,
														   scene 	: Scene) : void
        {
            super.targetRemovedFromScene(target, scene);

			if (getNumTargetsInScene(scene))
				_scene = null;

			if (_started[target])
			{
				_started[target] = false;
				stop(target);
			}
        }

		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			_deltaTime = time - _lastTime;
			_time = time;

			if (_updateRate != 0. && deltaTime < 1000. / _updateRate)
				return;

			_viewport = viewport;

            lockTargetsList();
            beforeUpdate();

			var numTargets : uint = this.numTargets;

			for (var i : uint = 0; i < numTargets; ++i)
            {
                var target : ISceneNode = getTarget(i);

                if (target.scene)
    				update(target);
            }

            afterUpdate();
            unlockTargetsList();

			_lastTime = time;
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

        private function lockTargetsList() : void
        {
            _targetsListLocked = true;
        }

        private function unlockTargetsList() : void
        {
            _targetsListLocked = false;

            if (_targetsToAdd)
            {
                var numTargetsToAdd : uint = _targetsToAdd.length;
                for (var i : uint = 0; i < numTargetsToAdd; ++i)
                    addTarget(_targetsToAdd[i]);
                _targetsToAdd = null;
            }

            if (_targetsToRemove)
            {
                var numTargetsToRemove : uint = _targetsToRemove.length;
                for (var j : uint = 0; j < numTargetsToRemove; ++j)
                    removeTarget(_targetsToRemove[j]);
                _targetsToRemove = null;
            }
        }

        override minko_scene function addTarget(target : ISceneNode) : void
        {
            if (_targetsListLocked)
            {
                if (_targetsToAdd)
                    _targetsToAdd.push(target);
                else
                    _targetsToAdd = new <ISceneNode>[target];
            }
            else
                super.addTarget(target);
        }

        override minko_scene function removeTarget(target : ISceneNode) : void
        {
            if (_targetsListLocked)
            {
                if (_targetsToRemove)
                    _targetsToRemove.push(target);
                else
                    _targetsToRemove = new <ISceneNode>[target];
            }
            else
                super.removeTarget(target);
        }
	}
}