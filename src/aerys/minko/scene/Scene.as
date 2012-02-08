package aerys.minko.scene
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderingList;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.scene.group.Group;
	import aerys.minko.scene.mesh.Mesh;
	import aerys.minko.type.controller.IController;
	import aerys.minko.type.data.IBindable;

	public final class Scene extends Group
	{
		use namespace minko_scene;
		
		private var _list				: RenderingList				= new RenderingList();
		
		private var _bindables			: Vector.<IBindable>		= new <IBindable>[];
		private var _meshes				: Vector.<Mesh>				= new <Mesh>[];
		private var _controllers		: Vector.<IController>		= new <IController>[];
		
		private var _invalidList		: Boolean					= false;
		private var _invalidBindings	: Boolean					= false;
		private var _locked				: Boolean					= false;
		
		public function get renderingList() : RenderingList
		{
			if (_invalidList)
				updateRenderingList();
			
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
			
			updateBindings();
			updateRenderingList();
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
			trace("remove", group, child);
			
			if (child is Mesh)
				meshRemovedHandler(child as Mesh);
			else if (child is IBindable)
				bindableRemovedHandler(child as IBindable);
			else if (child is Group)
				groupRemovedHandler(child as Group);
		}
		
		private function meshAddedHandler(mesh : Mesh) : void
		{
			_invalidList = true;
			
			if (_locked)
			{
				_invalidBindings = true;
			}
			else
			{
				_meshes.push(mesh);
				
				for (var bindableIndex : int = _bindables.length - 1; bindableIndex >= 0; --bindableIndex)
					mesh.bindings.add(_bindables[bindableIndex]);
			}
		}
		
		private function meshRemovedHandler(mesh : Mesh) : void
		{
			_invalidList = true;
			
			if (_locked)
			{
				_invalidBindings = true;
			}
			else
			{
				var meshIndex : int = 0;
				
				while (_meshes[meshIndex] != mesh)
					++meshIndex;
				_meshes.splice(meshIndex, 1);
				
				for (var bindableIndex : int = _bindables.length - 1; bindableIndex >= 0; --bindableIndex)
					mesh.bindings.remove(_bindables[bindableIndex]);
			}
		}
		
		private function bindableAddedHandler(bindable : IBindable) : void
		{
			_bindables.push(bindable);
			
			if (_locked)
			{
				_invalidBindings = true;
			}
			else
			{
				for (var meshIndex : int = _meshes.length - 1; meshIndex >= 0; --meshIndex)
					_meshes[meshIndex].bindings.add(bindable);
			}
		}
		
		private function bindableRemovedHandler(bindable : IBindable) : void
		{
			if (_locked)
			{
				_invalidBindings = true;
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
				_invalidBindings = true;
				_invalidList = true;
			}
			else
			{
				var newMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(Mesh);
				var meshIndex	: int					= newMeshes.length - 1;
				
				while (meshIndex >= 0)
				{
					meshAddedHandler(newMeshes[meshIndex] as Mesh);
					--meshIndex;
				}
				
				var newBindables	: Vector.<ISceneNode>	= group.getDescendantsByType(IBindable);
				var bindableIndex	: int					= newBindables.length - 1;
				
				while (bindableIndex >= 0)
				{
					bindableAddedHandler(newBindables[bindableIndex] as IBindable);
					--bindableIndex;
				}
			}
		}
		
		private function groupRemovedHandler(group : Group) : void
		{
			if (_locked)
			{
				_invalidBindings = true;
				_invalidList = true;
			}
			else
			{
				var oldMeshes	: Vector.<ISceneNode>	= group.getDescendantsByType(Mesh);
				var meshIndex	: int					= oldMeshes.length - 1;
				
				while (meshIndex >= 0)
				{
					meshRemovedHandler(oldMeshes[meshIndex] as Mesh);
					--meshIndex;
				}
				
				var oldBindables	: Vector.<ISceneNode>	= group.getDescendantsByType(IBindable);
				var bindableIndex	: int					= oldBindables.length - 1;
				
				while (bindableIndex >= 0)
				{
					bindableRemovedHandler(oldBindables[bindableIndex] as IBindable);
					--bindableIndex;
				}
			}
		}
		
		private function updateRenderingList() : void
		{
			if (_invalidList)
			{
				_list.clear();
				
				var newMeshes	: Vector.<ISceneNode>	= getDescendantsByType(Mesh);
				var numMeshes 	: int 					= newMeshes.length;
				
				trace("updateRenderingList", numMeshes);
				
				for (var meshIndex : int = 0; meshIndex < numMeshes; ++meshIndex)
				{
					var mesh		: Mesh							= newMeshes[meshIndex] as Mesh;
					var passes		: Vector.<ActionScriptShader>	= mesh.effect.passes;
					var numPasses	: int							= passes.length;
					var drawCalls 	: Vector.<DrawCall>				= mesh._drawCalls;
					
					_meshes[meshIndex] = mesh;
					
					for (var i : int = 0; i < numPasses; ++i)
						_list.pushDrawCall(passes[i].state, drawCalls[i]);
				}
				
				_meshes.length = numMeshes;
				
				_invalidList = false;
			}
		}
		
		private function updateBindings() : void
		{
			if (_invalidBindings)
			{
				var numMeshes 		: int 		= _meshes.length;
				var meshIndex 		: int 		= 0;
				var mesh			: Mesh		= null;
				var numBindables	: int		= _bindables.length;
				var bindableIndex 	: int 		= 0;
				var bindable		: IBindable	= null;
				
				// remove all bindings
				for (meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
				{
					mesh = _meshes[meshIndex];
					
					for (bindableIndex = 0; bindableIndex < numBindables; ++bindableIndex)
						mesh.bindings.remove(_bindables[bindableIndex]);
				}
				
				// empty lists
				_meshes.length = 0;
				_bindables.length = 0;
				
				// get new meshes and add bindings
				var newMeshes		: Vector.<ISceneNode>	= getDescendantsByType(Mesh);
				var newBindables	: Vector.<ISceneNode>	= getDescendantsByType(IBindable);
				var filled			: Boolean				= false;
				
				numMeshes = newMeshes.length;
				for (meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
				{
					mesh = newMeshes[meshIndex] as Mesh;
					_meshes[meshIndex] = mesh;
					
					for (bindableIndex = 0; bindableIndex < numBindables; ++bindableIndex)
					{
						bindable = newBindables[bindableIndex] as IBindable;
						mesh.bindings.add(bindable);
						
						if (filled)
							_bindables[bindableIndex] = bindable;
					}
					
					filled = true;
				}
			}
		}
	}
}