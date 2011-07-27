package aerys.minko.render.effect.skinning
{
	import aerys.minko.render.effect.Style;

	public class SkinningStyle
	{
		public static const METHOD			: uint = Style.getStyleId('skinning_method');
		
		public static const MAX_INFLUENCES	: uint = Style.getStyleId('skinning_maxInfluences');
		public static const NUM_BONES		: uint = Style.getStyleId('skinning_numBones');
		
		public static const BONE_MATRICES	: uint = Style.getStyleId('skinning_boneMatrices');
		public static const BIND_SHAPE		: uint = Style.getStyleId('skininig_bsm');
		public static const BONE_QN			: uint = Style.getStyleId('skinning_boneQn');
		public static const BONE_QD			: uint = Style.getStyleId('skinning_boneQd');
	}
}
