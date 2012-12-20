package aerys.minko.scene.controller.mesh
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataProvider;
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
			target.addedToScene.add(targetAddedToSceneHandler);
			target.removedFromScene.add(targetRemovedFromSceneHandler);
		}
		
		private function targetAddedToSceneHandler(target	: Mesh,
												   scene	: Scene) : void
		{
			_data = new DataProvider();
			_data.setProperty('localToWorld', target.getLocalToWorldTransform());
			_data.setProperty('worldToLocal', target.getWorldToLocalTransform(_worldToLocal));
			target.bindings.addProvider(_data);
			
			target.localToWorldTransformChanged.add(localToWorldChangedHandler);
		}
		
		private function targetRemovedFromSceneHandler(target	: Mesh,
													   scene	: Scene) : void
		{
			target.localToWorldTransformChanged.remove(localToWorldChangedHandler);
			
			target.bindings.removeProvider(_data);
			_data = null;
		}
		
		private function localToWorldChangedHandler(mesh : Mesh, localToWorld : Matrix4x4) : void
		{
			_data.setProperty('localToWorld', localToWorld);
			_data.setProperty('worldToLocal', _worldToLocal.copyFrom(localToWorld).invert());
		}
	}
}