package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.SFloat;
	
	import flash.geom.Rectangle;

	/**
	 * @author Romain Gilliotte
	 */	
	public interface IProjectionShaderPart
	{
		function projectVector(vector	: SFloat,
							   target	: Rectangle,
							   zNear	: Number		= 0,
							   zFar		: Number		= 1000) : SFloat;
		
		function unprojectVector(projectedVector	: SFloat,
								 source				: Rectangle) : SFloat;
	}
}
