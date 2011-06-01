package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.render.renderer.state.RenderState;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.RenderingData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
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
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var mesh : IMesh	= scene as IMesh;
			
			if (!mesh)
				throw new Error();
			
			// invalidate world objects cache
			for each (var worldObject : IWorldData in visitor.worldData)
				worldObject.invalidate();
			
			// pass "ready to draw" data to the renderer.
			var localData			: LocalData			= visitor.localData;
			var worldData			: Dictionary		= visitor.worldData;
			var renderingData		: RenderingData		= visitor.renderingData;
			var vertexStreamList 	: VertexStreamList	= mesh.vertexStreamList;
			var indexStream 		: IndexStream		= mesh.indexStream;
			var effects				: Vector.<IEffect>	= renderingData.effects;
			var numEffects 			: int 				= effects.length;
			
			if (numEffects == 0)
				throw new Error("Unable to draw without an effect.");
			
			for (var i : int = 0; i < numEffects; ++i)
			{
				var fx			: IEffect				= effects[i];
				var passes		: Vector.<IEffectPass>	= fx.getPasses(renderingData.styleStack,
																	   localData,
																	   worldData);
				var numPasses 	: int 					= passes.length;
				
				for (var j : int = 0; j < numPasses; ++j)
				{
					renderer.begin();
					
					var pass	: IEffectPass = passes[j];
					var state	: RenderState = renderer.state;
					
					if (pass.fillRenderState(state, renderingData.styleStack, localData, worldData))
					{
						state.setInputStreams(vertexStreamList, indexStream);
						renderer.drawTriangles();
					}
					
					renderer.end();
				}
			}
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
	}
}