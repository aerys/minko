package aerys.minko.render.material.realistic
{
    import aerys.minko.render.material.phong.PhongEffect;
    import aerys.minko.render.shader.Shader;
    
    public class RealisticEffect extends PhongEffect
    {
        public function RealisticEffect()
        {
            super(
                new RealisticSinglePassShader(null, 0.),
                new RealisticBaseShader(null, .5)
            );
        }
    }
}