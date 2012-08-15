package aerys.minko.render.shader.part.animation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	
	/**
	 * The shader part to handle morphing vertex animations.
	 * @author Romain Gilliotte
	 */
	public class MorphingShaderPart extends ShaderPart
	{
		public function MorphingShaderPart(main:Shader)
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
		
		public function getMorphedTangent() : SFloat
		{
			return getVertexAttribute(VertexComponent.TANGENT);
		}
	}
}
