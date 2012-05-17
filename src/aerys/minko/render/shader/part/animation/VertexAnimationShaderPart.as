package aerys.minko.render.shader.part.animation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.stream.format.VertexComponent;
	
	/**
	 * The shader part to handle all vertex animations (morphing/skinning).
	 * 
	 * @author Romain Gilliotte
	 * 
	 */
	public class VertexAnimationShaderPart extends ShaderPart
	{
		private var _morphing : MorphingShaderPart	= null;
		private var _skinning : SkinningShaderPart	= null;
		
		public function VertexAnimationShaderPart(main : Shader)
		{
			super(main);
			
			_morphing = new MorphingShaderPart(main);
			_skinning = new SkinningShaderPart(main);
		}
		
		public function getAnimatedVertexPosition() : SFloat
		{
			var vertexPosition : SFloat;
			
			if (meshBindings.propertyExists('morphingEnabled'))
				vertexPosition = _morphing.getMorphedPosition();
			else
				vertexPosition = getVertexAttribute(VertexComponent.XYZ);
			
			if (meshBindings.propertyExists('skinningMethod'))
				vertexPosition = _skinning.skinPosition(vertexPosition);
			
			return vertexPosition;
		}
		
		public function getAnimatedVertexNormal() : SFloat
		{
			var vertexNormal : SFloat;
			
			if (meshBindings.propertyExists('morphingEnabled'))
				vertexNormal = _morphing.getMorphedNormal();
			else
				vertexNormal = getVertexAttribute(VertexComponent.NORMAL)
			
			if (meshBindings.propertyExists('skinningMethod'))
				vertexNormal = _skinning.skinNormal(vertexNormal);
			
			return vertexNormal;
		}
	}
}