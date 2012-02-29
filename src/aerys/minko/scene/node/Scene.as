package aerys.minko.scene.node
{
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.ControllerMode;
	import aerys.minko.scene.controller.RenderingController;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.IDataProvider;

	public final class Scene extends Group
	{
		private static const TMP_SCENE_VECTOR	: Vector.<ISceneNode>	= new Vector.<ISceneNode>();
		private static const TIME_OFFSET		: Number				= new Date().time;
		
		private var _list			: RenderingList			= new RenderingList();
		
		private var _bindings		: DataBindings			= new DataBindings();
		private var _ctrlTargets	: Vector.<ISceneNode>	= new <ISceneNode>[];

		public function get renderingList() : RenderingList
		{
			return _list;
		}
		
		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
		public function Scene(...children)
		{
			super(children);
		}
		
		override protected function initialize() : void
		{
			super.initialize();
			
			childAdded.add(childAddedHandler);
			childRemoved.add(childRemovedHandler);
		}
		
		private function childAddedHandler(group : Group, child : ISceneNode) : void
		{
			addControllerTarget(child);
			
			if (child is IDataProvider)
				_bindings.add(child as IDataProvider);
			
			if (child is Group)
				groupAddedHandler(child as Group);
			
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			removeControllerTarget(child);
			
			if (child is IDataProvider)
				_bindings.remove(child as IDataProvider);
			
			if (child is Group)
				groupRemovedHandler(child as Group);
		}
		
		private function groupAddedHandler(group : Group) : void
		{
			// add data providers
			TMP_SCENE_VECTOR.length = 0;
			
			var newProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(
				IDataProvider,
				TMP_SCENE_VECTOR
			);
			var numProviders	: int	= newProviders.length;
			
			for (var providerId : int = 0; providerId < numProviders; ++providerId)
				_bindings.add(newProviders[providerId] as IDataProvider);
			
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
		
		private function groupRemovedHandler(group : Group) : void
		{
			// remove data providers
			var oldProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(
				IDataProvider
			);
			var numProviders	: int					= oldProviders.length;
			
			for (var providerId : int = 0; providerId < numProviders; ++providerId)
				_bindings.remove(oldProviders[providerId] as IDataProvider);
			
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
		
		public function update() : void

		{
			// update controllers
			var numControllers 	: int 		= _ctrlTargets.length;
			var time			: Number	= new Date().time - TIME_OFFSET;
			
			_bindings.setProperty("time", time);
			
			for (var targetId : int = numControllers - 1; targetId >= 0; --targetId)
			{
				var target 		: ISceneNode 	= _ctrlTargets[targetId] as ISceneNode;
				var numCtrls	: uint			= target.numControllers;
				
				for (var ctrlId : uint = 0; ctrlId < numCtrls; ++ctrlId)
					target.getController(ctrlId).tick(target, time);
			}
		}
	}
}