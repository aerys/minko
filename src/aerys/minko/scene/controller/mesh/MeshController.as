package aerys.minko.scene.controller.mesh
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.TransformController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class MeshController extends AbstractController
	{
		private var _data 			: DataProvider;
        private var _localToWorld   : Matrix4x4;
		private var _worldToLocal 	: Matrix4x4;
		
		public function MeshController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
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
            
            _localToWorld = new Matrix4x4();
            _worldToLocal = new Matrix4x4();
            
            var transformController : TransformController = target.scene.getControllersByType(TransformController)[0]
                as TransformController;
			
            transformController.setSharedLocalToWorldTransformReference(target, _localToWorld);
            transformController.setSharedWorldToLocalTransformReference(target, _worldToLocal);
            transformController.synchronizeTransforms(target, true);
            transformController.lockTransformsBeforeUpdate(target, true);
            
			_data = new DataProvider(
				null, target.name + '_transforms', DataProviderUsage.EXCLUSIVE
			);
			_data.setProperty('localToWorld', _localToWorld);
			_data.setProperty('worldToLocal', _worldToLocal);
			
			target.bindings.addProvider(_data);
		}
		
		private function removedHandler(target		: Mesh,
										ancestor	: Group) : void
		{
			if (!ancestor.scene)
				return ;
			
			target.bindings.removeProvider(_data);
			_data.dispose();
			_data = null;
            
            _localToWorld = null;
            _worldToLocal = null;
		}
	}
}