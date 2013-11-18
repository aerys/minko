package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.resource.texture.ITextureResource;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderOptimization;
    import aerys.minko.type.enum.SamplerFiltering;
    
    public class PhongEmissiveShader extends BasicShader
    {
        private var _diffuseAccumulator   	: ITextureResource;
		private var _specularAccumulator   	: ITextureResource;
        
        private var _screenPos          	: SFloat;
        
        public function PhongEmissiveShader(diffuseAccumulator	: ITextureResource,
											specularAccumulator : ITextureResource,
                                            renderTarget        : RenderTarget  = null,
                                            priority            : Number        = 0.0)
        {
            super(renderTarget, priority);
            
			optimization |= ShaderOptimization.RESOLVED_PARAMETRIZATION;
			
            _diffuseAccumulator = diffuseAccumulator;
			_specularAccumulator = specularAccumulator;
        }
        
        override protected function getVertexPosition() : SFloat
        {
            return _screenPos = super.getVertexPosition();
        }
        
        override protected function getPixelColor() : SFloat
        {
			var materialDiffuse	: SFloat	= super.getPixelColor();
            var shading 		: SFloat 	= materialDiffuse;
            
            if (_diffuseAccumulator || _specularAccumulator)
            {
                var uv : SFloat = interpolate(_screenPos);
                
                uv = divide(uv.xy, uv.w);
                uv = multiply(add(float2(uv.x, negate(uv.y)), 1), .5);
                				
				if (_diffuseAccumulator)
					shading = sampleTexture(getTexture(_diffuseAccumulator, SamplerFiltering.LINEAR), uv);
				if (_specularAccumulator)
				{
					var specular : SFloat = sampleTexture(getTexture(_specularAccumulator), uv);
					
					shading = shading ? add(shading, specular) : specular;
				}
                
				shading = float4(shading.rgb, materialDiffuse.a);
//				diffuse = float4(add(diffuse.rgb, lighting.rgb), diffuse.a);
//                diffuse = float4(multiply(diffuse.rgb, lighting.rgb), diffuse.a);
            }
            
            return shading;
        }
    }
}