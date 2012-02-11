package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.data.IBindable;
	
	import flash.utils.Dictionary;

	public final class Scene extends Group
	{
		use namespace minko_scene;
		
		private var _list			: RenderingList			= new RenderingList();
		
		private var _bindables		: Vector.<IBindable>	= new <IBindable>[];
		private var _meshes			: Vector.<Mesh>			= new <Mesh>[];
		private var _controllers	: Vector.<Group>		= new <Group>[];
		
		private var _invalid		: Boolean				= false;
		private var _locked			: Boolean				= false;
		
		public function get renderingList() : RenderingList
		{
			return _list;
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
				meshAddedHandler(child as Mesh);
			else if (child is IBindable)
				bindableAddedHandler(child as IBindable);
			else if (child is Group)
				groupAddedHandler(child as Group);
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (child is Mesh)
				meshRemovedHandler(child as Mesh);
			else if (child is IBindable)
				bindableRemovedHandler(child as IBindable);
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
				
				for (var bindableIndex : int = _bindables.length - 1; bindableIndex >= 0; --bindableIndex)
					mesh.bindings.add(_bindables[bindableIndex]);
				
				_list.addDrawCalls(mesh.effect.passes, mesh._drawCalls);
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
				var meshIndex : int = 0;
				
				while (_meshes[meshIndex] != mesh)
					++meshIndex;
				_meshes.splice(meshIndex, 1);
				
				for (var bindableIndex : int = _bindables.length - 1; bindableIndex >= 0; --bindableIndex)
					mesh.bindings.remove(_bindables[bindableIndex]);
				
				_list.removeDrawCalls(mesh.effect.passes, mesh._drawCalls);
			}
		}
		
		private function bindableAddedHandler(bindable : IBindable) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				_bindables.push(bindable);
				
				for (var meshIndex : int = _meshes.length - 1; meshIndex >= 0; --meshIndex)
					_meshes[meshIndex].bindings.add(bindable);
			}
		}
		
		private function bindableRemovedHandler(bindable : IBindable) : void
		{
			if (_locked)
			{
				_invalid = true;
			}
			else
			{
				var bindableIndex : int = 0;
				
				while (_bindables[bindableIndex] != bindable)
					++bindableIndex;
				_bindables.splice(bindableIndex, 1);
				
				for (var meshIndex : int = _meshes.length - 1; meshIndex >= 0; --meshIndex)
					_meshes[meshIndex].bindings.remove(bindable);
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
				var newMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(Mesh);
				var numMeshes	: int					= newMeshes.length;
				
				for (var meshIndex : int = 0; meshIndex < numMeshes; ++meshIndex)
					meshAddedHandler(newMeshes[meshIndex] as Mesh);
				
				// add bindables
				var newBindables	: Vector.<ISceneNode>	= group.getDescendantsByType(IBindable);
				var numBindables	: int					= newBindables.length;
				
				for (var bindableIndex : int = 0; bindableIndex < numBindables; ++bindableIndex)
					bindableAddedHandler(newBindables[bindableIndex] as IBindable);
				
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
				var oldBindables	: Vector.<ISceneNode>	= group.getDescendantsByType(IBindable);
				var numBindables	: int					= oldBindables.length;
				
				for (var bindableIndex : int = 0; bindableIndex < numBindables; ++bindableIndex)
					bindableRemovedHandler(oldBindables[bindableIndex] as IBindable);
				
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
			var controllerIndex	: int = _controllers.length - 1;
			
			while ((_controllers[controllerIndex] as Group).controller != controller)
				--controllerIndex;
			
			_controllers.splice(controllerIndex, 1);
		}
		
		public function update() : void
		{
			// update controllers
			var numControllers 	: int 		= _controllers.length;
			var time			: Number	= new Date().time;
			
			for (var ctrlIndex : int = 0; ctrlIndex < numControllers; ++ctrlIndex)
			{
				var ctrlGroup : Group = _controllers[ctrlIndex] as Group;
				
				ctrlGroup.controller.tick(ctrlGroup, time);
			}
			
			if (_invalid)
			{
				// clear
				var numMeshes 		: int	= _meshes.length;
				var meshIndex 		: int 	= 0;
				var numBindables	: int	= _bindables.length;
				var bindableIndex 	: int 	= 0;
				
				for (meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
					for (bindableIndex = 0; bindableIndex < numBindables; ++bindableIndex)
						(_meshes[meshIndex] as Mesh).bindings.remove(_bindables[bindableIndex]);
				
				_meshes.length = 0;
				_bindables.length = 0;
				_list.clear();
				
				// fill
				var newMeshes		: Vector.<ISceneNode>	= getDescendantsByType(Mesh);
				var newBindables	: Vector.<ISceneNode>	= getDescendantsByType(IBindable);
				var added			: Dictionary			= new Dictionary(true);
				
				numMeshes = newMeshes.length;
				numBindables = newBindables.length;
				
				for (meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
				{
					var mesh 	: Mesh 	= newMeshes[meshIndex] as Mesh;
					
					_meshes[meshIndex] = mesh;
					_list.addDrawCalls(mesh.effect.passes, mesh._drawCalls);
					
					for (bindableIndex = 0; bindableIndex < numBindables; ++bindableIndex)
					{
						var bindable : IBindable = newBindables[bindableIndex] as IBindable;
						
						mesh.bindings.add(bindable);
						_bindables[bindableIndex] = bindable;
					}
				}
			
				_invalid = false;
			}
		}
	}
}