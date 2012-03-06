package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.PixelColorShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.type.enum.Blending;
	
	/**
	 * The BasicShader is a simple shader program handling hardware vertex
	 * animations (skinning and morphing), a diffuse color or a texture and
	 * a directional light.
	 * 
	 * <table class="bindingsSummary">
	 * <tr>
	 * 	<th>Property Name</th>
	 * 	<th>Source</th>
	 * 	<th>Type</th>
	 * 	<th>Description</th>
	 * 	<th>Requires</th>
	 * </tr>
	 * <tr>
	 * 	<td>lightEnabled</td>
	 * 	<td>Scene</td>
	 * 	<td>Boolean</td>
	 * 	<td>Whether the light is enabled or not on the scene.</td>
	 * 	<td>lightDirection, lightAmbient, lightAmbientColor, lightDiffuse, lightDiffuseColor</td>
	 * </tr>
	 * <tr>
	 * 	<td>lightDirection</td>
	 * 	<td>Scene</td>
	 * 	<td>Vector4</td>
	 * 	<td>The direction of the light.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>lightAmbient</td>
	 * 	<td>Scene</td>
	 * 	<td>Number</td>
	 * 	<td>The ambient factor of the light.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>lightAmbientColor</td>
	 * 	<td>Scene</td>
	 * 	<td>uint or Vector4</td>
	 * 	<td>The ambient color of the light.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>lightDiffuse</td>
	 * 	<td>Scene</td>
	 * 	<td>Number</td>
	 * 	<td>The diffuse factor of the light</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>lightDiffuseColor</td>
	 * 	<td>Scene</td>
	 * 	<td>uint or Vector4</td>
	 * 	<td>The diffuse color of the light.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>lightEnabled</td>
	 * 	<td>Mesh</td>
	 * 	<td>Boolean</td>
	 * 	<td>Whether the light is enabled or not on the mesh.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>diffuseMap</td>
	 * 	<td>Mesh</td>
	 * 	<td>TextureResource</td>
	 * 	<td>The texture to use for rendering.</td>
	 * 	<td></td>
	 * </tr>
	 * <tr>
	 * 	<td>diffuseColor</td>
	 * 	<td>Mesh</td>
	 * 	<td>uint or Vector4</td>
	 * 	<td>The color to use for rendering. If the "diffuseMap" binding is set, this
	 * 	value is not used.</td>
	 * 	<td></td>
	 * </tr>
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
		 * @param triangleCulling Default value is TriangleCulling.BACK.
		 * @param priority Default value is 0.
		 * @param target Default value is null.
		 * 
		 */
		public function BasicShader(blending		: uint			= 524290,
									triangleCulling	: uint			= 1,
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
		 * @return The position of the vertex in clip space (normalized
		 * screen space).
		 * 
		 */
		override protected function getVertexPosition() : SFloat
		{
			return localToScreen(
				_vertexAnimationPart.getAnimatedVertexPosition()
			);
		}
		
		/**
		 * @return The pixel color using a diffuse color/map and an optional
		 * directional light.
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
				var normal				: SFloat	= normalize(
					interpolate(
						float4(multiply3x3(vertexNormal, localToWorldMatrix), 1)
					)
				);
				var lambert				: SFloat	= saturate(negate(dotProduct3(
					normal,
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