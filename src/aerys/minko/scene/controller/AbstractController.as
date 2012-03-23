package aerys.minko.scene.controller
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	
	import flash.utils.getQualifiedClassName;

	/**
	 * Controllers work on scene nodes to modify and update them. They offer the best
	 * way to add any kind of behavior on one or multiple scene nodes. They can be used
	 * to create animations, add physics, artificial intelligence...
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractController
	{
		private var _mode			: uint					= 0;
		
		private var _lastTime		: Number				= 0.0;
		private var _lastTarget		: ISceneNode			= null;
		
		private var _targetType		: Class					= null;
		private var _targets		: Vector.<ISceneNode>	= new <ISceneNode>[];
		
		private var _ticked			: Signal				= new Signal();
		private var _targetAdded	: Signal				= new Signal();
		private var _targetRemoved	: Signal				= new Signal();
		
		/**
		 * The mode of the controller: signal (ConttrollerMode.SIGNAL) or tick (ControllerMode.TICK).
		 * Controllers can work in two different ways:
		 * <ul>
		 * <li>signal: the controller listen for signals dispatched from other objects
		 * - such as its target scene nodes - and execute its operations acccordingle</li>
		 * <li>tick: the controller must be updated every frame and the method "tick"
		 * is called whenever of the its target is in the scene about to be rendered</li>
		 * </ul>
		 * @return 
		 * 
		 */
		public function get mode() : uint
		{
			return _mode;
		}
		
		/**
		 * The number of scene nodes targeted by this very controller. 
		 * @return 
		 * 
		 */
		public function get numTargets() : uint
		{
			return _targets.length;
		}
		
		/**
		 * The signal executed when the "tick" method is called to update the controller.
		 * If the controller's mode is Controller.SIGNAL, the "tick" method is never called
		 * and this signal is never dispatched.
		 * 
		 * Callback functions for this signal should accept the following arguments:
		 * <ul>
		 * <li>controller : AbstractController, the controller from which the signal is
		 * being executed</li>
		 * <li>time : Number, the time - in milliseconds - spent until the Scene object
		 * was created</li>
		 * <li>target : ISceneNode, the target of the controller</li>
		 * </ul>
		 * @return 
		 * 
		 */
		public function get ticked() : Signal
		{
			return _ticked;
		}
		
		/**
		 * The signal executed when a target is added to the controller. 
		 * @return 
		 * 
		 */
		public function get targetAdded() : Signal
		{
			return _targetAdded;
		}
		
		/**
		 * The signal executed when a target is removed from the controller. 
		 * @return 
		 * 
		 */
		public function get targetRemoved() : Signal
		{
			return _targetRemoved;
		}
		
		public function AbstractController(targetType : Class 	= null,
										   mode		  : uint	= 1)
		{
			_targetType = targetType || ISceneNode;
			_mode = mode;
		}
		
		/**
		 * Add a target to the controller. 
		 * @param target
		 * 
		 */
		minko_scene function addTarget(target : ISceneNode) : void
		{
			if (_targetType && !(target is _targetType))
			{
				throw new Error(
					"Controller '" + getQualifiedClassName(this)
					+ " cannot target objects from class '"
					+ getQualifiedClassName(target) + "'."
				);
			}
			
			_targets.push(target);
			
			_targetAdded.execute(this, target);
		}
		
		/**
		 * Remove a target from the controller. 
		 * @param target
		 * 
		 */
		minko_scene function removeTarget(target : ISceneNode) : void
		{
			var index : int = _targets.indexOf(target);
			var numTargets : int = _targets.length - 1;
			
			if (index < 0)
				throw new Error();
			
			_targets[index] = _targets[numTargets];
			_targets.length = numTargets;
			
			_targetRemoved.execute(this, target);
		}
		
		/**
		 * Get the target at the specified index. 
		 * @param index
		 * @return 
		 * 
		 */
		public function getTarget(index : uint) : ISceneNode
		{
			return _targets[index];
		}
		
		public function clone() : AbstractController
		{
			throw new Error("The method AbstractController.clone() must be overriden.");
		}
		
		public function tick(target : ISceneNode, time : Number) : void
		{
			var update : Boolean	= false;
			
			if (time != _lastTime)
			{
				update = updateOnTime(time);
				_lastTime = time;
			}
			
			if (target != _lastTarget || update)
			{
				updateTarget(target);
				_lastTarget = target;
			}
			
			_ticked.execute(this, target, time);
		}
		
		protected function updateOnTime(time : Number) : Boolean
		{
			// nothing
			
			return false;
		}
		
		protected function updateTarget(target : ISceneNode) : void
		{
			// nothing
		}
	}
}