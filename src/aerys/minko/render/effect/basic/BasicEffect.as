package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.CompareMode;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;
	
	[StyleParameter(name="basic diffuse map", type="texture")]
	[StyleParameter(name="basic diffuse multiplier", type="color")]
	
	public class BasicEffect extends SinglePassEffect implements IRenderingEffect
	{
		private static const BASIC_SHADER	: BasicShader	= new BasicShader();
		
		public function BasicEffect(priority		: Number		= 0,
								  	renderTarget	: RenderTarget	= null)
		{
			super(BASIC_SHADER, priority, renderTarget);
		}
		
		override public function fillRenderState(state		: RendererState, 
												 style		: StyleData, 
												 transform	: TransformData, 
												 world		: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, transform, world);
			
			state.priority	= state.priority + .5;
			
			if (state.blending != Blending.NORMAL)
				state.priority -= .5;
			
			return true;
		}
		
		
	}
}
