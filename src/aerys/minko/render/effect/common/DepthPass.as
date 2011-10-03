package aerys.minko.render.effect.common
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.node.common.ClipspacePosition;
	import aerys.minko.render.shader.node.common.PackedDepth;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;

	import flash.utils.Dictionary;

	public class DepthPass implements IEffectPass
	{
		protected static var SHADER			: Shader		=
			Shader.create(new ClipspacePosition(), new PackedDepth());

		protected var _priority				: Number		= 0.;
		protected var _renderTarget			: RenderTarget	= null;

		public function DepthPass(priority		: Number		= 0,
								  renderTarget	: RenderTarget	= null)
		{
			_priority			= priority;
			_renderTarget		= renderTarget;
		}

		public function fillRenderState(state			: RendererState,
										styleData		: StyleData,
										transformData	: TransformData,
										worldData		: Dictionary) : Boolean
		{
			state.blending			= Blending.NORMAL;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget || worldData[ViewportData].renderTarget;
			state.program			= SHADER.resource;
			// state.triangleCulling	= styleStack.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.triangleCulling	= TriangleCulling.FRONT;

			SHADER.fillRenderState(state, styleData, transformData, worldData);

			return true;
		}
	}
}
