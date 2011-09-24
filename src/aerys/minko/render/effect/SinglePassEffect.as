package aerys.minko.render.effect
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.shader.IShader;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.CompareMode;
	import aerys.minko.type.enum.TriangleCulling;
	
	import flash.utils.Dictionary;
	
	public class SinglePassEffect implements IEffect, IEffectPass
	{
		private var _passes	 		: Vector.<IEffectPass>	= Vector.<IEffectPass>([this]);
		
		private var _shader			: IShader				= null
		private var _priority		: Number				= 0.;
		private var _renderTarget	: RenderTarget			= null;
		
		protected function get priority() 		: Number				{ return _priority; }
		protected function get renderTarget() 	: RenderTarget			{ return _renderTarget; }
		protected function get passes()			: Vector.<IEffectPass>	{ return _passes; }
		
		public function SinglePassEffect(shader			: IShader,
										 priority		: Number				= 0.,
										 renderTarget	: RenderTarget			= null)
		{
			_shader = shader;
			_priority = priority;
			_renderTarget = renderTarget;
		}
		
		public function getPasses(styleStack 	: StyleData,
								  transformData : TransformData,
								  worldData 	: Dictionary) : Vector.<IEffectPass>
		{
			return _passes;
		}
		
		public function fillRenderState(state			: RendererState, 
										styleData		: StyleData, 
										transformData	: TransformData, 
										worldData		: Dictionary)  :  Boolean
		{
			_shader.fillRenderState(state, styleData, transformData, worldData);
			
			state.depthTest			= CompareMode.LESS;
			state.blending			= styleData.get(BasicStyle.BLENDING, Blending.NORMAL) as uint;
			state.triangleCulling	= styleData.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.renderTarget		= _renderTarget || (worldData[ViewportData] as ViewportData).renderTarget;
			state.priority			= _priority;
			state.rectangle			= null;
			
			return true;
		}
	}
}