package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.geometry.primitive.CubeGeometry;
	import aerys.minko.render.geometry.stream.iterator.VertexIterator;
	import aerys.minko.render.geometry.stream.iterator.VertexReference;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.math.Vector4;
	
	public final class VertexNormalsDebugController extends AbstractController
	{
		private var _vertexNormalMaterial	: Material;
		
		public function VertexNormalsDebugController(vertexNormalMaterial : Material = null)
		{
			super(Mesh);
			
			initialize(vertexNormalMaterial);
		}
		
		private function initialize(vertexNormalMaterial : Material) : void
		{
			_vertexNormalMaterial = vertexNormalMaterial;
			if (!_vertexNormalMaterial)
			{
				_vertexNormalMaterial = new BasicMaterial({
					diffuseColor 	: 0xffff007f,
					blending		: Blending.ALPHA
				});
			}
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: VertexNormalsDebugController,
											target	: Mesh) : void
		{
			target.added.add(addedHandler);
		}
		
		private function addedHandler(target 	: Mesh,
									  ancestor	: Group) : void
		{
			if (!target.scene)
				return ;
			
			var vertices : VertexIterator = new VertexIterator(target.geometry.getVertexStream(0));
			
			for each (var vertex : VertexReference in vertices)
			{
				var normal : Mesh = new Mesh(
					CubeGeometry.cubeGeometry, _vertexNormalMaterial, '__normal__'
				);
				
				normal.transform
					.orientTo(
						new Vector4(vertex.nx, vertex.ny, vertex.nz),
						new Vector4(vertex.x, vertex.y, vertex.z)
					)
					.prependTranslation(0., 0., 0.05)
					.prependScale(0.01, 0.01, 0.1)
				
				target.parent.addChild(normal);
			}
		}
		
		private function targetRemovedHandler(ctrl		: VertexPositionsDebugController,
											  target	: Mesh) : void
		{
			target.added.remove(addedHandler);
			for each (var position : Mesh in target.parent.get("/mesh[name='__normal__']"))
				position.parent = null;
		}
	}
}