package aerys.minko.scene.controller.mesh
{
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
	
	import flash.display.BitmapData;
	
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
			_geometry = value;
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
			target.added.add(addedHandler);
			target.removed.add(removedHandler);
		}
		
		private function addedHandler(target	: Mesh,
									  ancestor	: Group) : void
		{
			if (!target.scene)
				return ;
            
            _mesh = target;
            target.scene.renderingBegin.add(nextFrameHanlder);
		}
        
        private function nextFrameHanlder(scene			: Scene,
                                          viewport		: Viewport,
                                          destination	: BitmapData,
                                          time			: Number) : void
        {
            scene.renderingBegin.remove(nextFrameHanlder);

            var transformController : TransformController = scene.getControllersByType(TransformController)[0]
                as TransformController;
            
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
			if (!ancestor.scene)
				return ;
			
            if (ancestor.scene.renderingBegin.hasCallback(nextFrameHanlder))
            {
                ancestor.scene.renderingBegin.remove(nextFrameHanlder);
                return ;
            }
            
			target.bindings.removeProvider(_data);
			_data.dispose();
			_data = null;
            
            _localToWorld = null;
            _worldToLocal = null;
            
            _mesh = null;
		}
	}
}