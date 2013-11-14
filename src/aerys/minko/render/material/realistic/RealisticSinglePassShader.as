package aerys.minko.render.material.realistic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.material.environment.EnvironmentMappingProperties;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderOptimization;
	import aerys.minko.render.shader.part.BlendingShaderPart;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
	import aerys.minko.render.shader.part.phong.PhongShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerFormat;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public class RealisticSinglePassShader extends BasicShader
	{
		private var _diffuse            : DiffuseShaderPart;
		private var _phong              : PhongShaderPart;
		private var _blending           : BlendingShaderPart;
		private var _environmentMapping : EnvironmentMappingShaderPart;
		private var _uv					: SFloat;
		
		public function RealisticSinglePassShader(target      : RenderTarget  = null,
												  priority    : Number        = 0.)
		{
			super(target, priority);
			
			optimization |= ShaderOptimization.RESOLVED_PARAMETRIZATION;
			
			_diffuse = new DiffuseShaderPart(this);
			_phong = new PhongShaderPart(this);
			_blending = new BlendingShaderPart(this);
			_environmentMapping = new EnvironmentMappingShaderPart(this);
		}
		
		override protected function getVertexPosition():SFloat
		{
			_uv = vertexUV.xy;
			
			if (meshBindings.propertyExists(BasicProperties.UV_SCALE))
				_uv.scaleBy(meshBindings.getParameter(BasicProperties.UV_SCALE, 2));
			
			if (meshBindings.propertyExists(BasicProperties.UV_OFFSET))
				_uv.incrementBy(meshBindings.getParameter(BasicProperties.UV_OFFSET, 2));
			
			return super.getVertexPosition();
		}
		
		override protected function getPixelColor() : SFloat
		{
			var materialDiffuse : SFloat    = _diffuse.getDiffuseColor();			
			var envColor        : SFloat    = _environmentMapping.getEnvironmentColor();
			var envBlending     : uint      = meshBindings.getProperty(
				EnvironmentMappingProperties.ENVIRONMENT_BLENDING, Blending.ALPHA
			);
			var finalDiffuse	: SFloat	= _blending.blend(envColor, materialDiffuse, envBlending);
			
			// BigPoint trick
			if (meshBindings.propertyExists(PhongProperties.SPECULAR_MAP))
			{
				var uv			: SFloat	= interpolate(_uv);
				var map			: SFloat	= meshBindings.getTextureParameter(
					PhongProperties.SPECULAR_MAP,
					SamplerFiltering.LINEAR,
					SamplerMipMapping.DISABLE,
					SamplerWrapping.REPEAT,
					0,
					meshBindings.getProperty(PhongProperties.SPECULAR_MAP_FORMAT, SamplerFormat.RGBA)
				);
				var envWeight	: SFloat	= sampleTexture(map, uv);
				
				finalDiffuse	= mix(materialDiffuse, finalDiffuse, envWeight);
			}
			
			var shading	: SFloat	= add(
				_phong.getStaticLighting(float4(1, 1, 1, 1)),
				_phong.getDynamicLighting(-1, true, true, true, finalDiffuse)
			);
			
			return float4(shading.rgb, materialDiffuse.a);
		}       
	}
}