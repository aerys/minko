package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.stream.format.VertexComponent;

	public class AnimatedPosition extends Dummy
	{
		public function AnimatedPosition(skinningMethod	: uint,
										maxInfluences	: uint,
										numBones		: uint)
		{
			super(null);

			initialize(skinningMethod, maxInfluences, numBones);
		}

		private function initialize(animationMethod	: uint,
									maxInfluences	: uint,
									numBones		: uint) : void
		{
			switch (animationMethod)
			{
				case AnimationMethod.DISABLED :
					_node = new Attribute(VertexComponent.XYZ);
					break ;

				case AnimationMethod.MORPHING :
					_node = new MorphedPosition();
					break ;

				case AnimationMethod.MATRIX_SKINNING :
					_node = new MatrixSkinnedPosition(maxInfluences, numBones);
					break ;

				case AnimationMethod.DUAL_QUATERNION_SKINNING :
					_node = new DQSkinnedPosition(maxInfluences, numBones);
					break ;

				default :
					throw new Error('Unknown animation method.');
			}
		}
	}
}
