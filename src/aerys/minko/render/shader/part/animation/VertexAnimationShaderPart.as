package aerys.minko.render.shader.part.animation
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
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
		private var _morphingShaderPart : MorphingShaderPart;
		private var _skinningShaderPart : SkinningShaderPart;
		
		public function VertexAnimationShaderPart(main : ActionScriptShader)
		{
			super(main);
			
			_skinningShaderPart = new SkinningShaderPart(main);
		}
		
		public function getAnimatedVertexPosition() : SFloat
		{
			var vertexPosition : SFloat;
			
			if (meshBindings.propertyExists("morphingEnabled"))
				vertexPosition = _morphingShaderPart.getMorphedPosition();
			else
				vertexPosition = getVertexAttribute(VertexComponent.XYZ);
			
			if (meshBindings.propertyExists("skinningMethod"))
				vertexPosition = _skinningShaderPart.skinPosition(vertexPosition);
			
			return vertexPosition;
		}
		
		public function getAnimatedVertexNormal() : SFloat
		{
			var vertexNormal : SFloat;
			
			if (meshBindings.propertyExists("morphingEnabled"))
				vertexNormal = _morphingShaderPart.getMorphedNormal();
			else
				vertexNormal = getVertexAttribute(VertexComponent.NORMAL)
			
			if (meshBindings.propertyExists("skinningMethod"))
				vertexNormal = _skinningShaderPart.skinNormal(vertexNormal);
			
			return vertexNormal;
		}
	}
}