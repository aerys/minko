package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.DataBinding;
	import aerys.minko.type.data.IDataProvider;

	public final class Scene extends Group
	{
		use namespace minko_scene;
		use namespace minko_render;
		
		private static const TMP_SCENE_VECTOR	: Vector.<ISceneNode>	= new Vector.<ISceneNode>();
		
		private var _list			: RenderingList				= new RenderingList();
		
		private var _globalBindinds	: DataBinding				= new DataBinding();
		private var _controllers	: Vector.<Group>			= new <Group>[];

		public function get renderingList() : RenderingList
		{
			return _list;
		}
		
		public function get globalBindings() : DataBinding
		{
			return _globalBindinds;
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
			if (child is Mesh)
			{
				var mesh : Mesh = child as Mesh;
				
				_list.addDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
			if (child is IDataProvider)
				_globalBindinds.add(child as IDataProvider);
			else if (child is Group)
				groupAddedHandler(child as Group);
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (child is Mesh)
			{
				var mesh : Mesh = child as Mesh;
				
				_list.removeDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
			if (child is IDataProvider)
				_globalBindinds.remove(child as IDataProvider);
			else if (child is Group)
				groupRemovedHandler(child as Group);
		}
		
		private function groupAddedHandler(group : Group) : void
		{
			// add meshes
			TMP_SCENE_VECTOR.length = 0;
			
			var newMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(
				Mesh,
				TMP_SCENE_VECTOR
			);
			var numMeshes	: int	= newMeshes.length;
			
			for (var meshId : int = 0; meshId < numMeshes; ++meshId)
			{
				var mesh : Mesh = newMeshes[meshId] as Mesh;
				
				_list.addDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
			
			// add data providers
			TMP_SCENE_VECTOR.length = 0;
			
			var newProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(
				IDataProvider,
				TMP_SCENE_VECTOR
			);
			var numProviders	: int	= newProviders.length;
			
			for (var providerId : int = 0; providerId < numProviders; ++providerId)
				_globalBindinds.add(newProviders[providerId] as IDataProvider);
			
			// add controllers
			group.controllerChanged.add(controllerChangedHandler);
			if (group.controller)
				_controllers.push(group);
			
			var newGroups	: Vector.<ISceneNode>	= group.getDescendantsByType(Group);
			var numGroups	: int					= newGroups.length;
			
			for (var groupIndex : int = 0; groupIndex < numGroups; ++groupIndex)
			{
				group = newGroups[groupIndex] as Group;
				
				var controller : AbstractController	= group.controller;
				
				group.controllerChanged.add(controllerChangedHandler);
				
				if (controller)
					_controllers.push(group);
			}
		}
		
		private function groupRemovedHandler(group : Group) : void
		{
			// remove meshes
			var oldMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(Mesh);
			var numMeshes	: int					= oldMeshes.length;
			
			for (var meshId : int = 0; meshId < numMeshes; ++meshId)
			{
				var mesh : Mesh = oldMeshes[meshId] as Mesh;
				
				_list.removeDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
			
			// remove data providers
			var oldProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(IDataProvider);
			var numProviders	: int					= oldProviders.length;
			
			for (var providerId : int = 0; providerId < numProviders; ++providerId)
				_globalBindinds.remove(oldProviders[providerId] as IDataProvider);
			
			// remove controllers
			var controller : AbstractController	= group.controller;
			
			group.controllerChanged.remove(controllerChangedHandler);
			if (controller)
				removeController(controller);
			
			var newGroups	: Vector.<ISceneNode>	= group.getDescendantsByType(Group);
			var numGroups	: int					= 0;
			
			for (var groupIndex : int = 0; groupIndex < numGroups; ++groupIndex)
			{
				group = newGroups[groupIndex] as Group;
				controller = group.controller;
				
				group.controllerChanged.remove(controllerChangedHandler);
				
				if (controller)
					removeController(controller);
			}
		}
		
		private function controllerChangedHandler(group 		: Group,
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
				_controllers.push(group);
			}
		}
		
		private function removeController(controller : AbstractController) : void
		{
			var numControllers 	: int	= _controllers.length - 1;
			var controllerIndex	: int	= 0;
			
			while ((_controllers[controllerIndex] as Group).controller != controller)
				controllerIndex++;
			
			_controllers[controllerIndex] = _controllers[numControllers];
			_controllers.length = numControllers;
		}
		
		public function update() : void
		{
			// update controllers
			var numControllers 	: int 		= _controllers.length;
			var time			: Number	= new Date().time;
			
			for (var ctrlIndex : int = numControllers - 1; ctrlIndex >= 0; --ctrlIndex)
			{
				var ctrlGroup : Group = _controllers[ctrlIndex] as Group;
				
				ctrlGroup.controller.tick(ctrlGroup, time);
			}
		}
	}
}