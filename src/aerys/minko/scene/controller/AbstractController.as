package aerys.minko.scene.controller
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	
	import flash.display.BitmapData;
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
		private var _targetType		: Class					= null;
		private var _targets		: Vector.<ISceneNode>	= new <ISceneNode>[];
		
		private var _ticked			: Signal				= new Signal('AbstractController.ticked');
		private var _targetAdded	: Signal				= new Signal('AbstractController.targetAdded');
		private var _targetRemoved	: Signal				= new Signal('AbstractController.targetRemoved');
		
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
		
		public function AbstractController(targetType 		: Class 	= null)
		{
			_targetType = targetType || ISceneNode;
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
	}
}