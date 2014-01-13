package aerys.minko.render.shader.picking
{
	import flash.geom.Rectangle;
	
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.render.shader.part.animation.VertexAnimationShaderPart;
	import aerys.minko.type.enum.TriangleCulling;
	
	public final class PickingShader extends Shader
	{
		private var _vertexAnimation 	: VertexAnimationShaderPart;
		private var _diffuse			: DiffuseShaderPart;
		
		public function PickingShader()
		{
			super(null, Number.MAX_VALUE);
			
			_vertexAnimation = new VertexAnimationShaderPart(this);
			_diffuse = new DiffuseShaderPart(this);
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);
			
			settings.triangleCulling = meshBindings.propertyExists('triangleCulling') ? meshBindings.getProperty('triangleCulling') : TriangleCulling.BACK;
			settings.priority = Number.MAX_VALUE;
			settings.scissorRectangle = new Rectangle(0, 0, 1, 1);
			settings.enabled = meshBindings.propertyExists('pickingId');
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return multiply4x4(
				worldToView(localToWorld(_vertexAnimation.getAnimatedVertexPosition())),
				sceneBindings.getParameter('pickingProjection', 16)
			);
		}
		
		override protected function getPixelColor() : SFloat
		{
			// only to kill transparent pixels according to diffuse's alpha
			_diffuse.getDiffuseColor(true);
			
			return float4(meshBindings.getParameter('pickingId', 3), 1);
		}
	}
}