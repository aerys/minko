package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.parts.SkinningShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	
	public class BasicShader extends ActionScriptShader
	{
		private var _skinning	: SkinningShaderPart	= null;
		
		/**
		 * 
		 * @param blending Default value is Blending.NORMAL.
		 * @param triangleCulling Default value is TriangleCulling.FRONT.
		 * @param priority Default value is 0.
		 * @param target Default value is null.
		 * 
		 */
		public function BasicShader(blending		: uint			= 524290,
									triangleCulling	: uint			= 2,
									priority		: Number		= 0,
									target			: RenderTarget	= null)
		{
			if (blending == Blending.ALPHA)
				priority -= 0.5;
			
			super(blending, triangleCulling, priority, target);
			
			_skinning = new SkinningShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var vertexPosition	: SFloat = vertexXYZ;
			
			if (meshBindings.propertyExists("skinningMethod"))
				vertexPosition = _skinning.skinPosition(vertexPosition);
			
			return localToScreen(vertexPosition);
		}
		
		override protected function getPixelColor() : SFloat
		{
			if (meshBindings.propertyExists("diffuseMap"))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter("diffuseMap");
				var uv			: SFloat	= interpolate(vertexUV);
				
				return sampleTexture(diffuseMap, uv);
			}
			else if (meshBindings.propertyExists("diffuseColor"))
			{
				return meshBindings.getParameter("diffuseColor", 4);
			}
			
			throw new Error(
				"Property 'diffuseColor' or 'diffuseMap' must be set."
			);
		}
	}
}