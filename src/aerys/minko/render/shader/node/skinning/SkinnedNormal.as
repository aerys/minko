package aerys.minko.render.shader.node.skinning
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.type.skinning.SkinningMethod;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class SkinnedNormal extends Dummy
	{
		public function SkinnedNormal(skinningMethod	: uint,
									  maxInfluences		: uint,
									  numBones			: uint)
		{
			super(null);
			
			initialize(skinningMethod, maxInfluences, numBones);
		}
		
		private function initialize(skinningMethod	: uint,
									maxInfluences	: uint,
									numBones		: uint) : void
		{
			var normal : INode = new Attribute(VertexComponent.NORMAL);
			
			/*switch (skinningMethod)
			{
				case SkinningMethod.DISABLED :
					normal = new Attribute(VertexComponent.NORMAL);
					break ;
				case SkinningMethod.MATRIX :
					normal = new MatrixSkinnedNormal(maxInfluences, numBones);
					break ;
				case SkinningMethod.DUAL_QUATERNION :
					normal = new DQSkinnedNormal(maxInfluences, numBones);
					break ;
				default :
					throw new Error('Unknown SkinningMethod.');
			}*/
			
			_node = normal;
		}
	}
}