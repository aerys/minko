package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.CrossProduct;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.ReciprocalRoot;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.RootWrapper;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	import aerys.minko.render.shader.node.operation.math.Sum;
	import aerys.minko.type.stream.format.VertexComponent;

	public class DQSkinnedNormal extends Dummy
	{
		public function DQSkinnedNormal(maxInfluences : uint, numBones : uint)
		{
			var inNormal	: INode = new Extract(new Attribute(VertexComponent.NORMAL), Components.XYZ);
			var outNormal	: INode	= null;

			if (maxInfluences == 0)
			{
				outNormal = inNormal; // new Combine(inNormal, new Constant(1.0));
			}
			else
			{
				var dQnList : StyleParameter = new StyleParameter(4 * numBones, AnimationStyle.BONE_QN);
				var dQdList : StyleParameter = new StyleParameter(4 * numBones, AnimationStyle.BONE_QD);

				var dQn : INode;
				var dQd : INode;

				if (maxInfluences == 1)
				{
					var singleJointAttr			: INode = new Attribute(VertexComponent.BONES[0]);
					var singleJointId			: INode = new Extract(singleJointAttr, Components.X);

					dQn = new RootWrapper(new VariadicExtract(singleJointId, dQnList, 4));
					dQd = new VariadicExtract(singleJointId, dQdList, 4);
				}
				else
				{
					dQn = new Sum();
					dQd	= new Sum();

					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						var jointAttr	: INode = new Attribute(VertexComponent.BONES[i]);

						var jointId		: INode = new Extract(jointAttr, Components.X);
						var jointWeight	: INode = new Extract(jointAttr, Components.Y);

						var jointDQn	: INode = new VariadicExtract(jointId, dQnList, 4);
						var jointDQd	: INode = new VariadicExtract(jointId, dQdList, 4);

						Sum(dQn).addTerm(new Multiply(jointWeight, jointDQn));
						Sum(dQd).addTerm(new Multiply(jointWeight, jointDQd));
					}
				}

				var dQnInvLength : INode = new ReciprocalRoot(new DotProduct4(dQn, dQn));

				dQn = new Multiply(dQn, dQnInvLength);
				dQd = new Multiply(dQd, dQnInvLength);

				var constant2		: INode = new Constant(2.0);
				var dQnX			: INode	= new RootWrapper(new Extract(dQn, Components.W));
				var dQnYZW			: INode	= new RootWrapper(new Extract(dQn, Components.XYZ));
				var dQdX			: INode	= new RootWrapper(new Extract(dQd, Components.W));
				var dQdYZW			: INode	= new RootWrapper(new Extract(dQd, Components.XYZ));

				var tmp0			: INode = new Multiply(dQnX, inNormal);			//	tmp0 = blendDQ[0].x*IN.position.xyz
				var tmp1			: INode = new CrossProduct(dQnYZW, inNormal);	//	tmp1 = cross(blendDQ[0].yzw, IN.position.xyz)
				var tmp2			: INode = new Add(tmp1, tmp0);					//	tmp2 = tmp1 + tmp0
				var tmp3			: INode = new CrossProduct(dQnYZW, tmp2);		//	tmp3 = cross(blendDQ[0].yzw, tmp2)
				var tmp4			: INode = new Multiply(constant2, tmp3);		//	tmp4 = 2.0*tmp3
				var tmp5			: INode = new Add(inNormal, tmp4);				//	tmp5 = IN.position.xyz + tmp4
				var tmp6			: INode = new Multiply(dQnX, dQdYZW);			//	tmp6 = blendDQ[0].x*blendDQ[1].yzw
				var tmp7			: INode = new Multiply(dQdX, dQnYZW);			//	tmp7 = blendDQ[1].x*blendDQ[0].yzw
				var tmp8			: INode = new CrossProduct(dQnYZW, dQdYZW);		//	tmp8 = cross(blendDQ[0].yzw, blendDQ[1].yzw)
				var tmp9			: INode = new Substract(tmp6, tmp7);			//	tmp9 = tmp6 - tmp7
				var tmp10			: INode = new Add(tmp9, tmp8);					//	tmp10 = tmp9 + tmp8
				var tmp11			: INode = new Multiply(constant2, tmp10);		//	tmp11 = 2.0*(tmp10)
				outNormal			= new Add(tmp5, tmp11);							//	position = tmp5 + tmp11

				// outNormal		= new Combine(outNormal, new Constant(1.0));
			}

			super(new Normalize(outNormal));
		}
	}
}
