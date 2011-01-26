package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.effect.IEffect3DStyle;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.shader.DefaultShader3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.BlendingSource;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	import flash.display3D.Context3DProgramType;
	import flash.display3D.textures.Texture;
	
	public class DiffusePass3D implements IEffect3DPass
	{
		private static const SHADER	: Shader3D	= new DefaultShader3D();
		
		public function DiffusePass3D()
		{
		}
		
		public function begin(renderer : IRenderer3D, style : IEffect3DStyle) : Boolean
		{
			var states 		: RenderStatesManager 	= renderer.states;
			var diffuse		: Texture				= style.get(BasicStyle3D.DIFFUSE)
													  as Texture;
			var toScreen	: Matrix4x4				= style.get(BasicStyle3D.LOCAL_TO_SCREEN)
													  as Matrix4x4;
			var blending	: uint					= style.get(BasicStyle3D.BLENDING) as uint
													  || Blending.NORMAL;
			
			states.push();
			states.vertexFormat = Vertex3DFormat.XYZ_UV;
			states.shader = SHADER;
			states.blending = blending;
			
			renderer.setTexture(0, diffuse);
			renderer.setMatrix(0,
							   Context3DProgramType.VERTEX,
							   toScreen);
			
			return true;
		}
		
		public function end(renderer : IRenderer3D, style : IEffect3DStyle) : void
		{
			renderer.states.pop();
		}
	}
}