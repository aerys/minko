package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	import aerys.minko.render.shader.node.operation.math.Sum;
	import aerys.minko.type.stream.format.VertexComponent;

	public class MatrixSkinnedPosition extends Dummy
	{
		public function MatrixSkinnedPosition(maxInfluences : uint, numBones : uint)
		{
			var inVertexPosition	: INode = new Attribute(VertexComponent.XYZ);
			var outVertexPosition	: INode = null;

			if (maxInfluences == 0)
			{
				outVertexPosition = inVertexPosition;
			}
			else
			{
				inVertexPosition = new Multiply4x4(inVertexPosition, new StyleParameter(16, AnimationStyle.BIND_SHAPE));

				var skinningMatrices : StyleParameter = new StyleParameter(16 * numBones, AnimationStyle.BONE_MATRICES);

				if (maxInfluences == 1)
				{
					var singleJointAttr				: INode = new Attribute(VertexComponent.BONES[0]);
					var singleJointId				: INode = new Extract(singleJointAttr, Components.X);
					var singleJointSkinningMatrix	: INode = new VariadicExtract(singleJointId, skinningMatrices, 16);

					outVertexPosition = new Multiply4x4(inVertexPosition, singleJointSkinningMatrix);
				}
				else
				{
					outVertexPosition = new Sum();
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						var jointAttr				: INode = new Attribute(VertexComponent.BONES[i]);

						var jointId					: INode = new Extract(jointAttr, Components.X);
						var jointWeight				: INode = new Extract(jointAttr, Components.Y);
						var jointSkinningMatrix		: INode = new VariadicExtract(jointId, skinningMatrices, 16);

						var jointOutVertexPosition	: INode;
						jointOutVertexPosition = new Multiply4x4(inVertexPosition, jointSkinningMatrix);
						jointOutVertexPosition = new Multiply(jointWeight, jointOutVertexPosition);

						Sum(outVertexPosition).addTerm(jointOutVertexPosition);
					}
				}
			}

			super(outVertexPosition);
		}
	}
}
