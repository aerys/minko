package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.skinning.SkinningStyle;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.skinning.SkinningMethod;
	
	import flash.utils.Dictionary;

	public class DebugEffect extends SinglePassEffect
	{
		private const COLOR			: SValue	= float4(.5, .5, .5, 1.);
		
		private var _vertexColor	: SValue	= null;
		
		override protected function getOutputPosition() : SValue
		{
			// compute vertex color to be interpolated by the fragment shader
			var lightDir	: SValue	= subtract(cameraLocalPosition, vertexPosition);
			
			lightDir.normalize();
			
			_vertexColor = vertexNormal.dotProduct3(lightDir);
			_vertexColor = float4(multiply(_vertexColor, COLOR.rgb), COLOR.a);
			
			// perform skinning
			var skinningMethod	: uint	= getStyleConstant(SkinningStyle.METHOD, SkinningMethod.DISABLED)
									      as uint;
			
			// handle skinning
			if (skinningMethod != SkinningMethod.DISABLED)
			{
				var maxInfluences	: uint		= getStyleConstant(SkinningStyle.MAX_INFLUENCES, 0) as uint;
				var numBones		: uint		= getStyleConstant(SkinningStyle.NUM_BONES, 0) as uint;
				var skinnedPosition	: SValue	= getVertexSkinnedPosition(skinningMethod, maxInfluences, numBones);
				
				return multiply4x4(skinnedPosition, localToScreenMatrix);
			}
			
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			return interpolate(_vertexColor);
		}
		
		override protected function getDataHash(style	: StyleStack,
												local	: LocalData,
												world	: Dictionary) : String
		{
			var hash 			: String	= "debug";
			
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