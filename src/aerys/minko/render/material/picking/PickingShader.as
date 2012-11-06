package aerys.minko.render.material.picking
{
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderSettings;
	
	import flash.geom.Rectangle;
	
	public final class PickingShader extends BasicShader
	{
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
			super.initializeSettings(settings);
			
			settings.priority = Number.MAX_VALUE;
			settings.scissorRectangle = new Rectangle(0, 0, 1, 1);
//			settings.enabled = meshBindings.propertyExists('pickingId');
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return multiply4x4(
				localToView(vertexAnimation.getAnimatedVertexPosition()),
				sceneBindings.getParameter('pickingProjection', 16)
			);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(meshBindings.getParameter('pickingId', 3), 1);
		}
	}
}