package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.animation.AnimationMethod;
	
	import flash.utils.Dictionary;

	public class VertexNormalColorEffect extends SinglePassEffect implements IRenderingEffect
	{
		private var _vertexNormal	: SValue	= null;
		
		override protected function getOutputPosition() : SValue
		{
			var animationShaderPart : AnimationShaderPart = new AnimationShaderPart();
			
			var animationMethod		: uint	= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED) as uint;
			var maxInfluences		: uint	= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0) as uint;
			var numBones			: uint	= getStyleConstant(AnimationStyle.NUM_BONES, 0) as uint;
			
			var animationPosition	: SValue	= animationShaderPart.getVertexPosition(animationMethod, maxInfluences, numBones);
			
			_vertexNormal	= animationShaderPart.getVertexNormal(animationMethod, maxInfluences, numBones);
			
			return multiply4x4(animationPosition, localToScreenMatrix);
		}
		
		override protected function getOutputColor() : SValue
		{
			return divide(add(1., interpolate(_vertexNormal)), 2.);
		}
		
		override protected function getDataHash(styleData		: StyleStack,
												transformData	: TransformData,
												worldData		: Dictionary) : String
		{
			var hash : String	= "vertexNormalColor";
			
			if (styleData.get(AnimationStyle.METHOD, AnimationMethod.DISABLED) != AnimationMethod.DISABLED)
			{
				hash += "_animation(";
				hash += "method=" + styleData.get(AnimationStyle.METHOD);
				hash += ",maxInfluences=" + styleData.get(AnimationStyle.MAX_INFLUENCES, 0);
				hash += ",numBones=" + styleData.get(AnimationStyle.NUM_BONES, 0);
				hash += ")";
			}
			
			return hash;
		}
	}
}