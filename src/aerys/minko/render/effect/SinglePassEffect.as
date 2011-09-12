package aerys.minko.render.effect
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class SinglePassEffect extends ActionScriptShader implements IEffect, IEffectPass
	{
		private var _passes	 		: Vector.<IEffectPass>	= Vector.<IEffectPass>([this]);
		private var _priority		: Number				= 0.;
		private var _renderTarget	: RenderTarget			= null;
		
		protected function get priority() 		: Number				{ return _priority; }
		protected function get renderTarget() 	: RenderTarget			{ return _renderTarget; }
		protected function get passes()			: Vector.<IEffectPass>	{ return _passes; }
		
		public function SinglePassEffect(priority		: Number		= 0.,
										 renderTarget	: RenderTarget	= null)
		{
			_priority = priority;
			_renderTarget = renderTarget;
		}
		
		public function getPasses(styleStack 	: StyleStack,
								  local 		: TransformData,
								  world 		: Dictionary) : Vector.<IEffectPass>
		{
			return _passes;
		}
		
		override public function fillRenderState(state		: RendererState, 
												 style		: StyleStack, 
												 transform	: TransformData, 
												 world		: Dictionary)  :  Boolean
		{
			super.fillRenderState(state, style, transform, world);
			
			state.depthTest			= CompareMode.LESS;
			state.blending			= style.get(BasicStyle.BLENDING, Blending.NORMAL) as uint;
			state.triangleCulling	= style.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.renderTarget		= _renderTarget || (world[ViewportData] as ViewportData).renderTarget;
			state.priority			= _priority;
			state.rectangle			= null;
			
			return true;
		}
	}
}