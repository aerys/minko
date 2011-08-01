package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.utils.Dictionary;
	
	public class MeshAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.RENDER;
		
		private static var _instance	: MeshAction	= null;
		
		public static function get meshAction() : MeshAction
		{
			return _instance || (_instance = new MeshAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var mesh : IMesh	= scene as IMesh;
			
			if (!mesh)
				throw new Error();
			
			// invalidate world objects cache
			for each (var worldObject : IWorldData in visitor.worldData)
				worldObject.invalidate();
			
			// pass "ready to draw" data to the renderer.
			var localData		: LocalData			= visitor.localData;
			var worldData		: Dictionary		= visitor.worldData;
			var renderingData	: RenderingData		= visitor.renderingData;
			var effect			: IEffect			= renderingData.effect;
			
			if (!effect)
				throw new Error("Unable to draw without an effect.");
			
			var vertexStream	: IVertexStream			= mesh.vertexStream;
			var indexStream 	: IndexStream			= mesh.indexStream;
			var passes			: Vector.<IEffectPass>	= effect.getPasses(renderingData.styleStack,
															   	   		   localData,
																		   worldData);
			var numPasses 		: int 					= passes.length;
			
			for (var j : int = 0; j < numPasses; ++j)
			{
				renderer.begin();
				
				var pass	: IEffectPass	= passes[j];
				var state	: RendererState	= renderer.state;
				
				if (pass.fillRenderState(state, renderingData.styleStack, localData, worldData))
				{
					state.setInputStreams(vertexStream, indexStream);
					renderer.drawTriangles();
					
					renderer.end();
				}
				
			}
			
			return true;
		}
	}
}