package aerys.minko.render.effect.basic
{
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.BlendingSource;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.StencilAction;
	import aerys.minko.type.enum.TriangleCulling;
	
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
	public class BasicShader extends Shader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _diffuseShaderPart		: DiffuseShaderPart;
		
		protected function get diffuse() : DiffuseShaderPart
		{
			return _diffuseShaderPart;
		}
		
		protected function get vertexAnimation() : VertexAnimationShaderPart
		{
			return _vertexAnimationPart;
		}
		
		/**
		 * @param priority Default value is 0.
		 * @param target Default value is null.
		 */
		public function BasicShader(target		: RenderTarget	= null,
									priority	: Number		= 0.)
		{
			super(target, priority);
			
			// init shader parts
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
			_diffuseShaderPart		= new DiffuseShaderPart(this);
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);
			
			// depth test
			settings.depthWriteEnabled = meshBindings.getConstant(
				BasicProperties.DEPTH_WRITE_ENABLED, true
			);
			settings.depthTest = meshBindings.getConstant(
				BasicProperties.DEPTH_TEST, DepthTest.LESS
			);
			
			settings.triangleCulling = meshBindings.getConstant(
				BasicProperties.TRIANGLE_CULLING, TriangleCulling.BACK
			);

			// stencil operations
			settings.stencilTriangleFace = meshBindings.getConstant(
				BasicProperties.STENCIL_TRIANGLE_FACE, TriangleCulling.BOTH
			);
			settings.stencilCompareMode = meshBindings.getConstant(
				BasicProperties.STENCIL_COMPARE_MODE, DepthTest.EQUAL
			);
			settings.stencilActionOnBothPass = meshBindings.getConstant(
				BasicProperties.STENCIL_ACTION_BOTH_PASS, StencilAction.KEEP
			);
			settings.stencilActionOnDepthFail = meshBindings.getConstant(
				BasicProperties.STENCIL_ACTION_DEPTH_FAIL, StencilAction.KEEP
			);
			settings.stencilActionOnDepthPassStencilFail = meshBindings.getConstant(
				BasicProperties.STENCIL_ACTION_DEPTH_PASS_STENCIL_FAIL, StencilAction.KEEP
			);
			settings.stencilReferenceValue = meshBindings.getConstant(
				BasicProperties.STENCIL_REFERENCE_VALUE, 0
			);
			settings.stencilReadMask = meshBindings.getConstant(
				BasicProperties.STENCIL_READ_MASK, 255
			);
			settings.stencilWriteMask = meshBindings.getConstant(
				BasicProperties.STENCIL_WRITE_MASK, 255
			);
			
			// blending and priority
			var blending : uint = meshBindings.getConstant(
				BasicProperties.BLENDING, Blending.NORMAL
			);

			if ((blending & 0xff) == BlendingSource.SOURCE_ALPHA)
			{
				settings.priority -= 0.5;
				settings.depthSortDrawCalls = true;
			}
			
			settings.blending			= blending;
			settings.enabled			= true;
			settings.scissorRectangle	= null;
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
		 */
		override protected function getPixelColor() : SFloat
		{
			var diffuse			: SFloat = _diffuseShaderPart.getDiffuse();
			var vertexNormal	: SFloat = _vertexAnimationPart.getAnimatedVertexNormal();
			
			// directional lighting
			if (sceneBindings.getConstant('lightEnabled', false)
				&& meshBindings.getConstant('lightEnabled', false))
			{
				var lightDirection	: SFloat = sceneBindings.getParameter("lightDirection", 3);
				var normal			: SFloat = normalize(
					interpolate(
						float4(multiply3x3(vertexNormal, localToWorldMatrix), 1)
					)
				);
				var lambert			: SFloat = saturate(negate(dotProduct3(
					normal,
					normalize(lightDirection)
				)));
				
				lambert.scaleBy(sceneBindings.getParameter("lightDiffuse", 1));
				
				var lightColor		: SFloat = add(
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
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var alphaThreshold : SFloat = meshBindings.getParameter('alphaThreshold', 1);
				
				kill(subtract(0.5, lessThan(diffuse.w, alphaThreshold)));
			}
			
			return diffuse;
		}
	}
}
