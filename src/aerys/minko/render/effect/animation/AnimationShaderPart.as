package aerys.minko.render.effect.animation
{
	import aerys.minko.render.shader.ActionScriptShaderPart;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.render.shader.node.animation.DQSkinnedNormal;
	import aerys.minko.render.shader.node.animation.DQSkinnedPosition;
	import aerys.minko.render.shader.node.animation.MatrixSkinnedNormal;
	import aerys.minko.render.shader.node.animation.MatrixSkinnedPosition;
	import aerys.minko.render.shader.node.animation.MorphedNormal;
	import aerys.minko.render.shader.node.animation.MorphedPosition;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.animation.AnimationMethod;
	
	import flash.utils.Dictionary;
	
	public class AnimationShaderPart extends ActionScriptShaderPart
	{
		
		public final function getVertexPosition(animationMethod	: uint,
												maxInfluences	: uint = 0,
												numBones		: uint = 0) : SValue
		{
			switch (animationMethod)
			{
				case AnimationMethod.DISABLED :
					return vertexPosition;
					
				case AnimationMethod.MORPHING :
					return new SValue(new MorphedPosition())
					
				case AnimationMethod.MATRIX_SKINNING:
					return new SValue(new MatrixSkinnedPosition(maxInfluences, numBones));
					
				case AnimationMethod.DUAL_QUATERNION_SKINNING:
					return new SValue(new DQSkinnedPosition(maxInfluences, numBones));
					
				default :
					throw new Error('Unknown animation method.');
			}
		}
		
		public final function getVertexNormal(animationMethod	: uint,
											  maxInfluences		: uint = 0,
											  numBones			: uint = 0) : SValue
		{
			switch (animationMethod)
			{
				case AnimationMethod.DISABLED :
					return vertexNormal;
					
				case AnimationMethod.MORPHING :
					return new SValue(new MorphedNormal())
					
				case AnimationMethod.MATRIX_SKINNING:
					return new SValue(new MatrixSkinnedNormal(maxInfluences, numBones));
					
				case AnimationMethod.DUAL_QUATERNION_SKINNING:
					return new SValue(new DQSkinnedNormal(maxInfluences, numBones));
					
				default :
					throw new Error('Unknown animation method.');
			}
		}
		
		override public function getDataHash(styleData		: StyleStack,
											 transformData	: TransformData,
											 worldData		: Dictionary) : String
		{
			var hash : String = "_animation";
			
			if (styleData.get(AnimationStyle.METHOD, AnimationMethod.DISABLED) != AnimationMethod.DISABLED)
			{
				hash += "(method=" + styleData.get(AnimationStyle.METHOD)
						+ ",maxInfluences=" + styleData.get(AnimationStyle.MAX_INFLUENCES, 0)
						+ ",numBones=" + styleData.get(AnimationStyle.NUM_BONES, 0)
						+ ")";
			}
			
			return hash;
		}
	}
}