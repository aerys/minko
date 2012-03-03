package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.parts.PixelColorShaderPart;
	import aerys.minko.render.shader.parts.animation.VertexAnimationShaderPart;
	import aerys.minko.type.enum.Blending;
	
	/**
	 * The BasicShader is a simple shader program handling hardware vertex
	 * animations (skinning and morphing), a diffuse color or a texture and
	 * a directional light.
	 * 
	 * Scene bindings:
	 * <table>
	 * <tr><td>Property Name</td><td>Type</td></tr>
	 * <tr><td>lightEnabled</td><td>Boolean</td></tr>
	 * <tr><td>lightDirection</td><td>Vector4</td></tr>
	 * <tr><td>lightAmbient</td><td>Number</td></tr>
	 * <tr><td>lightAmbientColor</td>uint or Vector4<td></td></tr>
	 * <tr><td>lightDiffuse</td><td>Number</td></tr>
	 * <tr><td>lightDiffuseColor</td><td>uint or Vector4</td></tr>
	 * </table>
	 * 
	 * Mesh bindings:
	 * <table>
	 * <tr><td>Property Name</td><td>Type</td></tr>
	 * <tr><td>lightEnabled</td><td>Boolean</td></tr>
	 * <tr><td>diffuseMap</td><td>TextureResource</td></tr>
	 * <tr><td>diffuseColor</td><td>uint or Vector4</td></tr>
	 * </table>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class BasicShader extends ActionScriptShader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _pixelColorPart			: PixelColorShaderPart;
		
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
			
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
			_pixelColorPart			= new PixelColorShaderPart(this);
		}
		
		/**
		 * The getVertexPosition() method is called to evaluate the vertex shader
		 * program that shall be executed on the GPU.
		 *  
		 * @return The position of the vertex in clip space (normalized screen space).
		 * 
		 */
		override protected function getVertexPosition() : SFloat
		{
			return localToScreen(
				_vertexAnimationPart.getAnimatedVertexPosition()
			);
		}
		
		/**
		 * The getPixelColor() method is called to evaluate the fragment shader
		 * program that shall be executed on the GPU.
		 *  
		 * @return The color of the pixel on the screen.
		 * 
		 */
		override protected function getPixelColor() : SFloat
		{
			var diffuse	: SFloat	= _pixelColorPart.getPixelColor();
			
			if (sceneBindings.propertyExists("lightEnabled")
				&& sceneBindings.getProperty("lightEnabled") === true
				&& meshBindings.propertyExists("lightEnabled")
				&& meshBindings.getProperty("lightEnabled") === true)
			{
				var lightDirection		: SFloat	= sceneBindings.getParameter("lightDirection", 3);
				var lambert				: SFloat	= saturate(negate(dotProduct3(
					normalize(interpolate(vertexNormal)),
					normalize(lightDirection)
				)));
				
				lambert.scaleBy(sceneBindings.getParameter("lightDiffuse", 1));
				
				var lightColor	: SFloat	= add(
					// ambient
					multiply(
						sceneBindings.getParameter("lightAmbient", 1),
						sceneBindings.getParameter("lightAmbientColor", 3)
					),
					// diffuse
					multiply(
						lambert,
						sceneBindings.getParameter("lightDiffuseColor", 3)
					)
				);
				
				diffuse = float4(multiply(diffuse.rgb, lightColor), diffuse.a);
			}
			
			return diffuse;
		}
	}
}