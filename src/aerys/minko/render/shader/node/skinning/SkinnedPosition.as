package aerys.minko.render.shader.node.skinning
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.type.skinning.SkinningMethod;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class SkinnedPosition extends Dummy
	{
		public function SkinnedPosition(skinningMethod	: uint,
										maxInfluences	: uint,
										numBones		: uint)
		{
			super(null);
			
			initialize(skinningMethod, maxInfluences, numBones);
		}
		
		private function initialize(skinningMethod	: uint,
									maxInfluences	: uint,
									numBones		: uint) : void
		{
			switch (skinningMethod)
			{
				case SkinningMethod.DISABLED :
					_node = new Attribute(VertexComponent.XYZ);
					break ;
				case SkinningMethod.MATRIX :
					_node = new MatrixSkinnedPosition(maxInfluences, numBones);
					break ;
				case SkinningMethod.DUAL_QUATERNION :
					_node = new DQSkinnedPosition(maxInfluences, numBones);
					break ;
				default :
					throw new Error('Unknown SkinningMethod.');
			}
		}
	}
}