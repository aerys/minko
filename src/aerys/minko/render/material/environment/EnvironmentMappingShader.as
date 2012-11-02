package aerys.minko.render.material.environment
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
	
	public class EnvironmentMappingShader extends BasicShader
	{
		private var _reflectionPart	: EnvironmentMappingShaderPart;
		
		public function EnvironmentMappingShader(renderTarget	: RenderTarget 	= null,
										 		 priority		: Number 		= 0)
		{
			super(renderTarget, priority);
			
			// init needed shader parts
			_reflectionPart	= new EnvironmentMappingShaderPart(this);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return _reflectionPart.applyEnvironmentMapping(super.getPixelColor());
		}
	}
}
