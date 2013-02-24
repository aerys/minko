package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.resource.texture.ITextureResource;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    import aerys.minko.type.enum.BlendingDestination;
    import aerys.minko.type.enum.BlendingSource;
    import aerys.minko.type.enum.DepthTest;
    
    public class PhongEmissiveShader extends BasicShader
    {
        private var _lightAccumulator   : ITextureResource;
        
        private var _screenUv           : SFloat;
        
        public function PhongEmissiveShader(lightAccumulator    : ITextureResource,
                                            renderTarget        : RenderTarget  = null,
                                            priority            : Number        = 0.0)
        {
            super(renderTarget, priority);
            
            _lightAccumulator = lightAccumulator;
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {
            super.initializeSettings(settings);
            
            if (!_lightAccumulator)
            {
                settings.blending = BlendingDestination.SOURCE_COLOR | BlendingSource.ZERO;
                settings.depthTest = DepthTest.LESS | DepthTest.EQUAL;
            }
        }
        
        override protected function getVertexPosition() : SFloat
        {
            var pos : SFloat = super.getVertexPosition();
            
//            _screenUv = divide(pos.xy, pos.w);
//            _screenUv = float2(
//                divide(add(_screenUv.x, 1), 2),
//                subtract(1, divide(subtract(1, _screenUv.y), 2))
//            );
            
            _screenUv = pos.xy;
            
            return pos;
        }
        
        override protected function getPixelColor() : SFloat
        {
            var diffuse : SFloat = super.getPixelColor();
            
            if (_lightAccumulator)
            {
                var uv : SFloat = interpolate(_screenUv);
                var lighting : SFloat = sampleTexture(getTexture(_lightAccumulator), uv);
                
                return float4(uv, 0, 1);
                
                diffuse = float4(multiply(diffuse.rgb, lighting.rgb), diffuse.a);
            }
            
            return diffuse;
        }
    }
}