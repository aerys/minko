package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.DefaultShader;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.CompareMode;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.visitor.data.Style;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display3D.textures.Texture;
	import flash.utils.Dictionary;
	
	public class DiffusePass implements IEffectPass
	{
		private static const DEFAULT_SHADER	: Shader	= new DefaultShader();
		private static const DEPTH_TEST		: int		= CompareMode.LESS | CompareMode.EQUAL;
		
		public function DiffusePass()
		{
		}
		
		
		public function fillRenderState(state	: RenderState,
										style	: StyleStack, 
										local	: TransformData, 
										world	: Dictionary) : Boolean
		{
			var diffuse		: TextureRessource		= style.get(BasicStyle.DIFFUSE_MAP) as TextureRessource;
			if (!diffuse)
				return false;
			
			var blending	: uint			= style.get(BasicStyle.BLENDING, Blending.NORMAL) as uint;
			
			var toScreen : Matrix4x4 = local.localToScreen;
			
			state.shader = DEFAULT_SHADER;
			state.blending = blending;
			state.depthMask = CompareMode.LESS;
			
			state.setTextureAt(0, diffuse);
			
			state.setVertexConstantMatrix(0, toScreen);
			
			return true;
		}
	}
}