package aerys.minko.render.material.phong
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.LightAwareDiffuseShaderPart;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    import aerys.minko.type.enum.BlendingDestination;
    import aerys.minko.type.enum.BlendingSource;
    import aerys.minko.type.enum.DepthTest;
    
    public class MultiPassPhongShader extends BasicShader
    {
        private var _diffuse	: LightAwareDiffuseShaderPart;
        private var _phong		: PhongShaderPart;
        
        private var _lightId    : int;
        
        public function MultiPassPhongShader(renderTarget   : RenderTarget  = null,
                                             priority       : Number        = 0.0,
                                             lightId        : int           = -1)
        {
            super(renderTarget, priority);
            
            // init shader parts
            _diffuse	= new LightAwareDiffuseShaderPart(this);
            _phong		= new PhongShaderPart(this);
            
            _lightId = lightId;
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {
            super.initializeSettings(settings);
            
//            settings.blending = BlendingSource.DESTINATION_COLOR | BlendingDestination.ZERO;
//            settings.blending = BlendingSource.DESTINATION_ALPHA | BlendingDestination.ONE;
            settings.blending = BlendingSource.ONE | BlendingDestination.ONE;
            settings.depthTest = DepthTest.LESS | DepthTest.EQUAL;
        }
        
        override protected function getPixelColor() : SFloat
        {
            return float4(_phong.getDynamicLighting(_lightId, true), 1);
        }
    }
}