package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.skinning.SkinningStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.skinning.SkinningMethod;
	
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
		
		override public function fillRenderState(state	: RendererState, 
												 style	: StyleStack, 
												 local	: LocalData, 
												 world	: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, local, world);
			
			state.depthTest	= CompareMode.LESS;
			state.priority	= state.priority + .5;
			
			if (state.blending != Blending.NORMAL)
				state.priority -= .5;
			
			return true;
		}
		
		override protected function getOutputPosition() : SValue
		{
			var skinningMethod	: uint	= getStyleConstant(SkinningStyle.METHOD, SkinningMethod.DISABLED)
										  as uint;
			
			// handle skinning
			if (skinningMethod != SkinningMethod.DISABLED)
			{
				var maxInfluences	: uint		= getStyleConstant(SkinningStyle.MAX_INFLUENCES, 0)
												  as uint;
				var numBones		: uint		= getStyleConstant(SkinningStyle.NUM_BONES, 0)
												  as uint;
				var skinnedPosition	: SValue	= getVertexSkinnedPosition(skinningMethod,
																		   maxInfluences,
																		   numBones);
				
				return multiply4x4(skinnedPosition, localToScreenMatrix);
			}
			
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			var diffuse : SValue	= diffuseColor;
			
			if (styleIsSet(BasicStyle.DIFFUSE_MULTIPLIER))
				diffuse.scaleBy(copy(getStyleParameter(4, BasicStyle.DIFFUSE_MULTIPLIER)));
						
			return diffuse;
		}
		
		override protected function getDataHash(style	: StyleStack,
												local	: LocalData,
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
			
			if (style.get(SkinningStyle.METHOD, SkinningMethod.DISABLED) != SkinningMethod.DISABLED)
			{
				hash += "_skin(";
				hash += "method=" + style.get(SkinningStyle.METHOD);
				hash += ",maxInfluences=" + style.get(SkinningStyle.MAX_INFLUENCES, 0);
				hash += ",numBones=" + style.get(SkinningStyle.NUM_BONES, 0);
				hash += ")";
			}
			
			return hash;
		}
	}
}
