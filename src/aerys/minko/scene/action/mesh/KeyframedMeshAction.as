package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.IWorldData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.RenderingData;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.KeyframedMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.utils.Dictionary;
	
	public final class KeyframedMeshAction implements IAction
	{
		private static const TYPE			: uint		= ActionType.RENDER;
		private static const EMPTY_STYLE	: Style		= new Style();
		
		private static var _instance : KeyframedMeshAction	= null;
		
		public static function get keyframedMeshAction() : KeyframedMeshAction
		{
			return _instance || (_instance = new KeyframedMeshAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene		: IScene, 
							visitor		: ISceneVisitor, 
							renderer	: IRenderer) : Boolean
		{
			var mesh : KeyframedMesh = scene as KeyframedMesh;
			
			if (!mesh)
				throw new Error();
			
			var currentFrame	: IVertexStream	= mesh.currentFrameVertexStream;
			var nextFrame		: IVertexStream	= mesh.nextFrameVertexStream;
			
			// invalidate world objects cache
			for each (var worldObject : IWorldData in visitor.worldData)
				worldObject.invalidate();
			
			// pass "ready to draw" data to the renderer.
			var transformData		: TransformData			= visitor.transformData;
			var worldData		: Dictionary		= visitor.worldData;
			var renderingData	: RenderingData		= visitor.renderingData;
			var styleStack		: StyleData		= renderingData.styleStack;
			var effectStack		: Vector.<IEffect>	= renderingData.effects;
			var effect			: IEffect			= effectStack[int(effectStack.length - 1)];
			
			if (!effect)
				throw new Error("Unable to draw without an effect.");
			
			var indexStream 	: IndexStream				= mesh.indexStream;
			var passes			: Vector.<IEffectPass>		= effect.getPasses(renderingData.styleStack, transformData, worldData);
			var numPasses 		: int 						= passes.length;
			
			var frameId			: Number					= mesh.frameId;
			
			styleStack.push(EMPTY_STYLE);
			styleStack.set(AnimationStyle.MORPHING_RATIO, frameId - (frameId << 0));
			
			for (var j : int = 0; j < numPasses; ++j)
			{
				var pass	: IEffectPass	= passes[j];
				var state	: RendererState	= RendererState.create(true);
				
				if (pass.fillRenderState(state, renderingData.styleStack, transformData, worldData))
				{
					state.indexStream = indexStream;
					state.setVertexStreamAt(currentFrame, 0);
					state.setVertexStreamAt(nextFrame, 1);

					renderer.pushState(state);
					renderer.drawTriangles();
				}
			}
			
			styleStack.pop();
			
			return true;
		}
	}
}