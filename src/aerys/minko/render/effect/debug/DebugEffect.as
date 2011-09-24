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

	public class DebugEffect extends SinglePassEffect implements IRenderingEffect
	{
		private static const ANIMATION		: AnimationShaderPart	= new AnimationShaderPart();
		
		private const COLOR			: SValue	= float4(.5, .5, .5, 1.);
		
		private var _vertexColor	: SValue	= null;
		
		override protected function getOutputPosition() : SValue
		{
			var animationMethod	: uint		= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED) as uint;
			var maxInfluences	: uint		= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0) as uint;
			var numBones		: uint		= getStyleConstant(AnimationStyle.NUM_BONES, 0) as uint;
			var vertexPosition	: SValue	= ANIMATION.getVertexPosition(animationMethod, maxInfluences, numBones);
			var vertexNormal	: SValue	= ANIMATION.getVertexNormal(animationMethod, maxInfluences, numBones);
			var lightDir		: SValue	= subtract(cameraLocalPosition, vertexPosition);
			
			lightDir.normalize();
			
			// compute vertex color to be interpolated by the fragment shader
			_vertexColor = vertexNormal.dotProduct3(lightDir);
			_vertexColor = float4(multiply(_vertexColor, COLOR.rgb), COLOR.a);
			
			return multiply4x4(vertexPosition, localToScreenMatrix);
		}
		
		override protected function getOutputColor() : SValue
		{
			return interpolate(_vertexColor);
		}
		
		override public function getDataHash(styleData		: StyleStack,
											 transformData	: TransformData,
											worldData		: Dictionary) : String
		{
			return "debug" + ANIMATION.getDataHash(styleData, transformData, worldData);
		}
		
	}
}