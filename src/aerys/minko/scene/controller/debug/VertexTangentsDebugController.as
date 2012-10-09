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
	import aerys.minko.type.math.Vector4;
	
	public class VertexTangentsDebugController extends AbstractController
	{
		private var _vertexTangentMaterial : Material;
		
		public function VertexTangentsDebugController()
		{
			super(Mesh);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_vertexTangentMaterial = new BasicMaterial({
				diffuseColor 	: 0x00ff007f,
				blending		: Blending.ALPHA
			});
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: VertexTangentsDebugController,
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
				var normal : Mesh = new Mesh(
					CubeGeometry.cubeGeometry, _vertexTangentMaterial, '__tangent__'
				);
				
				normal.transform
					.orientTo(
						new Vector4(vertex.tx, vertex.ty, vertex.tz),
						new Vector4(vertex.x, vertex.y, vertex.z)
					)
					.prependTranslation(0., 0., 0.05)
					.prependScale(0.01, 0.01, 0.1)
				
				target.parent.addChild(normal);
			}
		}
		
		private function targetRemovedHandler(ctrl		: VertexTangentsDebugController,
											  target	: Mesh) : void
		{
			target.addedToScene.remove(targetAddedToSceneHandler);
			for each (var position : Mesh in target.parent.get("/mesh[name='__tangent__']"))
				position.parent = null;
		}
	}
}