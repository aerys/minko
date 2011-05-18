package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.DynamicShader;
	import aerys.minko.render.shader.node.operation.common.ClipspacePosition;
	import aerys.minko.render.shader.node.operation.common.DiffuseMapTexture;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;
	
	public class BasicPass implements IEffectPass
	{
		protected static const SHADER	: DynamicShader = 
			DynamicShader.create(new ClipspacePosition(), new DiffuseMapTexture());
		
		protected var _priority			: Number;
		protected var _renderTarget		: RenderTarget;
		
		public function BasicPass(priority		: Number		= 0,
								  renderTarget	: RenderTarget	= null)
		{
			_priority		= priority;
			_renderTarget	= renderTarget;
		}
		
		public function fillRenderState(state		: RenderState, 
										styleStack	: StyleStack, 
										local		: TransformData, 
										world		: Dictionary) : Boolean
		{
			state.blending			= styleStack.get(BasicStyle.BLENDING, Blending.ALPHA) as uint;
			state.triangleCulling	= styleStack.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget;
			state.shader			= SHADER;
			
			SHADER.setConstants(styleStack, local, world, state);
			SHADER.setTextures(styleStack, local, world, state);
			
			return true;
		}
	}
}