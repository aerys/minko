package aerys.minko.scene.controller.mesh
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class MeshController extends AbstractController
	{
		private var _data 			: DataProvider;
		private var _worldToLocal 	: Matrix4x4;
		
		public function MeshController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_worldToLocal = new Matrix4x4();
			
			targetAdded.add(targetAddedHandler);
		}
		
		private function targetAddedHandler(ctrl	: MeshController,
											target	: Mesh) : void
		{
			target.added.add(addedHandler);
			target.removed.add(removedHandler);
		}
		
		private function addedHandler(target	: Mesh,
									  ancestor	: Group) : void
		{
			if (!target.scene)
				return ;
			
			_data = new DataProvider(
				null, target.name + '_transforms', DataProviderUsage.EXCLUSIVE
			);
			_data.setProperty('localToWorld', target.getLocalToWorldTransform());
			_data.setProperty('worldToLocal', target.getWorldToLocalTransform(false, _worldToLocal));
			
			target.bindings.addProvider(_data);
			
			target.localToWorldTransformChanged.add(localToWorldChangedHandler);
		}
		
		private function removedHandler(target		: Mesh,
										ancestor	: Group) : void
		{
			if (!ancestor.scene)
				return ;
			
			target.localToWorldTransformChanged.remove(localToWorldChangedHandler);
			
			target.bindings.removeProvider(_data);
			
			_data = null;
		}
		
		private function localToWorldChangedHandler(mesh : Mesh, localToWorld : Matrix4x4) : void
		{
			_data.setProperty('localToWorld', localToWorld);
            mesh.getWorldToLocalTransform(false, _worldToLocal);
		}
	}
}