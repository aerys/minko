package aerys.minko.effect.common
{
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.effect.basic.BasicStyle;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.DynamicShader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.WorldNormal;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;
	
	public class NormalPass implements IEffectPass
	{
		protected static const SHADER : DynamicShader = 
			DynamicShader.create(new ClipspacePosition(), new WorldNormal().interpolated);
		
		protected var _priority		: Number;
		protected var _renderTarget : RenderTarget;
		
		public function NormalPass(priority		: Number		= 0,
								   renderTarget : RenderTarget	= null)
		{
			_priority		= priority;
			_renderTarget	= renderTarget;
		}
		
		public function fillRenderState(state		: RenderState,
										styleStack	: StyleStack, 
										local		: TransformData, 
										world		: Dictionary) : Boolean
		{
			state.blending			= Blending.NORMAL;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget;
			state.shader			= SHADER;
			state.triangleCulling	= styleStack.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			
			var triangleCullingModifier : Number = state.triangleCulling == TriangleCulling.BACK ? 1.0 : -1.0;
			styleStack.set(BasicStyle.TRIANGLE_CULLING_MULTIPLIER, triangleCullingModifier)
			
			SHADER.setConstants(styleStack, local, world, state);
			SHADER.setTextures(styleStack, local, world, state);
			return true;
		}
		
	}
}