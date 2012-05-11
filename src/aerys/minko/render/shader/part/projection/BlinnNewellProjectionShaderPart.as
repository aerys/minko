package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
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
		private static const SPHERICAL_COORDINATES_RECTANGLE : Rectangle = new Rectangle(0, 0, 2 * Math.PI, Math.PI);
		
		private var _sphericalPart : SphericalCoordinatesShaderPart;
		
		public function BlinnNewellProjectionShaderPart(main : Shader)
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
									  target	: Rectangle,
									  zNear		: Number	= 0,
									  zFar		: Number	= 1000) : SFloat
		{
			var sphericalCoordinates	: SFloat = _sphericalPart.fromOrtho(vector);
			var projectedVector			: SFloat = sphericalCoordinates.zy;
			
			return transform2DCoordinates(projectedVector, SPHERICAL_COORDINATES_RECTANGLE, target);
		}
		
		/**
		 * Unproject a vector from a plane with Blinn-Newell projection
		 * 
		 * @param projectedVector The vector to be unprojected
		 * @param source The rectangle to unproject from. Should be new Rectangle(0, 0, 1, 1) most of the time (if used to read from a texture).
		 * @return The unprojected vector (which is normalized).
		 */
		public function unprojectVector(projectedVector	: SFloat,
										source			: Rectangle) : SFloat
		{
			var coordinates				: SFloat = transform2DCoordinates(projectedVector, source, SPHERICAL_COORDINATES_RECTANGLE);
			var unprojectedVector		: SFloat = _sphericalPart.toOrtho(float3(1, coordinates.yx));
			
			return unprojectedVector;
		}
	}
}
