package aerys.minko.scene.controller.scene
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.ControllerMode;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;

	/**
	 * The SceneController listen for the childAdded/childRemoved signals of
	 * a Scene to collect and handle controllers.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class TickController extends AbstractController
	{
		private static const TMP_SCENE_VECTOR	: Vector.<ISceneNode>	= new Vector.<ISceneNode>();
		private static const TIME_OFFSET		: Number				= new Date().time;
		
		private var _ctrlTargets	: Vector.<ISceneNode>	= new <ISceneNode>[];
		
		public function TickController()
		{
			super(Scene, ControllerMode.SIGNAL);
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		override public function clone() : AbstractController
		{
			return new TickController();
		}
		
		public function update() : void
		{
			// update controllers
			var numControllers 	: int 		= _ctrlTargets.length;
			var time			: Number	= new Date().time - TIME_OFFSET;
			
			(getTarget(0) as Scene).bindings.setProperty("time", time);
			
			for (var targetId : int = numControllers - 1; targetId >= 0; --targetId)
			{
				var target 		: ISceneNode 	= _ctrlTargets[targetId] as ISceneNode;
				var numCtrls	: uint			= target.numControllers;
				
				for (var ctrlId : uint = 0; ctrlId < numCtrls; ++ctrlId)
					target.getController(ctrlId).tick(target, time);
			}
		}
		
		private function targetAddedHandler(ctrl	: TickController,
											target	: Scene) : void
		{
			target.added.add(addedHandler);
			target.descendantAdded.add(descendantAddedHandler);
			target.descendantRemoved.add(descendantRemovedHandler);
		}
		
		private function targetRemovedHandler(ctrl		: TickController,
											  target	: Scene) : void
		{
			target.added.remove(addedHandler);
			target.descendantAdded.remove(descendantAddedHandler);
			target.descendantRemoved.remove(descendantRemovedHandler);
		}
		
		private function addedHandler(parent	: Group,
									  child		: Scene) : void
		{
			throw new Error();
		}
		
		private function descendantAddedHandler(group : Group, child : ISceneNode) : void
		{
			var scene	: Scene	= group.root as Scene;
			
			addControllerTarget(child);
			
			if (child is Group)
				groupAddedHandler(scene, child as Group);
		}
		
		private function descendantRemovedHandler(group : Group, child : ISceneNode) : void
		{
			var scene	: Scene	= group.root as Scene;
			
			removeControllerTarget(child);
			
			if (child is Group)
				groupRemovedHandler(scene, child as Group);
		}
		
		private function groupAddedHandler(scene : Scene, group : Group) : void
		{
			// add controllers
			TMP_SCENE_VECTOR.length = 0;
			var newTargets	: Vector.<ISceneNode>	= group.getDescendantsByType(
				ISceneNode,
				TMP_SCENE_VECTOR
			);
			var numTargets	: int					= newTargets.length;
			
			for (var targetId : int = 0; targetId < numTargets; ++targetId)
				addControllerTarget(newTargets[targetId]);
		}
		
		private function groupRemovedHandler(scene : Scene, group : Group) : void
		{
			// remove controllers
			TMP_SCENE_VECTOR.length = 0;
			var oldTargets	: Vector.<ISceneNode>	= group.getDescendantsByType(
				ISceneNode,
				TMP_SCENE_VECTOR
			);
			
			for (var targetId : int = oldTargets.length - 1; targetId >= 0; --targetId)
				removeControllerTarget(oldTargets[targetId]);
		}
		
		private function controllerAddedHandler(target		: ISceneNode,
												controller	: AbstractController) : void
		{
			if (controller.mode == ControllerMode.TICK)
			{
				target.controllerAdded.remove(controllerAddedHandler);
				target.controllerRemoved.add(controllerRemovedHandler);
				
				_ctrlTargets.push(target);
			}
		}
		
		private function controllerRemovedHandler(target		: ISceneNode,
												  controller	: AbstractController) : void
		{
			if (target.numControllers == 0)
			{
				removeControllerTarget(target);
				target.controllerAdded.add(controllerAddedHandler);
				
				removeControllerTarget(target);
			}
		}
		
		private function addControllerTarget(target : ISceneNode) : void
		{
			var numControllers	: uint	= target.numControllers;
			
			for (var ctrlId : uint = 0; ctrlId < numControllers; ++ctrlId)
			{
				if (target.getController(ctrlId).mode == ControllerMode.TICK)
				{
					_ctrlTargets.push(target);
					
					return ;
				}
			}
			
			target.controllerAdded.add(controllerAddedHandler);
		}
		
		private function removeControllerTarget(target : ISceneNode) : void
		{
			var index : int = _ctrlTargets.indexOf(target);
			
			if (index >= 0)
			{
				var numTargets : uint = _ctrlTargets.length - 1;
				
				_ctrlTargets[index] = _ctrlTargets[numTargets];
				_ctrlTargets.length = numTargets;
			}
		}
	}
}