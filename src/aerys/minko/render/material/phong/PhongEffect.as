package aerys.minko.render.material.phong
{
	import aerys.minko.render.shader.Shader;
	
	public class PhongEffect extends AbstractShadowMappingEffect
	{
		public function PhongEffect(shader : Shader = null)
		{
			super(shader || new PhongShader());
		}
	}
}