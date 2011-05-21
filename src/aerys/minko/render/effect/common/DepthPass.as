package aerys.minko.render.effect.common
{
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.DynamicShader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.PackedDepth;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;
	
	public class DepthPass implements IEffectPass
	{
		protected static var SHADER : DynamicShader = 
			DynamicShader.create(new ClipspacePosition(), new PackedDepth());
		
		protected var _priority				: Number;
		protected var _renderTarget			: RenderTarget;
		
		public function DepthPass(priority		: Number		= 0,
										renderTarget	: RenderTarget	= null)
		{
			_priority			= priority;
			_renderTarget		= renderTarget;
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
			
			SHADER.fillRenderState(state, styleStack, local, world);
			
			return true;
		}
		
	}
}