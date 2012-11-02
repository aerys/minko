package aerys.minko.scene.controller
{
	import aerys.minko.scene.data.TransformDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;

	public final class TransformController extends AbstractController
	{
		private var _node	: ISceneNode;
		private var _data	: TransformDataProvider;
		
		public function TransformController()
		{
			super();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: TransformController,
											node 	: ISceneNode) : void
		{
			if (_node)
				throw new Error();
			
			_node = node;
			_node.transform.changed.add(transformChangedHandler);
			_node.added.add(addedHandler);
			_node.removed.add(removedHandler);
			
			if (_node is Mesh)
			{
				_node.addedToScene.add(meshAddedToSceneHandler);
				_node.removedFromScene.add(meshRemovedFromSceneHandler);
			}
			
			transformChangedHandler(_node.transform);
		}
		
		private function targetRemovedHandler(ctrl	: TransformController,
											  node 	: ISceneNode) : void
		{
			if (_node is Mesh)
			{
				_node.addedToScene.remove(meshAddedToSceneHandler);
				_node.removedFromScene.remove(meshRemovedFromSceneHandler);
			}
			
			_node.removed.remove(removedHandler);
			_node.added.remove(addedHandler);
			_node.transform.changed.remove(transformChangedHandler);
			_node = null;
		}
		
		private function meshAddedToSceneHandler(mesh : Mesh, scene : Scene) : void
		{
			_data = new TransformDataProvider(mesh.localToWorld, mesh.worldToLocal);
			mesh.bindings.addProvider(_data);
		}
		
		private function meshRemovedFromSceneHandler(mesh : Mesh, scene : Scene) : void
		{
			mesh.bindings.removeProvider(_data);
			_data = null;
		}
		
		private function transformChangedHandler(transform : Matrix4x4) : void
		{
			var parent : ISceneNode = _node.parent;
			
            _node.worldToLocal.lock();
            _node.localToWorld.lock();
            
			if (_node.parent)
			{
				_node.localToWorld.lock()
					.copyFrom(_node.transform)
					.append(parent.localToWorld)
			}
			else
				_node.localToWorld.copyFrom(_node.transform);
			
			_node.worldToLocal.lock()
				.copyFrom(_node.localToWorld)
				.invert();
            
            _node.worldToLocal.unlock();
            _node.localToWorld.unlock();
		}
		
		private function addedHandler(node : ISceneNode, parent : Group) : void
		{
			if (node === _node)
			{
				parent.localToWorld.changed.add(transformChangedHandler);
				transformChangedHandler(_node.transform);
			}
		}
		
		private function removedHandler(node : ISceneNode, parent : Group) : void
		{
			if (node === _node)
			{
				parent.localToWorld.changed.remove(transformChangedHandler);
				transformChangedHandler(_node.transform);
			}
		}
	}
}