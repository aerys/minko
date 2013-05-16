package aerys.minko.render.shader.part.phong.contribution
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;

	public class InfiniteShaderPart extends AbstractContributionShaderPart
	{
		public function InfiniteShaderPart(main : Shader)
		{
			super(main);
		}
		
		/**
		 * @inherit
		 */
		override public function computeDiffuseInTangentSpace(lightId : uint) : SFloat
		{
			var cLightLocalDirection	: SFloat = deltaWorldToLocal(getLightParameter(lightId, 'worldDirection', 3));
			var fsTangentLightDirection	: SFloat = normalize(interpolate(deltaLocalToTangent(cLightLocalDirection)));
			
			return diffuseFromVectors(lightId, fsTangentLightDirection, fsTangentNormal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeDiffuseInLocalSpace(lightId : uint) : SFloat
		{
			var cLocalLightDirection : SFloat = deltaWorldToLocal(getLightParameter(lightId, 'worldDirection', 3));
			
			return diffuseFromVectors(lightId, cLocalLightDirection, fsLocalNormal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeDiffuseInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			var cWorldLightDirection : SFloat = getLightParameter(lightId, 'worldDirection', 3);
			
			return diffuseFromVectors(lightId, cWorldLightDirection, normal);
		}
		
		/**
		 * @inherit
		 */
		override public function computeSpecularInTangentSpace(lightId:uint):SFloat
		{
			var cLocalLightDirection		: SFloat = deltaWorldToLocal(getLightParameter(lightId, 'worldDirection', 3));
			var vsTangentLightDirection		: SFloat = deltaLocalToTangent(cLocalLightDirection);
			var fsTangentLightDirection		: SFloat = normalize(interpolate(vsTangentLightDirection));
			
			var cLocalCameraPosition		: SFloat = worldToLocal(sceneBindings.getParameter('cameraPosition', 4));
			var vsLocalCameraDirection		: SFloat = subtract(vsLocalPosition, cLocalCameraPosition);
			var vsTangentCameraDirection	: SFloat = deltaLocalToTangent(vsLocalCameraDirection);
			var fsTangentCameraDirection	: SFloat = normalize(interpolate(vsTangentCameraDirection));
			
			return specularFromVectors(lightId, fsTangentLightDirection, fsTangentNormal, fsTangentCameraDirection);
		}
		
		/**
		 * @inherit
		 */		
		override public function computeSpecularInLocalSpace(lightId:uint):SFloat
		{
			var cLocalLightDirection	: SFloat = normalize(deltaWorldToLocal(getLightParameter(lightId, 'worldDirection', 3)));
			
			var cLocalCameraPosition	: SFloat = worldToLocal(sceneBindings.getParameter('cameraPosition', 4));
			var vsLocalCameraDirection	: SFloat = subtract(vsLocalPosition, cLocalCameraPosition);
			var fsLocalCameraDirection	: SFloat = normalize(interpolate(vsLocalCameraDirection));
			
			return specularFromVectors(lightId, cLocalLightDirection, fsLocalNormal, fsLocalCameraDirection);
		}
		
		/**
		 * @inherit
		 */
		override public function computeSpecularInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			var cWorldLightDirection	: SFloat = getLightParameter(lightId, 'worldDirection', 3);
			var cWorldCameraPosition	: SFloat = sceneBindings.getParameter('cameraPosition', 4);
			var fsWorldCameraDirection	: SFloat = normalize(subtract(fsWorldPosition, cWorldCameraPosition));
			
			return specularFromVectors(lightId, cWorldLightDirection, normal, fsWorldCameraDirection);
		}
		
	}
}
