package aerys.minko.render.effect.common
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.PackedDepth;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class DepthPass implements IEffectPass
	{
		protected static var SHADER : Shader = 
			Shader.create(new ClipspacePosition(), new PackedDepth());
		
		protected var _priority				: Number;
		protected var _renderTarget			: RenderTarget;
		
		public function DepthPass(priority		: Number		= 0,
								  renderTarget	: RenderTarget	= null)
		{
			_priority			= priority;
			_renderTarget		= renderTarget;
		}
		
		public function fillRenderState(state		: RendererState,
										styleStack	: StyleStack, 
										local		: TransformData,
										world		: Dictionary) : Boolean
		{
			state.blending			= Blending.NORMAL;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget || world[ViewportData].renderTarget;
			state.program			= SHADER.resource;
//			state.triangleCulling	= styleStack.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.triangleCulling	= TriangleCulling.FRONT;
			
			SHADER.fillRenderState(state, styleStack, local, world);
			
			return true;
		}
		
	}
}