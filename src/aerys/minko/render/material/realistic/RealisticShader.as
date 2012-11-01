package aerys.minko.render.material.realistic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.BlendingShaderPart;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
	import aerys.minko.render.shader.part.phong.LightAwareDiffuseShaderPart;
	import aerys.minko.render.shader.part.phong.PhongShaderPart;
	
	public class RealisticShader extends BasicShader
	{
		private var _diffuse			: LightAwareDiffuseShaderPart;
		private var _phong				: PhongShaderPart;
		private var _environmentMapping	: EnvironmentMappingShaderPart;
		private var _blending			: BlendingShaderPart;
		
		public function RealisticShader(priority		: Number		= 0.,
										renderTarget	: RenderTarget	= null)
		{
			super(renderTarget, priority);
			
			_diffuse			= new LightAwareDiffuseShaderPart(this);
			_phong				= new PhongShaderPart(this);
			_environmentMapping = new EnvironmentMappingShaderPart(this);
			_blending			= new BlendingShaderPart(this);
		}
		
		override protected function getPixelColor() : SFloat
		{
			var diffuse	: SFloat	= _diffuse.getDiffuseColor();
			
			diffuse = _environmentMapping.applyEnvironmentMapping(diffuse);
			diffuse = _phong.applyPhongLighting(diffuse);
			
			return diffuse;
		}
	}
}