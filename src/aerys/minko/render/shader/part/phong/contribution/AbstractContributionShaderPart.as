package aerys.minko.render.shader.part.phong.contribution
{
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerFormat;

	/**
	 * Methods in this class allow to compute light contributions.
	 * It is extended by InfiniteShaderPart for directional lights, and LocalizedShaderPart for point and spot lights.
	 * 
	 * All methods are available in tangent, local and world space.
	 * 
	 * - Computing in tangent space is computationally more expensive, both in CPU and GPU, because we need to feed
	 * more data to the shaders than the other methods but it allows to do both bump and parallax mapping.
	 * 
	 * - Doing it in local space is computationally more expensive on the CPU, because the light
	 * position will need to be converted in the CPU for each mesh and at each frame will produce the lighter shaders.
	 * 
	 * Doing as much work on the CPU as possible, and have as short a possible shader would be the way to go
	 * if adobe's compiler was better than it is. Therefor, minko defaults to computing everything in world space
	 * which is faster in practical tests.
	 * 
	 * The cost of each light will be in O(number of drawcalls) on the CPU.
	 * However, this is the way that will lead to the smaller shaders, and is therefor recomended when used
	 * in small scenes with many lights.
	 * 
	 * - Computing lights in world space is cheaper on the CPU, because no extra computation is
	 * needed for each mesh, but will cost one extra matrix multiplication on the GPU (we need the world position
	 * or every fragment).
	 * 
	 * @author Romain Gilliotte
	 * 
	 */	
	public class AbstractContributionShaderPart extends LightAwareShaderPart
	{
		
		public function AbstractContributionShaderPart(main : Shader)
		{
			super(main);
		}
		
		/**
		 * Creates the shader subgraph to compute the diffuse value of a given light.
		 * Computations will be done in tangent space.
		 * 
		 * @param lightId The id of the localized light (PointLight or SpotLight)
		 * @return Shader subgraph representing the diffuse value of this light. 
		 */
		public function computeDiffuseInTangentSpace(lightId : uint) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Creates the shader subgraph to compute the diffuse value of a given light.
		 * Computation will be done in local space.
		 * 
		 * @param lightId The id of the localized light (PointLight or SpotLight)
		 * @return Shader subgraph representing the diffuse value of this light. 
		 */
		public function computeDiffuseInLocalSpace(lightId : uint) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Creates the shader subgraph to compute the diffuse value of a given light.
		 * Computation will be done in world space.
		 * 
		 * @param lightId The id of the light
		 * @return Shader subgraph representing the diffuse value of this light. 
		 */
		public function computeDiffuseInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Creates the shader subgraph to compute the specular value of a given light.
		 * Computation will be done in tangent space.
		 * 
		 * @param lightId
		 * @return 
		 */		
		public function computeSpecularInTangentSpace(lightId : uint) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Creates the shader subgraph to compute the specular value of a given light.
		 * Computation will be done in local space.
		 * 
		 * @param lightId
		 * @return 
		 */
		public function computeSpecularInLocalSpace(lightId : uint) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Creates the shader subgraph to compute the specular value of a given light.
		 * Computation will be done in world space.
		 * 
		 * @param lightId
		 * @return 
		 */
		public function computeSpecularInWorldSpace(lightId : uint, normal : SFloat) : SFloat
		{
			throw new Error('Must be overriden');
		}
		
		/**
		 * Compute final diffuse value from light direction and normal.
		 * Both requested vector can be in any space (tangent, local, light, view or whatever) but must be in the same space.
		 * Also they must be recheable in the fragment shader (they must be constant, or already interpolated)
		 * 
		 * @param lightId
		 * @param fsLightDirection
		 * @param fsNormal
		 * @return 
		 */		
		protected function diffuseFromVectors(lightId			: uint,
											  fsLightDirection	: SFloat,
											  fsNormal			: SFloat) : SFloat
		{
			var fsLambertProduct	: SFloat = saturate(negate(dotProduct3(fsLightDirection, fsNormal)));
			var cDiffuse			: SFloat = getLightParameter(lightId, 'diffuse', 1);
			
			if (meshBindings.propertyExists(PhongProperties.DIFFUSE_MULTIPLIER))
				cDiffuse.scaleBy(meshBindings.getParameter(PhongProperties.DIFFUSE_MULTIPLIER, 1));
			
			return multiply(cDiffuse, fsLambertProduct);
		}
		
		/**
		 * Compute final specular value from light direction, normal, and camera direction.
		 * All three requested vector can be in any space (tangent, local, light, view or whatever) but must all be in the same sapce.
		 * Also they must be recheable in the fragment shader (they must be constant, or already interpolated)
		 * 
		 * @param lightId
		 * @param fsLightDirection
		 * @param fsNormal
		 * @param fsCameraDirection
		 * @return 
		 */		
		protected function specularFromVectors(lightId				: uint, 
											   fsLightDirection		: SFloat, 
											   fsNormal				: SFloat, 
											   fsCameraDirection	: SFloat) : SFloat
		{
			var fsLightReflectedDirection	: SFloat = reflect(fsLightDirection, fsNormal);
			var fsLambertProduct			: SFloat = saturate(negate(dotProduct3(fsLightReflectedDirection, fsCameraDirection)));
			
			var cLightSpecular	: SFloat 	= getLightParameter(lightId, 'specular', 1);
			var cLightShininess	: SFloat 	= getLightParameter(lightId, 'shininess', 1);
			
			if (meshBindings.propertyExists(PhongProperties.SPECULAR))
			{
				var specular	: SFloat	= meshBindings.getParameter(PhongProperties.SPECULAR, 4);
                
				cLightSpecular = multiply(cLightSpecular, specular.xyz);
			}
			
			if (meshBindings.propertyExists(PhongProperties.SPECULAR_MAP))
			{
				var fsSpecularSample 	: SFloat	= sampleTexture(
					meshBindings.getTextureParameter(
						PhongProperties.SPECULAR_MAP,
						1,
						0,
						1,
						0,
						meshBindings.getProperty(PhongProperties.SPECULAR_MAP_FORMAT, SamplerFormat.RGBA)
					),
					fsUV
				);
				
				cLightSpecular.scaleBy(fsSpecularSample.x);
			}
			
			if (meshBindings.propertyExists(PhongProperties.SHININESS))
				cLightShininess.scaleBy(meshBindings.getParameter(PhongProperties.SHININESS, 1));
			
			return multiply(cLightSpecular, power(fsLambertProduct, cLightShininess));
		}
	}
}
