package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.ControllerMode;
	import aerys.minko.scene.controller.IControllerTarget;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.data.IDataProvider;

	public final class Scene extends Group
	{
		use namespace minko_scene;
		use namespace minko_render;
		
		private static const TMP_SCENE_VECTOR	: Vector.<ISceneNode>	= new Vector.<ISceneNode>();
		private static const TIME_OFFSET		: Number				= new Date().time;
		
		private var _list			: RenderingList					= new RenderingList();
		
		private var _bindings		: DataBindings					= new DataBindings();
		private var _ctrlTargets	: Vector.<IControllerTarget>	= new <IControllerTarget>[];

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
			if (child is IDataProvider)
				_bindings.add(child as IDataProvider);
			
			if (child is IControllerTarget)
			{
				var target 		: IControllerTarget 	= child as IControllerTarget;
				var controller	: AbstractController	= target.controller;
				
				target.controllerChanged.add(controllerChangedHandler);
				if (controller && controller.mode == ControllerMode.TICK)
					_ctrlTargets.push(target);
			}
			
			if (child is Group)
				groupAddedHandler(child as Group);
			
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (child is IDataProvider)
				_bindings.remove(child as IDataProvider);
			
			if (child is IControllerTarget)
			{
				var target 		: IControllerTarget 	= child as IControllerTarget;
				var controller	: AbstractController	= target.controller;
				
				target.controllerChanged.remove(controllerChangedHandler);
				
				if (controller && controller.mode == ControllerMode.TICK)
					removeController(controller);
			}
			
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
				IControllerTarget,
				TMP_SCENE_VECTOR
			);
			var numTargets	: int					= newTargets.length;
			
			for (var targetId : int = 0; targetId < numTargets; ++targetId)
			{
				var target		: IControllerTarget	= newTargets[targetId]
													  as IControllerTarget;
				var controller : AbstractController	= target.controller;
				
				target.controllerChanged.add(controllerChangedHandler);
				
				if (controller && controller.mode == ControllerMode.TICK)
					_ctrlTargets.push(target);
			}
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
				IControllerTarget,
				TMP_SCENE_VECTOR
			);
			var numTargets	: int					= oldTargets.length;
			
			for (var targetId : int = 0; targetId < numTargets; ++targetId)
			{
				var target : IControllerTarget = oldTargets[targetId]
												 as IControllerTarget;
				var controller : AbstractController	= target.controller;
				
				target.controllerChanged.remove(controllerChangedHandler);
				
				if (controller && controller.mode == ControllerMode.TICK)
					removeController(controller);
			}
		}
		
		private function controllerChangedHandler(target		: IControllerTarget,
												  oldController : AbstractController,
												  newController : AbstractController) : void
		{
			if (oldController != null)
			{
				if (newController == null)
					removeController(oldController);
			}
			else
			{
				_ctrlTargets.push(target);
			}
		}
		
		private function removeController(controller : AbstractController) : void
		{
			var numControllers 	: int	= _ctrlTargets.length - 1;
			var controllerIndex	: int	= 0;
		
			while ((_ctrlTargets[controllerIndex] as IControllerTarget).controller != controller)
				controllerIndex++;
		
			_ctrlTargets[controllerIndex] = _ctrlTargets[numControllers];
			_ctrlTargets.length = numControllers;
		}
		
		public function update() : void

		{
			// update controllers
			var numControllers 	: int 		= _ctrlTargets.length;
			var time			: Number	= new Date().time - TIME_OFFSET;
			
			_bindings.setProperty("time", time);
			
			for (var ctrlIndex : int = numControllers - 1; ctrlIndex >= 0; --ctrlIndex)
			{
				var target : IControllerTarget = _ctrlTargets[ctrlIndex] as IControllerTarget;
				
				target.controller.tick(target, time);false;
			}
		}
	}
}