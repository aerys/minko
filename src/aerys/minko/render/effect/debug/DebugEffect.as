package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.type.animation.AnimationMethod;
	
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
			
			var animationShaderPart : AnimationShaderPart = new AnimationShaderPart();
			
			var animationMethod		: uint	= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED) as uint;
			var maxInfluences		: uint	= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0) as uint;
			var numBones			: uint	= getStyleConstant(AnimationStyle.NUM_BONES, 0) as uint;
			
			var animationPosition	: SValue	= animationShaderPart.getVertexPosition(animationMethod, maxInfluences, numBones);
			
			return multiply4x4(animationPosition, localToScreenMatrix);
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