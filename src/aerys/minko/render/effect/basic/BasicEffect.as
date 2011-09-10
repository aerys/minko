package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;
	
	[StyleParameter(name="basic diffuse map",type="texture")]
	[StyleParameter(name="basic diffuse multiplier",type="color")]
	
	public class BasicEffect extends SinglePassEffect
	{
		public function BasicEffect(priority		: Number		= 0,
								  	renderTarget	: RenderTarget	= null)
		{
			super(priority, renderTarget);
		}
		
		override public function fillRenderState(state		: RendererState, 
												 style		: StyleStack, 
												 transform	: TransformData, 
												 world		: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, transform, world);
			
			state.depthTest	= CompareMode.LESS;
			state.priority	= state.priority + .5;
			
			if (state.blending != Blending.NORMAL)
				state.priority -= .5;
			
			return true;
		}
		
		override protected function getOutputPosition() : SValue
		{
			var animationShaderPart : AnimationShaderPart = new AnimationShaderPart();
			
			var animationMethod		: uint	= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED) as uint;
			var maxInfluences		: uint	= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0) as uint;
			var numBones			: uint	= getStyleConstant(AnimationStyle.NUM_BONES, 0) as uint;
			
			var animationPosition	: SValue	= animationShaderPart.getVertexPosition(animationMethod, maxInfluences, numBones);
			
			return multiply4x4(animationPosition, localToScreenMatrix);
		}
		
		override protected function getOutputColor() : SValue
		{
			var diffuse : SValue	= diffuseColor;
			
			if (styleIsSet(BasicStyle.DIFFUSE_MULTIPLIER))
				diffuse.scaleBy(copy(getStyleParameter(4, BasicStyle.DIFFUSE_MULTIPLIER)));
						
			return diffuse;
		}
		
		override protected function getDataHash(style	: StyleStack,
												transform	: TransformData,
												world	: Dictionary) : String
		{
			var hash 			: String	= "basic";
			var diffuseStyle 	: Object 	= style.isSet(BasicStyle.DIFFUSE)
											  ? style.get(BasicStyle.DIFFUSE)
											  : null;
			
			if (diffuseStyle == null)
				hash += '_colorFromVertex';
			else if (diffuseStyle is uint || diffuseStyle is Vector4)
				hash += '_colorFromConstant';
			else if (diffuseStyle is TextureResource)
				hash += '_colorFromTexture';
			else
				throw new Error('Invalid BasicStyle.DIFFUSE value');
			
			if (style.isSet(BasicStyle.DIFFUSE_MULTIPLIER))
				hash += "_diffuseMultiplier";
			
			if (style.get(AnimationStyle.METHOD, AnimationMethod.DISABLED) != AnimationMethod.DISABLED)
			{
				hash += "_animation(";
				hash += "method=" + style.get(AnimationStyle.METHOD);
				hash += ",maxInfluences=" + style.get(AnimationStyle.MAX_INFLUENCES, 0);
				hash += ",numBones=" + style.get(AnimationStyle.NUM_BONES, 0);
				hash += ")";
			}
			
			return hash;
		}
	}
}
