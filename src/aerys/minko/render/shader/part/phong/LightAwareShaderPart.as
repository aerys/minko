package aerys.minko.render.shader.part.phong
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ParallaxMappingShaderPart;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.type.enum.NormalMappingType;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerFormat;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	import aerys.minko.type.enum.TriangleCulling;
	
	public class LightAwareShaderPart extends ShaderPart
	{
		use namespace minko_lighting;
		
		private var _vertexAnimationShaderPart	: VertexAnimationShaderPart;
		private var _parallaxMappingShaderPart	: ParallaxMappingShaderPart;
		
		private function get vertexAnimationShaderPart() : VertexAnimationShaderPart
		{
			_vertexAnimationShaderPart ||= new VertexAnimationShaderPart(main);
			return _vertexAnimationShaderPart;
		}
		
		private function get parallaxMappingShaderPart() : ParallaxMappingShaderPart
		{
			_parallaxMappingShaderPart ||= new ParallaxMappingShaderPart(main);
			return _parallaxMappingShaderPart;
		}
		
		protected function get vsLocalPosition() : SFloat
		{
			return vertexAnimationShaderPart.getAnimatedVertexPosition();
		}
		
		protected function get fsLocalPosition() : SFloat
		{
			return interpolate(vsLocalPosition);
		}
		
		protected function get fsUV() : SFloat
		{
			var result : SFloat = getVertexAttribute(VertexComponent.UV);
			
			if (meshBindings.propertyExists(BasicProperties.UV_SCALE))
				result.scaleBy(meshBindings.getParameter(BasicProperties.UV_SCALE, 2));
			
			if (meshBindings.propertyExists(BasicProperties.UV_OFFSET))
				result.incrementBy(meshBindings.getParameter(BasicProperties.UV_OFFSET, 2));
			
			result = interpolate(result.xy);
			
			var normalMappingType : uint = meshBindings.getProperty(
				PhongProperties.NORMAL_MAPPING_TYPE, NormalMappingType.NONE
			);
			
			switch (normalMappingType)
			{
				case NormalMappingType.NONE:
				case NormalMappingType.NORMAL:
					return result;
				
				case NormalMappingType.PARALLAX:
					return parallaxMappingShaderPart.getSteepParallaxMappedUV(result);
					
				default:
					throw new Error('Unknown normal mapping type.');
			}
		}
		
		protected function get vsWorldPosition() : SFloat
		{
			return localToWorld(vsLocalPosition);
		}
		
		protected function get fsWorldPosition() : SFloat
		{
			return interpolate(vsWorldPosition);
		}
		
		protected function get vsLocalNormal() : SFloat
		{
			return vertexAnimationShaderPart.getAnimatedVertexNormal();
		}
		
		protected function get vsWorldNormal() : SFloat
		{
			var v : SFloat = deltaLocalToWorld(vsLocalNormal);
			return normalize(v.xyz);
		}
		
		public function get fsLocalNormal() : SFloat
		{
			return normalize(interpolate(vsLocalNormal));
		}
		
		public function get fsWorldNormal() : SFloat
		{
			return normalize(interpolate(deltaLocalToWorld(vsLocalNormal)));
		}
		
		protected function get vsLocalTangent() : SFloat
		{
			var vertexTangent : SFloat = vertexAnimationShaderPart.getAnimatedVertexTangent();
			
			if (meshBindings.getProperty(BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK)
                != TriangleCulling.BACK)
				vertexTangent.negate();
			
			return vertexTangent;
		}
		
		protected function get fsLocalTangent() : SFloat
		{
			return interpolate(fsLocalTangent);
		}
		
		protected function get fsTangentNormal() : SFloat
		{
			var normalMappingType : uint = meshBindings.getProperty(
                PhongProperties.NORMAL_MAPPING_TYPE,
                NormalMappingType.NONE
            );
			
			switch (normalMappingType)
			{
				case NormalMappingType.NONE:
					return normalize(deltaLocalToTangent(fsLocalNormal));
				
				case NormalMappingType.NORMAL:
				case NormalMappingType.PARALLAX:
					var fsNormalMap	: SFloat = meshBindings.getTextureParameter(
						PhongProperties.NORMAL_MAP,
						meshBindings.getProperty(PhongProperties.NORMAL_MAP_FILTERING, SamplerFiltering.LINEAR),
						meshBindings.getProperty(PhongProperties.NORMAL_MAP_MIPMAPPING, SamplerMipMapping.LINEAR),
                        meshBindings.getProperty(PhongProperties.NORMAL_MAP_WRAPPING, SamplerWrapping.REPEAT),
                        0,
                        meshBindings.getProperty(PhongProperties.NORMAL_MAP_FORMAT, SamplerFormat.RGBA)
					);
					var fsPixel		: SFloat = sampleTexture(fsNormalMap, fsUV);
                    
                    fsPixel.scaleBy(2).decrementBy(1);
					
					return normalize(fsPixel.rgb);
					
				default:
					throw new Error('Invalid normap mapping type value');
			}
		}
		
		public function LightAwareShaderPart(main : Shader)
		{
			super(main);
		}
		
		protected function deltaLocalToTangent(v : SFloat) : SFloat
		{
			var vsLocalNormal	: SFloat = this.vsLocalNormal;
			var vsLocalTangent	: SFloat = this.vsLocalTangent;
			var vsLocalBinormal	: SFloat = crossProduct(vsLocalNormal, vsLocalTangent);
			
			return float3(
				dotProduct3(v, vsLocalTangent),
				dotProduct3(v, vsLocalBinormal),
				dotProduct3(v, vsLocalNormal)
			);
		}
		
		protected function deltaWorldToTangent(v : SFloat) : SFloat
		{
			return deltaLocalToTangent(deltaWorldToLocal(v));
		}
		
		public function lightPropertyExists(lightId : uint, name : String) : Boolean
		{
			return sceneBindings.propertyExists(
				LightDataProvider.getLightPropertyName(name, lightId)
			);
		}
		
		public function getLightProperty(lightId         : uint,
                                         name            : String,
                                         defaultValue    : Object = null) : *
		{
			return sceneBindings.getProperty(
				LightDataProvider.getLightPropertyName(name, lightId),
				defaultValue
			);
		}
		
		public function getLightParameter(lightId		: uint, 
                                          name			: String,
                                          size			: uint) : SFloat
		{
			return sceneBindings.getParameter(
				LightDataProvider.getLightPropertyName(name, lightId),
				size
			);
		}
		
		protected function getLightTextureParameter(lightId		: uint,
													name		: String, 
													filter		: uint = 1, 
													mipmap		: uint = 0, 
													wrapping	: uint = 1, 
													dimension	: uint = 0) : SFloat
		{
			return sceneBindings.getTextureParameter(
				LightDataProvider.getLightPropertyName(name, lightId),
				filter,
				mipmap,
				wrapping,
				dimension
			);
		}
		
	}
}
