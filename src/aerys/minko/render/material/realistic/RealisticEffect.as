package aerys.minko.render.material.realistic
{
    import aerys.minko.render.material.phong.PhongEffect;
    
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