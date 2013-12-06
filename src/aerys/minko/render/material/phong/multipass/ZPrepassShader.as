package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicProperties;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.Shader;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.DiffuseShaderPart;
    import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
    import aerys.minko.type.enum.ColorMask;
    
    public class ZPrepassShader extends Shader
    {
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffuseShaderPart 		: DiffuseShaderPart;

        public function ZPrepassShader(renderTarget : RenderTarget  = null,
                                       priority     : Number        = 0.0)
        {
            super(renderTarget, priority);
			
			_diffuseShaderPart 		= new DiffuseShaderPart(this);
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {
            super.initializeSettings(settings);
            
            settings.colorMask = ColorMask.NONE;
        }
        
        override protected function getVertexPosition():SFloat
        {
            return localToScreen(_vertexAnimationPart.getAnimatedVertexPosition());
        }
        
        override protected function getPixelColor():SFloat
        {	
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var diffuseColor : SFloat = _diffuseShaderPart.getDiffuseColor(false);
				
				var alphaThreshold : SFloat = meshBindings.getParameter(
					BasicProperties.ALPHA_THRESHOLD, 1
				);
				
				kill(subtract(0.5, lessThan(diffuseColor.w, alphaThreshold)));
			}
			
            return float4(0, 0, 0, 0);
        }
    }
}