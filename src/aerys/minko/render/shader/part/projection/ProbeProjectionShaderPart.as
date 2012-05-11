package aerys.minko.render.shader.part.projection
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	
	import flash.geom.Rectangle;
	
	/**
	 * @author Romain Gilliotte
	 */	
	public class ProbeProjectionShaderPart extends ShaderPart implements IProjectionShaderPart
	{
		private static const CLIPSPACE_RECTANGLE : Rectangle = new Rectangle(-1, -1, 2, 2);
		
		public function ProbeProjectionShaderPart(main : Shader)
		{
			super(main);
		}

		public function projectVector(vector	: SFloat,
									  target	: Rectangle,
									  zNear		: Number	= 0,
									  zFar		: Number	= 1000) : SFloat
		{
			target ||= new Rectangle(0, 0, 1, 1);
			
			vector = normalize(vector);
			
			var m				: SFloat = rsqrt(multiply(2, add(vector.z, 1)));
			var projectedVector : SFloat = multiply(vector.xy, m);
			
			// transform to target coordinate system
			projectedVector = transform2DCoordinates(projectedVector, CLIPSPACE_RECTANGLE, target);
			
			return projectedVector;
		}
		
		public function unprojectVector(projectedVector : SFloat,
										source			: Rectangle) : SFloat
		{
			source ||= new Rectangle(0, 0, 1, 1);
			
			// transform to target coordinate system
			projectedVector = transform2DCoordinates(projectedVector, source, CLIPSPACE_RECTANGLE);
			
			// Resolve equation to unproject coordinates
			// 		xt = x / sqrt(2*(z+1))
			// 		yt = y / sqrt(2*(z+1))
			// 		x^2 + y^2 + z^2 - 1 = 0
			// 
			// => z^2 + z(2*xt^2 + 2*yt^2) + 2*xt^2 + 2*yt^2 - 1 = 0 (for z != -1)
			var xt2PlusYt2	: SFloat = dotProduct2(projectedVector, projectedVector);
			var delta		: SFloat = multiply(4, add(1, multiply(xt2PlusYt2, subtract(xt2PlusYt2, 2))));
			
			// z = (-b + sqrt(delta)) / (2a)
			var z			: SFloat = multiply(1 / 2, add(negate(xt2PlusYt2), sqrt(delta)));
			var xy			: SFloat = multiply(projectedVector, sqrt(multiply(2, add(z, 1))));
			
			return float3(xy, z);
		}
	}
}