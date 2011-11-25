package aerys.minko.render.effect.basic
{
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.math.Vector4;

	import flash.utils.Dictionary;

	public class BasicShader extends ActionScriptShader
	{
		private static const ANIMATION	: AnimationShaderPart	= new AnimationShaderPart();

		public function BasicShader()
		{
			super("basic");
		}
		
		override protected function getOutputPosition() : SValue
		{
			var animationMethod	: uint		= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED)
											  as uint;
			var maxInfluences	: uint		= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0)
											  as uint;
			var numBones		: uint		= getStyleConstant(AnimationStyle.NUM_BONES, 0)
											  as uint;
			var vertexPosition	: SValue	= ANIMATION.getVertexPosition(animationMethod, maxInfluences, numBones);

			return multiply4x4(vertexPosition, localToScreenMatrix);
		}

		override protected function getOutputColor() : SValue
		{
			var diffuse : SValue	= null;

			if (styleIsSet(BasicStyle.DIFFUSE))
			{
				var diffuseStyle	: Object 	= getStyleConstant(BasicStyle.DIFFUSE);

				if (diffuseStyle is uint || diffuseStyle is Vector4)
					diffuse = copy(getStyleParameter(4, BasicStyle.DIFFUSE));
				else if (diffuseStyle is TextureResource)
					diffuse = sampleTexture(BasicStyle.DIFFUSE, interpolate(vertexUV));
				else
					throw new Error('Invalid BasicStyle.DIFFUSE value.');
			}
			else
				diffuse = float4(interpolate(vertexRGBColor).rgb, 1.);

			diffuse.scaleBy(getStyleParameter(4, BasicStyle.DIFFUSE_MULTIPLIER,	0xffffffff));

			return diffuse;
		}

		override public function getDataHash(styleData		: StyleData,
											 transformData	: TransformData,
											 worldData		: Dictionary) : String
		{
			var hash 			: String	= "basic";
			var diffuseStyle 	: Object 	= styleData.isSet(BasicStyle.DIFFUSE)
											  ? styleData.get(BasicStyle.DIFFUSE)
											  : null;

			if (diffuseStyle == null)
				hash += '_colorFromVertex';
			else if (diffuseStyle is uint || diffuseStyle is Vector4)
				hash += '_colorFromConstant';
			else if (diffuseStyle is TextureResource)
				hash += '_colorFromTexture';
			else
				throw new Error('Invalid BasicStyle.DIFFUSE value');

			hash += ANIMATION.getDataHash(styleData, transformData, worldData)

			return hash;
		}
	}
}
