package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.render.shader.part.coordinates.SphericalCoordinatesShaderPart;
	
	import flash.geom.Rectangle;
	
	/**
	 * Blinn-Newell projection and deprojection.
	 * It simply maps spherical coordinates to the plane (theta is x, phi is y).
	 * 
	 * @author Romain Gilliotte
	 */	
	public class BlinnNewellProjectionShaderPart extends ShaderPart implements IProjectionShaderPart
	{
		private var _sphericalPart : SphericalCoordinatesShaderPart;
		
		public function BlinnNewellProjectionShaderPart(main : ActionScriptShader)
		{
			super(main);
			
			_sphericalPart = new SphericalCoordinatesShaderPart(main);
		}
		
		/**
		 * Project a vector into a plane with Blinn-Newell projection
		 *  
		 * @param vector The vector to be projected
		 * @param target The rectangle to project into. Can be new Rectangle(0, 0, 1, 1) for UV projection, or new Rectangle(-1, -1, 2, 2) for clipspace projection.
		 * @return The projected vector (is a 3d vector, with depth).
		 */		
		public function projectVector(vector	: SFloat, 
									  target	: Rectangle = null,
									  zNear		: Number	= 0,
									  zFar		: Number	= 1000) : SFloat
		{
			target ||= new Rectangle(0, 0, 1, 1);
			
			var sphericalCoordinates	: SFloat = _sphericalPart.fromOrtho(vector);
			var projectedVector			: SFloat = sphericalCoordinates 
				.zy
				.scaleBy(float2(target.width / 2 / Math.PI, target.height / Math.PI))
				.incrementBy(float2(target.x, target.y));
			
			return projectedVector;
		}
		
		/**
		 * Unproject a vector from a plane with Blinn-Newell projection
		 * 
		 * @param projectedVector The vector to be unprojected
		 * @param source The rectangle to unproject from. Should be new Rectangle(0, 0, 1, 1) most of the time (if used to read from a texture).
		 * @return The unprojected vector (which is normalized).
		 */
		public function unprojectVector(projectedVector	: SFloat,
										source			: Rectangle = null) : SFloat
		{
			source ||= new Rectangle(0, 0, 1, 1);
			
			var sphericalCoordinates : SFloat = float3(
				1,
				projectedVector
					.decrementBy(float2(source.x, source.y))
					.scaleBy(float2(2 * Math.PI / source.width, Math.PI / source.height))
					.yx
			);
			
			var unprojectedVector : SFloat = _sphericalPart.toOrtho(sphericalCoordinates);
			
			return unprojectedVector;
		}
	}
}
