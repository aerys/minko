package aerys.minko.render.shader.parts.animation
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderPart;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class MorphingShaderPart extends ShaderPart
	{
		public function MorphingShaderPart(main:ActionScriptShader)
		{
			super(main);
		}
		
		public function getMorphedPosition() : SFloat
		{
			return getVertexAttribute(VertexComponent.XYZ);
		}
		
		public function getMorphedNormal() : SFloat
		{
			return getVertexAttribute(VertexComponent.NORMAL);
		}
		
	}
}