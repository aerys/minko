package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.fog.FogStyle;
	import aerys.minko.render.effect.skinning.SkinningStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.skinning.SkinningMethod;
	
	import flash.utils.Dictionary;
	
	[StyleParameter(name="basic diffuse map",type="texture")]
	[StyleParameter(name="basic diffuse multiplier",type="color")]
	
	[StyleParameter(name="fog enabled",type="boolean")]
	[StyleParameter(name="fog color",type="color")]
	[StyleParameter(name="fog start",type="number")]
	[StyleParameter(name="fog distance",type="number")]

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
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			var diffuse : SValue	= diffuseColor;
			
			if (styleIsSet(BasicStyle.DIFFUSE_MULTIPLIER))
			{
				var diffuseMultiplier : SValue	= getStyleParameter(4, BasicStyle.DIFFUSE_MULTIPLIER);
				
				diffuse.scaleBy(copy(diffuseMultiplier));
			}
			
			// fog
			if (getStyleConstant(FogStyle.FOG_ENABLED, false))
			{
				var zFar		: SValue = styleIsSet(FogStyle.DISTANCE)
										  ? getStyleParameter(1, FogStyle.DISTANCE)
										  : getWorldParameter(1, CameraData, CameraData.Z_FAR);
				var fogColor 	: SValue = styleIsSet(FogStyle.COLOR)
										  ? getStyleParameter(3, FogStyle.COLOR)
										  : float3(0., 0., 0.);
				var fogStart	: SValue = styleIsSet(FogStyle.START)
										  ? getStyleParameter(1, FogStyle.START)
										  : float(0.);
				
				fogColor = getFogColor(fogStart, zFar, fogColor); 
				diffuse  = blend(fogColor, diffuse, Blending.ALPHA);
			}
			
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
			else if (diffuseStyle is TextureRessource)
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
			
			if (style.get(FogStyle.FOG_ENABLED, false))
			{
				hash += "_fog(";
				hash += "start=" + style.get(FogStyle.START, 0.);
				hash += ",distance=" + style.get(FogStyle.DISTANCE, 0.);
				hash += ",color=" + style.get(FogStyle.COLOR, 0);
				hash += ")"
			}
			
			return hash;
		}
	}
}
