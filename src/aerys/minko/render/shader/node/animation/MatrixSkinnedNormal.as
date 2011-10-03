package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply3x3;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	import aerys.minko.render.shader.node.operation.math.Sum;
	import aerys.minko.type.stream.format.VertexComponent;

	public class MatrixSkinnedNormal extends Dummy
	{
		public function MatrixSkinnedNormal(maxInfluences : uint, numBones : uint)
		{
			var inNormal	: INode = new Extract(new Attribute(VertexComponent.NORMAL), Components.XYZ);
			var outNormal	: INode	= null;

			if (maxInfluences == 0)
			{
				outNormal = inNormal;
			}
			else
			{
				inNormal = new Multiply3x3(inNormal, new StyleParameter(16, AnimationStyle.BIND_SHAPE));

				var skinningMatrices : StyleParameter = new StyleParameter(16 * numBones, AnimationStyle.BONE_MATRICES);

				if (maxInfluences == 1)
				{
					var singleJointAttr				: INode = new Attribute(VertexComponent.BONES[0]);
					var singleJointId				: INode = new Extract(singleJointAttr, Components.X);
					var singleJointSkinningMatrix	: INode = new VariadicExtract(singleJointId, skinningMatrices, 16);

					outNormal = new Multiply3x3(inNormal, singleJointSkinningMatrix);
				}
				else
				{
					outNormal = new Sum();
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						var jointAttr				: INode = new Attribute(VertexComponent.BONES[i]);

						var jointId					: INode = new Extract(jointAttr, Components.X);
						var jointWeight				: INode = new Extract(jointAttr, Components.Y);
						var jointSkinningMatrix		: INode = new VariadicExtract(jointId, skinningMatrices, 16);

						var jointOutNormal	: INode;
						jointOutNormal = new Multiply3x3(inNormal, jointSkinningMatrix);
						jointOutNormal = new Multiply(jointWeight, jointOutNormal);

						Sum(outNormal).addTerm(jointOutNormal);
					}
				}
			}

			super(outNormal);
		}
	}
}
