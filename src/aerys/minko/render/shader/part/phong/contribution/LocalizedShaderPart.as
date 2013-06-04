package aerys.minko.render.shader.part.phong.contribution
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	
	public class LocalizedShaderPart extends AbstractContributionShaderPart
	{
		public function LocalizedShaderPart(main : Shader)
		{
			super(main);
		}

		/**
		 * @inherit
		 */
		override public function computeDiffuseInTangentSpace(lightId : uint) : SFloat
		{
			// compute light direction
			var cLocalLightPosition		: SFloat = worldToLocal(getLightParameter(lightId, 'worldPosition', 4));
			var vsLocalLightDirection	: SFloat = subtract(vsLocalPosition, cLocalLightPosition);
			var fsTangentLightDirection	: SFloat = normalize(interpolate(deltaLocalToTangent(vsLocalLightDirection)));
			
			return diffuseFromVectors(lightId, fsTangentLightDirection, fsTangentNormal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeDiffuseInLocalSpace(lightId : uint) : SFloat
		{
			var cLocalLightPosition		: SFloat = worldToLocal(getLightParameter(lightId, 'worldPosition', 4));
			var fsLocalLightDirection	: SFloat = normalize(subtract(fsLocalPosition, cLocalLightPosition));
			
			return diffuseFromVectors(lightId, fsLocalLightDirection, fsLocalNormal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeDiffuseInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			// compute light direction
			var cWorldLightPosition		: SFloat = getLightParameter(lightId, 'worldPosition', 4);
			var fsWorldLightDirection	: SFloat = normalize(subtract(fsWorldPosition, cWorldLightPosition));
			
			return diffuseFromVectors(lightId, fsWorldLightDirection, normal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeSpecularInTangentSpace(lightId : uint) : SFloat
		{
			// compute light direction
			var cLocalLightPosition			: SFloat = worldToLocal(getLightParameter(lightId, 'worldPosition', 4));
			var vsLocalLightDirection		: SFloat = subtract(vsLocalPosition, cLocalLightPosition.xyz);
			var fsTangentLightDirection		: SFloat = normalize(interpolate(deltaLocalToTangent(vsLocalLightDirection)));
			// compute camera direction
			var cLocalCameraPosition		: SFloat = worldToLocal(sceneBindings.getParameter('cameraPosition', 4));
			var vsLocalCameraDirection		: SFloat = subtract(vsLocalPosition, cLocalCameraPosition);
			var fsTangentCameraDirection	: SFloat = normalize(interpolate(deltaLocalToTangent(vsLocalCameraDirection)));
			
			return specularFromVectors(lightId, fsTangentLightDirection, fsTangentNormal, fsTangentCameraDirection);
		}
		
		/**
		 * @inherit
		 */
		override public function computeSpecularInLocalSpace(lightId : uint) : SFloat
		{
			throw new Error('Implement me');
		}
		
		/**
		 * @inherit
		 */
		override public function computeSpecularInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			// compute light direction
			var cWorldLightPosition		: SFloat = getLightParameter(lightId, 'worldPosition', 4);
			var fsWorldLightDirection	: SFloat = normalize(subtract(fsWorldPosition, cWorldLightPosition));
			// compute camera direction
			var cWorldCameraPosition	: SFloat = sceneBindings.getParameter('cameraPosition', 4);
			var fsWorldCameraDirection	: SFloat = normalize(subtract(fsWorldPosition, cWorldCameraPosition));
			
			return specularFromVectors(lightId, fsWorldLightDirection, normal, fsWorldCameraDirection);
		}
	}
}
