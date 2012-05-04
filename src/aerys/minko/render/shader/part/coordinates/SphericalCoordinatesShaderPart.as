package aerys.minko.render.shader.part.coordinates
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.ShaderPart;
	
	/**
	 * Convert orthographic to spherical coordinates and vice versa
	 * 
	 * @author Romain Gilliotte
	 */	
	public class SphericalCoordinatesShaderPart extends ShaderPart
	{
		public function SphericalCoordinatesShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function toOrtho(coords : SFloat) : SFloat
		{
			// x = r * sin(phi) * cos(theta)
			// y = r * sin(phi) * sin(theta)
			// z = p * cos(phi)
			
			var r		: SFloat = coords.x;
			var phi		: SFloat = coords.y;
			var theta	: SFloat = coords.z;
			
			var x		: SFloat = multiply(r, sin(phi), cos(theta));
			var y		: SFloat = multiply(r, sin(phi), sin(theta));
			var z		: SFloat = multiply(r, cos(phi));
			
			return float3(z, x, y);
		}
		
		public function fromOrtho(coords : SFloat) : SFloat
		{
			// r = sqrt(x ^ 2 + y ^ 2 + z ^ 2)
			// phi = arccos(z / r)
			// if y >= 0, theta =arccos(x / sqrt(x ^ 2 + y ^ 2))
			// if y < 0, theta = 2 * PI * arccos(x / sqrt(x ^ 2 + y ^ 2))
			
			coords = coords.zxy;
			
			var x		: SFloat = coords.x;
			var y		: SFloat = coords.y;
			var z		: SFloat = coords.z;
			var xy		: SFloat = coords.xy;
			
			var ySup0	: SFloat = greaterEqual(y, 0);
			
			var r		: SFloat = sqrt(dotProduct3(coords, coords));
			
			var phi		: SFloat;
			var theta	: SFloat;
			
			phi		= divide(z, r);
			theta	= divide(x, sqrt(dotProduct2(xy, xy)));
			
			// group phi and theta to compute acos, as it is an expensive operation
			var tmpFloat2 : SFloat = acos(float2(phi, theta), 0);
			
			phi			= tmpFloat2.x;
			theta		= tmpFloat2.y;
			
			theta = add(
				multiply(ySup0, theta), 
				multiply(
					subtract(1, ySup0), 
					subtract(2 * Math.PI, theta)
				)
			);
			
			return float3(r, phi, theta);
		}
	}
}