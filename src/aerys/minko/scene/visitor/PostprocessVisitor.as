package aerys.minko.scene.visitor
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.node.mesh.primitive.QuadMesh;
	import aerys.minko.type.stream.iterator.VertexIterator;
	import aerys.minko.type.stream.iterator.VertexReference;
	
	import flash.utils.Dictionary;
	
	public class PostprocessVisitor extends RenderingVisitor
	{
		private static const QUAD : QuadMesh = createQuad();
		
		private static function createQuad() : QuadMesh
		{
			var quad 		: QuadMesh 			= new QuadMesh();
			var iterator 	: VertexIterator 	= new VertexIterator(quad.vertexStream);
			
			for each (var vertex : VertexReference in iterator)
			{
				vertex.x *= 2;
				vertex.y *= 2;
			}
			
			return quad;
		}
		
		public function PostprocessVisitor()
		{
		}
		
		override public function processSceneGraph(scene			: IScene, 
												   localData		: LocalData, 
												   worldData		: Dictionary, 
												   renderingData	: RenderingData, 
												   renderer			: IRenderer) : void
		{
			scene = QUAD;
			super.processSceneGraph(scene, localData, worldData, renderingData, renderer);
		}
	}
}
