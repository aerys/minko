package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.effect.StyleStack;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.shader.DefaultShader;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.CompareMode;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display3D.textures.Texture;
	
	public class DiffusePass implements IEffectPass
	{
		private static const DEFAULT_SHADER	: Shader	= new DefaultShader();
		private static const DEPTH_TEST		: int		= CompareMode.LESS | CompareMode.EQUAL;
		
		public function DiffusePass()
		{
		}
		
		public function begin(renderer : IRenderer, style : StyleStack) : Boolean
		{
			var state 		: RenderState 	= renderer.state;
			var diffuse		: Texture		= style.get(BasicStyle.DIFFUSE_MAP, null)
											  as Texture;
			
			if (!diffuse)
				return false;
			
			var toScreen	: Matrix4x4		= style.get(BasicStyle.LOCAL_TO_SCREEN_MATRIX)
											  as Matrix4x4;
			var blending	: uint			= style.get(BasicStyle.BLENDING, Blending.NORMAL) as uint;
			
			if (!diffuse)
				return false;
			
			state.shader = DEFAULT_SHADER;
			state.blending = blending;
			state.depthMask = DEPTH_TEST;
			
			state.setTextureAt(0, diffuse);
			
			state.setVertexConstantMatrix(0, toScreen);
			
			return true;
		}
		
		public function end(renderer : IRenderer, style : StyleStack) : void
		{
			// NOTHING
		}
	}
}