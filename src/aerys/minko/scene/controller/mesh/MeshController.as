package aerys.minko.scene.controller.mesh
{
	import flash.display.BitmapData;
	
	import aerys.minko.render.Viewport;
	import aerys.minko.render.geometry.Geometry;
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
        private var _mesh           : Mesh;
        
		private var _data 			: DataProvider;
		
		private var _geometry		: Geometry;
		private var _frame			: uint;
        private var _localToWorld   : Matrix4x4;
		private var _worldToLocal 	: Matrix4x4;

		public function get geometry() : Geometry
		{
			return _geometry;
		}
		
		public function set geometry(value : Geometry) : void
		{
			if (_geometry && _mesh)
				_mesh.bindings.removeProvider(_geometry.geometryDataProvider);
			
			_geometry = value;
			
			if (_mesh && _geometry)
				_mesh.bindings.addProvider(_geometry.geometryDataProvider);
			
			if (_data)
				_data.setProperty('geometry', value);
		}
		
		public function get frame() : uint
		{
			return _frame;
		}
		public function set frame(value : uint) : void
		{
			_frame = value;
			if (_data)
				_data.setProperty('frame', value);
		}
		
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
			if (!target.added.hasCallback(addedHandler))
				target.added.add(addedHandler);
			if (!target.removed.hasCallback(removedHandler))
				target.removed.add(removedHandler);
		}
		
		private function addedHandler(target	: Mesh,
									  ancestor	: Group) : void
		{
			if (!target.scene)
				return ;
			
            _mesh = target;
            _mesh.scene.renderingBegin.add(sceneRenderingBeginHandler);
			
			if (_geometry && !_mesh.bindings.hasProvider(_geometry.geometryDataProvider))
			{
				_mesh.bindings.addProvider(_geometry.geometryDataProvider);
			}
		}
        
        private function sceneRenderingBeginHandler(scene			: Scene,
                                                    viewport		: Viewport,
                                                    destination	    : BitmapData,
                                                    time			: Number) : void
        {
            scene.renderingBegin.remove(sceneRenderingBeginHandler);
            
            var transformController : TransformController = _mesh.scene
                .getControllersByType(TransformController)[0] as TransformController;
            
            _localToWorld = transformController.getLocalToWorldTransform(_mesh);
            _worldToLocal = transformController.getWorldToLocalTransform(_mesh);
            transformController.synchronizeTransforms(_mesh, true);
            transformController.lockTransformsBeforeUpdate(_mesh, true);
            
            _data = new DataProvider(
                null, _mesh.name + '_DataProvider', DataProviderUsage.EXCLUSIVE
            );
            _data.setProperty('geometry', _geometry);
            _data.setProperty('frame', _frame);
            _data.setProperty('localToWorld', _localToWorld);
            _data.setProperty('worldToLocal', _worldToLocal);
            
            _mesh.bindings.addProvider(_data);
        }
		
		private function removedHandler(target		: Mesh,
										ancestor	: Group) : void
		{
            var scene : Scene = ancestor.scene;
            
			if (!scene)
				return ;

			if (_geometry && _mesh)
				_mesh.bindings.removeProvider(_geometry.geometryDataProvider);
			
            if (scene.renderingBegin.hasCallback(sceneRenderingBeginHandler))
                scene.renderingBegin.remove(sceneRenderingBeginHandler);
			
            if (_data)
            {
    			target.bindings.removeProvider(_data);
    			_data.dispose();
    			_data = null;
            }
            
            _localToWorld = null;
            _worldToLocal = null;
            
            _mesh = null;
		}
	}
}