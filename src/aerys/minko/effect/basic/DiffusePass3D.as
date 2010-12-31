package aerys.minko.effect.basic
{
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.shader.DefaultShader3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	import flash.display3D.Context3DProgramType;
	
	public class DiffusePass3D implements IEffect3DPass
	{
		private static const SHADER	: Shader3D	= new DefaultShader3D();
		private static const STATES	: uint		= RenderState.VERTEX_FORMAT
												  | RenderState.SHADER;
		
		public function DiffusePass3D()
		{
		}
		
		public function begin(renderer : IRenderer3D, style : Object = null) : void
		{
			var states : RenderStatesManager = renderer.states;
			
			states.push(STATES);
			states.vertexFormat = Vertex3DFormat.XYZ_UV;
			states.shader = SHADER;
			states.lock(STATES);
			
			renderer.setMatrix(0,
							   renderer.transform.localToScreen,
							   Context3DProgramType.VERTEX);
		}
		
		public function end(renderer : IRenderer3D, style : Object = null) : void
		{
			renderer.states.pop();
		}
	}
}