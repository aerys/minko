package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.DefaultShader3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.CompareMode;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display3D.textures.Texture;
	import flash.utils.Dictionary;
	
	public class DiffusePass3D implements IEffect3DPass
	{
		private static const DEFAULT_SHADER	: Shader3D	= new DefaultShader3D();
		
		public function DiffusePass3D()
		{
		}
		
		public function fillRenderState(state	: RenderState,
										style	: StyleStack3D, 
										local	: TransformData, 
										world	: Dictionary) : Boolean
		{
			var diffuse		: TextureRessource		= style.get(BasicStyle3D.DIFFUSE_MAP) as TextureRessource;
			if (!diffuse)
				return false;
			
			var blending	: uint			= style.get(BasicStyle3D.BLENDING, Blending.NORMAL) as uint;
			
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