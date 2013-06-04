package aerys.minko.render.material.realistic
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.phong.PhongEffect;
    
    public class RealisticEffect extends PhongEffect
    {
        public function RealisticEffect(renderTarget : RenderTarget = null)
        {
            super(
				renderTarget,
                new RealisticSinglePassShader(renderTarget, 0.),
                new RealisticBaseShader(renderTarget, .5)
            );
        }
    }
}