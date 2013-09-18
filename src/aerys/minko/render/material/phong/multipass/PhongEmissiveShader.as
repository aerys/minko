package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.resource.texture.ITextureResource;
    import aerys.minko.render.shader.SFloat;
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
            
            _diffuseAccumulator = diffuseAccumulator;
			_specularAccumulator = specularAccumulator;
        }
        
        override protected function getVertexPosition() : SFloat
        {
            return _screenPos = super.getVertexPosition();
        }
        
        override protected function getPixelColor() : SFloat
        {
            var diffuse : SFloat = super.getPixelColor();
            
            if (_diffuseAccumulator || _specularAccumulator)
            {
                var uv : SFloat = interpolate(_screenPos);
                
                uv = divide(uv.xy, uv.w);
                uv = multiply(add(float2(uv.x, negate(uv.y)), 1), .5);
                
                var lighting : SFloat = null;
				
				if (_diffuseAccumulator)
					lighting = sampleTexture(getTexture(_diffuseAccumulator, SamplerFiltering.LINEAR), uv);
				if (_specularAccumulator)
				{
					var specular : SFloat = sampleTexture(getTexture(_specularAccumulator), uv);
					
					lighting = lighting ? add(lighting, specular) : specular;
				}
                
                diffuse = float4(multiply(diffuse.rgb, lighting.rgb), diffuse.a);
            }
            
            return diffuse;
        }
    }
}