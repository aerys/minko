package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3D;
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	
	public class BasicEffect3D extends AbstractEffect3D
	{
		public function BasicEffect3D()
		{
			super();
			
			passes[0] = new DiffusePass3D();
		}
		
	}
}
