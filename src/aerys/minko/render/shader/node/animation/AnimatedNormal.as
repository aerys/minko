package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.stream.format.VertexComponent;

	public class AnimatedNormal extends Dummy
	{
		public function AnimatedNormal(skinningMethod	: uint,
									  maxInfluences		: uint,
									  numBones			: uint)
		{
			super(null);

			initialize(skinningMethod, maxInfluences, numBones);
		}

		private function initialize(animationMethod	: uint,
									maxInfluences	: uint	= 0,
									numBones		: uint	= 0) : void
		{
			var normal : INode = new Attribute(VertexComponent.NORMAL);

			switch (animationMethod)
			{
				case AnimationMethod.DISABLED :
					normal = new Attribute(VertexComponent.NORMAL);
					break ;

				case AnimationMethod.MORPHING :
					normal = new MorphedNormal();
					break ;

				case AnimationMethod.MATRIX_SKINNING :
					normal = new MatrixSkinnedNormal(maxInfluences, numBones);
					break ;

				case AnimationMethod.DUAL_QUATERNION_SKINNING :
					normal = new DQSkinnedNormal(maxInfluences, numBones);
					break ;

				default :
					throw new Error('Unknown animation method.');
			}

			_node = normal;
		}
	}
}
