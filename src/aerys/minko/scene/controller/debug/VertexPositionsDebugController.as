package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.geometry.primitive.CubeGeometry;
	import aerys.minko.render.geometry.stream.iterator.VertexIterator;
	import aerys.minko.render.geometry.stream.iterator.VertexReference;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.enum.Blending;
	
	public final class VertexPositionsDebugController extends AbstractController
	{
		private var _vertexPositionMaterial	: Material;
		
		public function VertexPositionsDebugController(vertexPositionMaterial : Material = null)
		{
			super(Mesh);
			
			initialize(vertexPositionMaterial);
		}
		
		private function initialize(vertexPositionMaterial : Material) : void
		{
			_vertexPositionMaterial = vertexPositionMaterial;
			if (!_vertexPositionMaterial)
			{
				_vertexPositionMaterial = new BasicMaterial({
					diffuseColor 	: 0xff00007f,
					blending		: Blending.ALPHA
				});
			}
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: VertexPositionsDebugController,
											target	: Mesh) : void
		{
			target.addedToScene.add(targetAddedToSceneHandler);
		}
		
		private function targetAddedToSceneHandler(target 	: Mesh,
												   scene	: Scene) : void
		{
			var vertices : VertexIterator = new VertexIterator(target.geometry.getVertexStream(0));
			
			for each (var vertex : VertexReference in vertices)
			{
				var position : Mesh = new Mesh(
					CubeGeometry.cubeGeometry, _vertexPositionMaterial, '__position__'
				);
				
				position.transform
					.appendUniformScale(.025)
					.appendTranslation(vertex.x, vertex.y, vertex.z);
				
				target.parent.addChild(position);
			}
		}
		
		private function targetRemovedHandler(ctrl		: VertexPositionsDebugController,
											  target	: Mesh) : void
		{
			target.addedToScene.remove(targetAddedToSceneHandler);
			for each (var position : Mesh in target.parent.get("/mesh[name='__position__']"))
				position.parent = null;
		}
	}
}