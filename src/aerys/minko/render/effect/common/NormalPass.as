package aerys.minko.render.effect.common
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.WorldNormal;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class NormalPass implements IEffectPass
	{
		protected static const SHADER : Shader = 
			Shader.create(new ClipspacePosition(), new WorldNormal().interpolated);
		
		protected var _priority		: Number;
		protected var _renderTarget : RenderTarget;
		
		public function NormalPass(priority		: Number		= 0,
								   renderTarget : RenderTarget	= null)
		{
			_priority		= priority;
			_renderTarget	= renderTarget;
		}
		
		public function fillRenderState(state		: RendererState,
										styleStack	: StyleStack, 
										local		: LocalData, 
										world		: Dictionary) : Boolean
		{
			state.depthTest			= CompareMode.LESS;
			state.blending			= Blending.NORMAL;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget || (world[ViewportData] as ViewportData).renderTarget;
			state.shader			= SHADER.resource;
			state.triangleCulling	= styleStack.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			
			var triangleCullingModifier : Number = state.triangleCulling == TriangleCulling.BACK ? 1.0 : -1.0;
			
			styleStack.set(BasicStyle.NORMAL_MULTIPLIER, triangleCullingModifier);
			
			SHADER.fillRenderState(state, styleStack, local, world);
			
			return true;
		}
		
	}
}