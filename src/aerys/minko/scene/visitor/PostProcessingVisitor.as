package aerys.minko.scene.visitor
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.render.shader.PostProcessingActionScriptShader;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.node.mesh.primitive.QuadMesh;
	import aerys.minko.scene.node.texture.Texture;
	import aerys.minko.type.stream.iterator.VertexIterator;
	import aerys.minko.type.stream.iterator.VertexReference;
	
	import flash.utils.Dictionary;

	public final class PostProcessingVisitor extends RenderingVisitor
	{
		private var _scene	: Group	= new Group(
			new Group(),
			createFullscreenQuad()
		);

		private static function createFullscreenQuad() : QuadMesh
		{
			var quad 		: QuadMesh 			= new QuadMesh();
			var iterator 	: VertexIterator 	= new VertexIterator(quad.vertexStream);

			for each (var vertex : VertexReference in iterator)
			{
				vertex.x *= 2.;
				vertex.y *= 2.;
			}

			return quad;
		}

		override public function processSceneGraph(scene			: IScene,
												   transformData	: TransformData,
												   worldData		: Dictionary,
												   renderingData	: RenderingData,
												   renderer			: IRenderer) : void
		{
			var backBuffer : TextureResource	= (worldData[ViewportData] as ViewportData).renderTarget.textureResource;
			
			_scene.removeChildAt(0);
			_scene.addChildAt(new Texture(backBuffer, PostProcessingActionScriptShader.BACKBUFFER_STYLE_ID), 0);
			
			super.processSceneGraph(_scene, transformData, worldData, renderingData, renderer);
		}
	}
}
