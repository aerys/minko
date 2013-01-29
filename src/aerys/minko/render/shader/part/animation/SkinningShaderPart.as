package aerys.minko.render.shader.part.animation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.animation.SkinningMethod;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	
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
			var skinningMethod : uint = uint(meshBindings.getProperty('skinningMethod'));
			
			switch (skinningMethod)
			{
				case SkinningMethod.HARDWARE_MATRIX:
					return matrixSkinPosition(inPosition);
				
				case SkinningMethod.HARDWARE_DUAL_QUATERNION:
					return dualQuaternionSkinPosition(inPosition);
				
				case SkinningMethod.HARDWARE_DUAL_QUATERNION_SCALE:
					return dualQuaternionScaleSkinPosition(inPosition);
				
				default:
					throw new Error('Unknown animation method.');
			}
		}
		
		public function skinNormal(inNormal : SFloat) : SFloat
		{
			var skinningMethod : uint = uint(meshBindings.getProperty('skinningMethod'));
			
			switch (skinningMethod)
			{
				case SkinningMethod.HARDWARE_MATRIX:
					return matrixSkinNormal(inNormal);
					
				case SkinningMethod.HARDWARE_DUAL_QUATERNION:
					return dualQuaternionSkinNormal(inNormal);

				case SkinningMethod.HARDWARE_DUAL_QUATERNION_SCALE:
					return dualQuaternionScaleSkinNormal(inNormal);
					
				default :
					throw new Error('Unknown animation method.');
			}
		}
		
		public function skinTangent(inTangent : SFloat) : SFloat
		{
			var skinningMethod : uint = uint(meshBindings.getProperty('skinningMethod'));
			
			switch (skinningMethod)
			{
				case SkinningMethod.HARDWARE_MATRIX:
					return matrixSkinTangent(inTangent);
					
				case SkinningMethod.HARDWARE_DUAL_QUATERNION:
					return dualQuaternionSkinTangent(inTangent);
					
				case SkinningMethod.HARDWARE_DUAL_QUATERNION_SCALE:
					return dualQuaternionScaleSkinTangent(inTangent);
					
				default :
					throw new Error('Unknown animation method.');
			}
		}
		
		private function matrixSkinPosition(inVertexPosition : SFloat) : SFloat
		{
			var numBones			: uint		= uint(meshBindings.getProperty('skinningNumBones'));
			var maxInfluences		: uint		= uint(meshBindings.getProperty('skinningMaxInfluences'));
			var outVertexPosition	: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				outVertexPosition = inVertexPosition;
			}
			else
			{
				var skinningMatrices	: SFloat = meshBindings.getParameter('skinningMatrices', 16 * numBones);
				var jointAttr			: SFloat;
				var jointSkinningMatrix	: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr			= getVertexAttribute(VertexComponent.BONE_S);
					jointSkinningMatrix	= getFieldFromArray(multiply(4, jointAttr.x), skinningMatrices, true);
					outVertexPosition	= multiply4x4(inVertexPosition, jointSkinningMatrix);
				}
				else
				{
//					outVertexPosition = float4(0, 0, 0, 0);
					
					var jointOutVertexPosition : SFloat;
					for (var i : uint = 0; i < (maxInfluences >> 1); ++i)
					{
						jointAttr				= getVertexAttribute(VertexComponent.BONES[i]);
						var jointId : SFloat = multiply(4, jointAttr.xz);
						
						jointSkinningMatrix		= getFieldFromArray(jointId.x, skinningMatrices, true);
						jointOutVertexPosition	= multiply4x4(inVertexPosition, jointSkinningMatrix);
						jointOutVertexPosition	= multiply(jointAttr.y, jointOutVertexPosition);
						outVertexPosition = outVertexPosition
                            ? outVertexPosition.incrementBy(jointOutVertexPosition)
                            : jointOutVertexPosition;
						
						jointSkinningMatrix		= getFieldFromArray(jointId.y, skinningMatrices, true);
						jointOutVertexPosition	= multiply4x4(inVertexPosition, jointSkinningMatrix);
						jointOutVertexPosition	= multiply(jointAttr.w, jointOutVertexPosition);
						outVertexPosition.incrementBy(jointOutVertexPosition);
					}
					
					if (maxInfluences % 2 == 1)
					{
						jointAttr				= getVertexAttribute(VertexComponent.BONE_S);
						
						jointSkinningMatrix		= getFieldFromArray(multiply(4, jointAttr.x), skinningMatrices, true);
						jointOutVertexPosition	= multiply4x4(inVertexPosition, jointSkinningMatrix);
						jointOutVertexPosition	= multiply(jointAttr.y, jointOutVertexPosition);
						outVertexPosition.incrementBy(jointOutVertexPosition);
					}
				}
			}
			
			return outVertexPosition;
		}
		
		private function matrixSkinNormal(inVertexNormal : SFloat) : SFloat
		{
			inVertexNormal = inVertexNormal.xyz;
			
			var numBones		: uint		= uint(meshBindings.getProperty('skinningNumBones'));
			var maxInfluences	: uint		= uint(meshBindings.getProperty('skinningMaxInfluences'));
			var outVertexNormal	: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				outVertexNormal = inVertexNormal;
			}
			else
			{
				var skinningMatrices	: SFloat = meshBindings.getParameter('skinningMatrices', 16 * numBones);
				var jointAttr			: SFloat;
				var jointSkinningMatrix	: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr			= getVertexAttribute(VertexComponent.BONE_S);
					jointSkinningMatrix	= getFieldFromArray(jointAttr.x, skinningMatrices, true);
					outVertexNormal		= multiply3x3(inVertexNormal, jointSkinningMatrix);
				}
				else
				{
					var jointOutNormal	: SFloat;

					outVertexNormal = float4(0, 0, 0, 0);
					for (var i : uint = 0; i < (maxInfluences >> 1); ++i)
					{
						jointAttr			= getVertexAttribute(VertexComponent.BONES[i]);
						var jointId : SFloat = multiply(4, jointAttr.xz);
						
						jointSkinningMatrix	= getFieldFromArray(jointId.x, skinningMatrices, true);
						jointOutNormal		= multiply3x3(inVertexNormal, jointSkinningMatrix);
						jointOutNormal		= multiply(jointAttr.y, jointOutNormal);
//                        outVertexNormal.incrementBy(jointOutNormal)
						outVertexNormal = outVertexNormal
                            ? outVertexNormal.incrementBy(jointOutNormal)
                            : jointOutNormal;
						
						jointSkinningMatrix	= getFieldFromArray(jointId.y, skinningMatrices, true);
						jointOutNormal		= multiply3x3(inVertexNormal, jointSkinningMatrix);
						jointOutNormal		= multiply(jointAttr.w, jointOutNormal);
						outVertexNormal.incrementBy(jointOutNormal);
					}
					
					if (maxInfluences % 2 == 1)
					{
						jointAttr			= getVertexAttribute(VertexComponent.BONE_S);
						
						jointSkinningMatrix	= getFieldFromArray(multiply(4, jointAttr.x), skinningMatrices, true);
						jointOutNormal		= multiply3x3(inVertexNormal, jointSkinningMatrix);
						jointOutNormal		= multiply(jointAttr.y, jointOutNormal);
						outVertexNormal.incrementBy(jointOutNormal);
					}
				}
			}
			
			return outVertexNormal;
		}
		
		private function matrixSkinTangent(inTangent:SFloat):SFloat
		{
			return inTangent;
		}
		
		private function dualQuaternionSkinPosition(inPosition : SFloat) : SFloat
		{
			var numBones		: uint	= uint(meshBindings.getProperty('skinningNumBones'));
			var maxInfluences	: uint	= uint(meshBindings.getProperty('skinningMaxInfluences'));
			
			var outVertexPosition	: SFloat	= null;
			var result				: SFloat	= null;
			
			if (maxInfluences == 0)
			{
				result = inPosition;
			}
			else
			{
				var dQnList		: SFloat = meshBindings.getParameter('skinningDQn', 4 * numBones);
				var dQdList		: SFloat = meshBindings.getParameter('skinningDQd', 4 * numBones);
				
				inPosition = inPosition.xyz;
				
				var jointAttr	: SFloat;
				var dQn			: SFloat;
				var dQd			: SFloat;
				
				if (maxInfluences == 1)
				{
					jointAttr	= getVertexAttribute(VertexComponent.BONE_S);
					
					dQn = getFieldFromArray(jointAttr.x, dQnList, false);
					dQd = getFieldFromArray(jointAttr.x, dQdList, false);
				}
				else
				{
					dQn = float4(0, 0, 0, 0);
					dQd	= float4(0, 0, 0, 0);
					
					var jointDQn	: SFloat;
					var jointDQd	: SFloat;
					for (var i : uint = 0; i < (maxInfluences >> 1); ++i)
					{
						jointAttr	= getVertexAttribute(VertexComponent.BONES[i]);
						
						jointDQn	= getFieldFromArray(jointAttr.x, dQnList, false);
						jointDQd	= getFieldFromArray(jointAttr.x, dQdList, false);
						dQn.incrementBy(multiply(jointAttr.y, jointDQn));
						dQd.incrementBy(multiply(jointAttr.y, jointDQd));
						
						jointDQn	= getFieldFromArray(jointAttr.z, dQnList, false);
						jointDQd	= getFieldFromArray(jointAttr.z, dQdList, false);
						dQn.incrementBy(multiply(jointAttr.w, jointDQn));
						dQd.incrementBy(multiply(jointAttr.w, jointDQd));
					}
					
					if (maxInfluences % 2 == 1)
					{
						jointAttr	= getVertexAttribute(VertexComponent.BONE_S);
						
						jointDQn	= getFieldFromArray(jointAttr.x, dQnList, false);
						jointDQd	= getFieldFromArray(jointAttr.x, dQdList, false);
						dQn.incrementBy(multiply(jointAttr.y, jointDQn));
						dQd.incrementBy(multiply(jointAttr.y, jointDQd));
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
			return inNormal;
		}
		
		private function dualQuaternionSkinTangent(inTangent : SFloat) : SFloat
		{
			return inTangent;
		}
		
		private function dualQuaternionScaleSkinPosition(inPosition : SFloat) : SFloat
		{
			throw new Error("This feature is still to be implemented. Use DualQuaternion of Matrix instead.");
		}
		
		private function dualQuaternionScaleSkinNormal(inNormal : SFloat) : SFloat
		{
			throw new Error("This feature is still to be implemented. Use DualQuaternion of Matrix instead.");
		}
		
		private function dualQuaternionScaleSkinTangent(inTangent:SFloat):SFloat
		{
			throw new Error("This feature is still to be implemented. Use DualQuaternion of Matrix instead.");
		}
	}
}
