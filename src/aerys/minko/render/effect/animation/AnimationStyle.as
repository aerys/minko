package aerys.minko.render.effect.animation
{
	import aerys.minko.render.effect.Style;

	public class AnimationStyle
	{
		public static const METHOD			: uint = Style.getStyleId('animation method');

		public static const MAX_INFLUENCES	: uint = Style.getStyleId('skinning maxInfluences');
		public static const NUM_BONES		: uint = Style.getStyleId('skinning numBones');

		public static const BONE_MATRICES	: uint = Style.getStyleId('skinning boneMatrices');
		public static const BIND_SHAPE		: uint = Style.getStyleId('skininig bsm');
		public static const BONE_QN			: uint = Style.getStyleId('skinning boneQn');
		public static const BONE_QD			: uint = Style.getStyleId('skinning boneQd');

		public static const MORPHING_RATIO	: uint = Style.getStyleId('morphing ratio');
	}
}
