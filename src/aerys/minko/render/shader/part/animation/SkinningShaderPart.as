package aerys.minko.render.shader.part.animation
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.Dictionary;

	/**
	 * The shader part to handle skinning vertex animations.
	 * 
	 * @author Romain Gilliotte
	 */
	public class SkinningShaderPart extends ShaderPart
	{
		public function SkinningShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function skinPosition(inPosition : SFloat) : SFloat
		{
			var skinningMethod : uint = uint(meshBindings.getConstant('skinningMethod'));
			
			switch (skinningMethod)
			{
				case SkinningMethod.MATRIX:
					return matrixSkinPosition(inPosition);
				
				case SkinningMethod.DUAL_QUATERNION:
					return dualQuaternionSkinPosition(inPosition);
				
				case SkinningMethod.DUAL_QUATERNION_SCALE:
					return dualQuaternionScaleSkinPosition(inPosition);
				
				default:
					throw new Error('Unknown animation method.');
			}
		}
		
		public function skinNormal(inNormal : SFloat) : SFloat
		{
			var skinningMethod : uint = uint(meshBindings.getConstant('skinningMethod'));
			
			switch (skinningMethod)
			{
				case SkinningMethod.MATRIX:
					return matrixSkinNormal(inNormal);
					
				case SkinningMethod.DUAL_QUATERNION:
					return dualQuaternionSkinNormal(inNormal);

				case SkinningMethod.DUAL_QUATERNION_SCALE:
					return dualQuaternionScaleSkinNormal(inNormal);
					
				default :
					throw new Error('Unknown animation method.');
			}
		}
		
		private function matrixSkinPosition(inVertexPosition : SFloat) : SFloat
		{
			var numBones			: uint		= uint(meshBindings.getConstant('skinningNumBones'));
			var maxInfluences		: uint		= uint(meshBindings.getConstant('skinningMaxInfluences'));
			var outVertexPosition	: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				outVertexPosition = inVertexPosition;
			}
			else
			{
				var bindShape			: SFloat = meshBindings.getParameter('skinningBindShape', 16);
				var skinningMatrices	: SFloat = meshBindings.getParameter('skinningMatrices', 16 * numBones);
				
				inVertexPosition = multiply4x4(inVertexPosition, bindShape);
				
				var jointAttr			: SFloat;
				var jointId				: SFloat;
				var jointWeight			: SFloat;
				var jointSkinningMatrix	: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr			= getVertexAttribute(VertexComponent.BONES[0]);
					jointId				= multiply(4, jointAttr.x);
					jointSkinningMatrix	= getFieldFromArray(jointId, skinningMatrices, true);
					
					outVertexPosition = multiply4x4(inVertexPosition, jointSkinningMatrix);
				}
				else
				{
					outVertexPosition = float4(0, 0, 0, 0);
					
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						jointAttr			= getVertexAttribute(VertexComponent.BONES[i]);
						jointId				= multiply(4, jointAttr.x);
						jointWeight			= jointAttr.y;
						jointSkinningMatrix	= getFieldFromArray(jointId, skinningMatrices, true);
						
						var jointOutVertexPosition	: SFloat;
						jointOutVertexPosition = multiply4x4(inVertexPosition, jointSkinningMatrix);
						jointOutVertexPosition = multiply(jointWeight, jointOutVertexPosition);
						
						outVertexPosition.incrementBy(jointOutVertexPosition);
					}
				}
			}
			
			return outVertexPosition;
		}
		
		private function matrixSkinNormal(inVertexNormal : SFloat) : SFloat
		{
			inVertexNormal = inVertexNormal.xyz;
			
			var numBones		: uint		= uint(meshBindings.getConstant('skinningNumBones'));
			var maxInfluences	: uint		= uint(meshBindings.getConstant('skinningMaxInfluences'));
			var outVertexNormal	: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				outVertexNormal = inVertexNormal;
			}
			else
			{
				var bindShape			: SFloat = meshBindings.getParameter('skinningBindShape', 16);
				var skinningMatrices	: SFloat = meshBindings.getParameter('skinningMatrices', 16 * numBones);
				
				inVertexNormal = multiply3x3(inVertexNormal, bindShape);
				
				var jointAttr			: SFloat;
				var jointId				: SFloat;
				var jointWeight			: SFloat;
				var jointSkinningMatrix	: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr = getVertexAttribute(VertexComponent.BONES[0]);
					jointId = jointAttr.x;
					jointSkinningMatrix = getFieldFromArray(jointId, skinningMatrices, true);
					
					outVertexNormal = multiply3x3(inVertexNormal, jointSkinningMatrix);
				}
				else
				{
					outVertexNormal = float4(0, 0, 0, 0);
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						jointAttr			= getVertexAttribute(VertexComponent.BONES[i]);
						jointId				= jointAttr.x;
						jointWeight			= jointAttr.y;
						jointSkinningMatrix	= getFieldFromArray(jointId, skinningMatrices, true);
						
						var jointOutNormal	: SFloat;
						jointOutNormal = multiply3x3(inVertexNormal, jointSkinningMatrix);
						jointOutNormal = multiply(jointWeight, jointOutNormal);
						
						outVertexNormal.incrementBy(jointOutNormal);
					}
				}
			}
			
			return outVertexNormal;
		}
		
		
		private function dualQuaternionSkinPosition(inPosition : SFloat) : SFloat
		{
			var numBones		: uint	= uint(meshBindings.getConstant('skinningNumBones'));
			var maxInfluences	: uint	= uint(meshBindings.getConstant('skinningMaxInfluences'));
			
			var outVertexPosition	: SFloat	= null;
			var result				: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				result = inPosition;
			}
			else
			{
				var bindShape	: SFloat = meshBindings.getParameter('skinningBindShape', 16);
				var dQnList		: SFloat = meshBindings.getParameter('skinningDQn', 4 * numBones);
				var dQdList		: SFloat = meshBindings.getParameter('skinningDQd', 4 * numBones);
				
				inPosition = multiply4x4(inPosition, bindShape);
				inPosition = inPosition.xyz;
				
				var jointAttr	: SFloat;
				var jointId		: SFloat;
				var jointWeight	: SFloat;
				var dQn			: SFloat;
				var dQd			: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr	= getVertexAttribute(VertexComponent.BONES[0]);
					jointId		= jointAttr.x;
					
					dQn = getFieldFromArray(jointId, dQnList, false);
					dQd = getFieldFromArray(jointId, dQdList, false);
				}
				else
				{
					dQn = float4(0, 0, 0, 0);
					dQd	= float4(0, 0, 0, 0);
					
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						jointAttr	= getVertexAttribute(VertexComponent.BONES[i]);
						jointId		= jointAttr.x;
						jointWeight	= jointAttr.y;
						
						var jointDQn	: SFloat = getFieldFromArray(jointId, dQnList, false);
						var jointDQd	: SFloat = getFieldFromArray(jointId, dQdList, false);
						
						dQn.incrementBy(multiply(jointWeight, jointDQn));
						dQd.incrementBy(multiply(jointWeight, jointDQd));
					}
				}
				
				var dQnInvLength : SFloat = rsqrt(dotProduct4(dQn, dQn));
				
				dQn = multiply(dQn, dQnInvLength);
				dQd = multiply(dQd, dQnInvLength);
				
				var dQnX	: SFloat = dQn.w;
				var dQnYZW	: SFloat = dQn.xyz;
				var dQdX	: SFloat = dQd.w;
				var dQdYZW	: SFloat = dQd.xyz;
				
				var tmp0	: SFloat = multiply(dQnX, inPosition);			//	tmp0 = blendDQ[0].x*IN.position.xyz
				var tmp1	: SFloat = crossProduct(dQnYZW, inPosition);	//	tmp1 = cross(blendDQ[0].yzw, IN.position.xyz)
				var tmp2	: SFloat = add(tmp1, tmp0);						//	tmp2 = tmp1 + tmp0
				var tmp3	: SFloat = crossProduct(dQnYZW, tmp2);			//	tmp3 = cross(blendDQ[0].yzw, tmp2)
				var tmp4	: SFloat = multiply(2, tmp3);					//	tmp4 = 2.0*tmp3
				var tmp5	: SFloat = add(inPosition, tmp4);				//	tmp5 = IN.position.xyz + tmp4
				var tmp6	: SFloat = multiply(dQnX, dQdYZW);				//	tmp6 = blendDQ[0].x*blendDQ[1].yzw
				var tmp7	: SFloat = multiply(dQdX, dQnYZW);				//	tmp7 = blendDQ[1].x*blendDQ[0].yzw
				var tmp8	: SFloat = crossProduct(dQnYZW, dQdYZW);		//	tmp8 = cross(blendDQ[0].yzw, blendDQ[1].yzw)
				var tmp9	: SFloat = subtract(tmp6, tmp7);				//	tmp9 = tmp6 - tmp7
				var tmp10	: SFloat = add(tmp9, tmp8);						//	tmp10 = tmp9 + tmp8
				var tmp11	: SFloat = multiply(2, tmp10);					//	tmp11 = 2.0*(tmp10)
				
				outVertexPosition	= add(tmp5, tmp11);						//	position = tmp5 + tmp11
				outVertexPosition	= float4(outVertexPosition, 1);
				
				result = outVertexPosition;
			}
			
			return result;
		}
		
		private function dualQuaternionSkinNormal(inNormal : SFloat) : SFloat
		{
			var numBones		: uint = uint(meshBindings.getConstant('skinningNumBones'));
			var maxInfluences	: uint = uint(meshBindings.getConstant('skinningMaxInfluences'));
			
			var outNormal	: SFloat = null;
			
			if (maxInfluences == 0)
			{
				outNormal = inNormal; // new Combine(inNormal, new Constant(1.0));
			}
			else
			{
				var bindShape	: SFloat = meshBindings.getParameter('skinningBindShape', 16);
				var dQnList		: SFloat = meshBindings.getParameter('skinningDQn', 4 * numBones);
				var dQdList		: SFloat = meshBindings.getParameter('skinningDQd', 4 * numBones);
				
				var jointAttr	: SFloat;
				var jointId		: SFloat;
				var jointWeight	: SFloat;
				
				var dQn			: SFloat;
				var dQd			: SFloat;
				
				inNormal = multiply3x3(inNormal, bindShape); // #
				
				if (maxInfluences == 1)
				{
					jointAttr	= getVertexAttribute(VertexComponent.BONES[0]);
					jointId		= jointAttr.x;
					
					dQn = getFieldFromArray(jointId, dQnList, false);
					dQd = getFieldFromArray(jointId, dQdList, false);
				}
				else
				{
					dQn = float4(0, 0, 0, 0);
					dQd	= float4(0, 0, 0, 0);
					
					for (var i : uint = 0; i < maxInfluences; ++i)
					{
						jointAttr	= getVertexAttribute(VertexComponent.BONES[i]);
						jointId		= jointAttr.x;
						jointWeight	= jointAttr.y;
						
						var jointDQn	: SFloat = getFieldFromArray(jointId, dQnList, false);
						var jointDQd	: SFloat = getFieldFromArray(jointId, dQdList, false);
						
						dQn.incrementBy(multiply(jointWeight, jointDQn));
						dQd.incrementBy(multiply(jointWeight, jointDQd));
					}
				}
				
				var dQnInvLength : SFloat = rsqrt(dotProduct4(dQn, dQn));
				
				dQn = multiply(dQn, dQnInvLength);
				dQd = multiply(dQd, dQnInvLength);
				
				var dQnX	: SFloat	= dQn.w;
				var dQnYZW	: SFloat	= dQn.xyz;
				var dQdX	: SFloat	= dQd.w;
				var dQdYZW	: SFloat	= dQd.xyz;
				
				var tmp0	: SFloat = multiply(dQnX, inNormal);		//	tmp0 = blendDQ[0].x*IN.position.xyz
				var tmp1	: SFloat = crossProduct(dQnYZW, inNormal);	//	tmp1 = cross(blendDQ[0].yzw, IN.position.xyz)
				var tmp2	: SFloat = add(tmp1, tmp0);					//	tmp2 = tmp1 + tmp0
				var tmp3	: SFloat = crossProduct(dQnYZW, tmp2);		//	tmp3 = cross(blendDQ[0].yzw, tmp2)
				var tmp4	: SFloat = multiply(2, tmp3);				//	tmp4 = 2.0*tmp3
				var tmp5	: SFloat = add(inNormal, tmp4);				//	tmp5 = IN.position.xyz + tmp4
				var tmp6	: SFloat = multiply(dQnX, dQdYZW);			//	tmp6 = blendDQ[0].x*blendDQ[1].yzw
				var tmp7	: SFloat = multiply(dQdX, dQnYZW);			//	tmp7 = blendDQ[1].x*blendDQ[0].yzw
				var tmp8	: SFloat = crossProduct(dQnYZW, dQdYZW);	//	tmp8 = cross(blendDQ[0].yzw, blendDQ[1].yzw)
				var tmp9	: SFloat = subtract(tmp6, tmp7);			//	tmp9 = tmp6 - tmp7
				var tmp10	: SFloat = add(tmp9, tmp8);					//	tmp10 = tmp9 + tmp8
				var tmp11	: SFloat = multiply(2, tmp10);				//	tmp11 = 2.0*(tmp10)
				
				outNormal = add(tmp5, tmp11);							//	position = tmp5 + tmp11
				
				// outNormal		= new Combine(outNormal, new Constant(1.0));
			}
			
			return normalize(outNormal);
		}
		
		
		private function dualQuaternionScaleSkinPosition(inPosition : SFloat) : SFloat
		{
			throw new Error("This feature is still to be implemented. Use DualQuaternion of Matrix instead.");
		}
		
		private function dualQuaternionScaleSkinNormal(inNormal : SFloat) : SFloat
		{
			throw new Error("This feature is still to be implemented. Use DualQuaternion of Matrix instead.");
		}

	}
}
