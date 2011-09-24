package aerys.minko.render.effect.common
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.CompareMode;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.WorldNormal;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class NormalPass implements IEffectPass
	{
		protected static const SHADER : Shader = Shader.create(new ClipspacePosition(),
															   new WorldNormal().interpolated);
		
		protected var _priority		: Number;
		protected var _renderTarget : RenderTarget;
		
		public function NormalPass(priority		: Number		= 0,
								   renderTarget : RenderTarget	= null)
		{
			_priority		= priority;
			_renderTarget	= renderTarget;
		}
		
		public function fillRenderState(state			: RendererState,
										styleData		: StyleData, 
										transformData	: TransformData, 
										worldData		: Dictionary) : Boolean
		{
			var triangleCulling		: uint		= styleData.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			var normalMultiplier	: Number	= triangleCulling == TriangleCulling.BACK ? 1.0 : -1.0;
			
			styleData.set(BasicStyle.NORMAL_MULTIPLIER, normalMultiplier);
			
			state.depthTest			= CompareMode.LESS;
			state.blending			= Blending.NORMAL;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget || (worldData[ViewportData] as ViewportData).renderTarget;
			state.triangleCulling	= triangleCulling
			
			SHADER.fillRenderState(state, styleData, transformData, worldData);
			
			return true;
		}
		
	}
}