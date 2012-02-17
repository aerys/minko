package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.IDataProvider;
	
	import flash.utils.Dictionary;

	public final class Scene extends Group
	{
		use namespace minko_scene;
		use namespace minko_render;
		
		private static const TMP_SCENE_VECTOR	: Vector.<ISceneNode>	= new Vector.<ISceneNode>();
		
		private var _list			: RenderingList				= new RenderingList();
		
		private var _dataProviders	: Vector.<IDataProvider>	= new <IDataProvider>[];
		private var _meshes			: Vector.<Mesh>				= new <Mesh>[];
		private var _controllers	: Vector.<Group>			= new <Group>[];
		
		private var _invalid		: Boolean					= false;
		private var _locked			: Boolean					= false;
		
		public function get renderingList() : RenderingList
		{
			return _list;
		}
		
		public function get locked() : Boolean
		{
			return _locked;
		}
		
		public function Scene(...children)
		{
			super(children);
		}
		
		public function lock() : void
		{
			_locked = true;
		}
		
		public function unlock() : void
		{
			_locked = false;
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
				meshAddedHandler(child as Mesh);
			else if (child is IDataProvider)
				dataProviderAddedHandler(child as IDataProvider);
			else if (child is Group)
				groupAddedHandler(child as Group);
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (child is Mesh)
				meshRemovedHandler(child as Mesh);
			else if (child is IDataProvider)
				dataProviderRemovedHandler(child as IDataProvider);
			else if (child is Group)
				groupRemovedHandler(child as Group);
		}
		
		private function meshAddedHandler(mesh : Mesh) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				_meshes.push(mesh);
				
				for (var providerId : int = _dataProviders.length - 1; providerId >= 0; --providerId)
					mesh.bindings.add(_dataProviders[providerId]);
				
				_list.addDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
		}
		
		private function meshRemovedHandler(mesh : Mesh) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				var numMeshes : int = _meshes.length - 1;
				var meshIndex : int = 0;
				
				while (_meshes[meshIndex] != mesh)
					++meshIndex;
				_meshes[meshIndex] = _meshes[numMeshes];
				_meshes.length = numMeshes;
				
				for (var bindableIndex : int = _dataProviders.length - 1; bindableIndex >= 0; --bindableIndex)
					mesh.bindings.remove(_dataProviders[bindableIndex]);
				
				_list.removeDrawCalls(mesh.effect._passes, mesh._drawCalls);
			}
		}
		
		private function dataProviderAddedHandler(dataProvider : IDataProvider) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				_dataProviders.push(dataProvider);
				
				for (var meshIndex : int = _meshes.length - 1; meshIndex >= 0; --meshIndex)
					_meshes[meshIndex].bindings.add(dataProvider);
			}
		}
		
		private function dataProviderRemovedHandler(dataProvider : IDataProvider) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				var numBindables	: int	= _dataProviders.length - 1;
				var bindableIndex 	: int 	= 0;
				
				while (_dataProviders[bindableIndex] != dataProvider)
					++bindableIndex;
				_dataProviders[bindableIndex] = _dataProviders[numBindables];
				_dataProviders.length = numBindables;
				
				for (var meshIndex : int = _meshes.length - 1; meshIndex >= 0; --meshIndex)
					_meshes[meshIndex].bindings.remove(dataProvider);
			}
		}
		
		private function groupAddedHandler(group : Group) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				
				// add meshes
				TMP_SCENE_VECTOR.length = 0;
				
				var newMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(
					Mesh,
					TMP_SCENE_VECTOR
				);
				var numMeshes	: int	= newMeshes.length;
				
				for (var meshIndex : int = 0; meshIndex < numMeshes; ++meshIndex)
					meshAddedHandler(newMeshes[meshIndex] as Mesh);
				
				// add bindables
				TMP_SCENE_VECTOR.length = 0;
				
				var newProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(
					IDataProvider,
					TMP_SCENE_VECTOR
				);
				var numProviders	: int	= newProviders.length;
				
				for (var providerId : int = 0; providerId < numProviders; ++providerId)
					dataProviderAddedHandler(newProviders[providerId] as IDataProvider);
				
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
		}
		
		private function groupRemovedHandler(group : Group) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				// remove meshes
				var oldMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(Mesh);
				var numMeshes	: int					= oldMeshes.length;
				
				for (var meshIndex : int = 0; meshIndex < numMeshes; ++meshIndex)
					meshRemovedHandler(oldMeshes[meshIndex] as Mesh);
				
				// remove bindables
				var oldProviders	: Vector.<ISceneNode>	= group.getDescendantsByType(IDataProvider);
				var numProviders	: int					= oldProviders.length;
				
				for (var providerId : int = 0; providerId < numProviders; ++providerId)
					dataProviderRemovedHandler(oldProviders[providerId] as IDataProvider);
				
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
			
			if (_invalid)
			{
				// clear
				var numMeshes 		: int	= _meshes.length;
				var meshId 			: int 	= 0;
				var numProviders	: int	= _dataProviders.length;
				var providerId 		: int 	= 0;
				
				for (meshId = 0; meshId < numMeshes; ++meshId)
					for (providerId = 0; providerId < numProviders; ++providerId)
						(_meshes[meshId] as Mesh).bindings.remove(_dataProviders[providerId]);
				
				_meshes.length = 0;
				_dataProviders.length = 0;
				_list.clear();
				
				// fill
				var newMeshes		: Vector.<ISceneNode>	= getDescendantsByType(Mesh);
				var newProviders	: Vector.<ISceneNode>	= getDescendantsByType(IDataProvider);
				var added			: Dictionary			= new Dictionary(true);
				
				numMeshes = newMeshes.length;
				numProviders = newProviders.length;
				
				for (meshId = 0; meshId < numMeshes; ++meshId)
				{
					var mesh 	: Mesh 	= newMeshes[meshId] as Mesh;
					
					_meshes[meshId] = mesh;
					_list.addDrawCalls(mesh.effect._passes, mesh._drawCalls);
					
					for (providerId = 0; providerId < numProviders; ++providerId)
					{
						var bindable : IDataProvider = newProviders[providerId] as IDataProvider;
						
						mesh.bindings.add(bindable);
						_dataProviders[providerId] = bindable;
					}
				}
			
				_invalid = false;
			}
		}
	}
}