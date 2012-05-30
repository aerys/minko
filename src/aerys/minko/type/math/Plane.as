package aerys.minko.type.math
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	
	import flash.geom.Vector3D;

	/**
	 * The Plane class represents a geometrical plane through its equation
	 * ax + by + cz + d = 0 where (a, b, c) is the normal of the plane and
	 * d the distance along that normal.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public final class Plane
	{
		use namespace minko_math;

		//{ region consts
		public static const POINT_INFRONT		: uint		= 1;
		public static const POINT_BEHIND		: uint		= 2;
		public static const POINT_COINCIDING	: uint		= 4;

		public static const A_INFRONT			: uint		= POINT_INFRONT	<< 16;
		public static const B_INFRONT			: uint		= POINT_INFRONT	<< 8;
		public static const C_INFRONT			: uint		= POINT_INFRONT;

		public static const A_BEHIND			: uint		= POINT_BEHIND << 16;
		public static const B_BEHIND			: uint		= POINT_BEHIND << 8;
		public static const C_BEHIND			: uint		= POINT_BEHIND;

		public static const A_COINCIDING		: uint		= POINT_COINCIDING << 16;
		public static const B_COINCIDING		: uint		= POINT_COINCIDING << 8;
		public static const C_COINCIDING		: uint		= POINT_COINCIDING;

		private static const AB_INFRONT			: uint		= A_INFRONT | B_INFRONT | C_BEHIND;
		private static const BC_INFRONT			: uint		= A_BEHIND | B_INFRONT | C_INFRONT;
		private static const CA_INFRONT			: uint		= A_INFRONT | B_BEHIND | C_INFRONT;

		private static const AB_BEHIND			: uint		= A_BEHIND | B_BEHIND | C_INFRONT;
		private static const BC_BEHIND			: uint		= A_INFRONT | B_BEHIND | C_BEHIND;
		private static const CA_BEHIND			: uint		= A_BEHIND | B_INFRONT | C_BEHIND;

		public static const POLYGON_INFRONT		: uint		= A_INFRONT | B_INFRONT | C_INFRONT;
		public static const POLYGON_BEHIND		: uint		= A_BEHIND | B_BEHIND | C_BEHIND;
		public static const POLYGON_COINCIDING	: uint		= A_COINCIDING | B_COINCIDING | C_COINCIDING;

		public static const OPPOSITE			: uint		= 1 << 24;
		public static const COLINEAR			: uint		= 2 << 24;

		public static const DEFAULT_THICKNESS	: Number	= .01;
		//} endregion

		minko_math var _a		: Number	= 0.;
		minko_math var _b		: Number	= 0.;
		minko_math var _c		: Number	= 0.;
		minko_math var _d		: Number	= 0.;
		minko_math var _normal	: Vector4	= new Vector4();

		public function get a() : Number
		{
			return _a;
		}
		public function get b() : Number
		{
			return _b;
		}
		public function get c() : Number
		{
			return _c;
		}
		public function get d() : Number
		{
			return _d;
		}
		
		public function get normal() : Vector4
		{
			_normal.set(_a, _b, _c);
			return _normal;
		}
		
		/**
		 * Creates a new Plane object.
		 */
		public function Plane(a	: Number 	= 0.,
							  b	: Number 	= 0.,
							  c	: Number 	= 0.,
							  d	: Number 	= 0.)
		{
			_a = a;
			_b = b;
			_c = c;
			_d = d;

			normalize();
		}
		
		/**
		 * Create a new Plane from a polygon defined by 3 set of (x, _b, _c) tuples.
		 * 
		 * @return The Plane containing the specified polygon.
		 */
		static public function fromTriangle(x1 : Number, y1 : Number, z1 : Number,
											x2 : Number, y2 : Number, z2 : Number,
											x3 : Number, y3 : Number, z3 : Number) : Plane
		{
			var plane : Plane = new Plane();
			
			plane.setFromTriangle(
				x1, y1, z1,
				x2, y2, z2,
				x3, y3, z3
			);
			
			return plane;
		}
		
		public function setFromTriangle(x1 : Number, y1 : Number, z1 : Number,
										x2 : Number, y2 : Number, z2 : Number,
										x3 : Number, y3 : Number, z3 : Number) : void
		{
			var nx : Number = (y1 - y3) * (z1 - z2) - (z1 - z3) * (y1 - y2);
			var ny : Number = (z1 - z3) * (x1 - x2) - (x1 - x3) * (z1 - z2);
			var nz : Number = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
			
			_a = nx;
			_b = ny;
			_c = nz;
			_d = x1 * nx + y1 * ny + z1 * nz;
			
			normalize();
		}
		
		public function set(a : Number, b : Number, c : Number, d : Number) : void
		{
			_a = a;
			_b = b;
			_c = c;
			_d = d;
		}
			
		public function intersectWith(plane1 : Plane, 
									  plane2 : Plane) : Vector4
		{
			var X : uint = 0;
			var Y : uint = 1;
			var Z : uint = 2;
			
			var s : Vector.<Vector.<Number>>;
			
			s = createGaussianFromPlanes(this, plane1, plane2);
			sortGaussian(s);
			simplifyGaussian(s, 1, 0, X);	// remove x from (1)
			simplifyGaussian(s, 2, 0, X);	// remove x from (2)
			simplifyGaussian(s, 2, 1, Y);	// remove y from (2)
			simplifyGaussian(s, 1, 2, Z);	// remove z from (1)
			simplifyGaussian(s, 0, 1, Y);	// remove y from (0)
			simplifyGaussian(s, 0, 2, Z);	// remove z from (0)
			
			return new Vector4(
				s[0][3] / s[0][0],
				s[1][3] / s[1][1],
				s[2][3] / s[2][2]
			);
		}
		
		public function normalize() : Number
		{
			var mag	: Number	= Math.sqrt(_a * _a + _b * _b + _c * _c);

			if (mag && mag != 1.)
			{
				_a /= mag;
				_b /= mag;
				_c /= mag;
				_d /= mag;
			}

			return mag;
		}

		/**
		 * @fixme This looks like wrong to me (romain). Where is the d component in there?
		 */		
		public function project(v : Vector4, out : Vector4 = null) : Vector4
		{
			var scale : Number = _a * v.x + _b * v.y + _c * v.z;
			
			out ||= new Vector4();
			out.set(
				v._vector.x - scale * _a,
				v._vector.y - scale * _b,
				v._vector.z - scale * _c
			);
			
			return out;
		}
		
		public function reflect(v : Vector4, out : Vector4 = null) : Vector4
		{
			out ||= new Vector4();
			
			// project origin (could be any other point, but we need a point on the plane)
			var opX : Number = _a * _d;
			var opY : Number = _b * _d;
			var opZ : Number = _c * _d;
			
			// om = op + pm, with p on the plane
			var pmX : Number = v._vector.x - opX;
			var pmY : Number = v._vector.y - opY;
			var pmZ : Number = v._vector.z - opZ;
			
			// reflect pm
			var dotProduct : Number = pmX * _a + pmY * _b + pmZ * _c; 
			var rpmX : Number = pmX - 2 * dotProduct * _a;
			var rpmY : Number = pmY - 2 * dotProduct * _b;
			var rpmZ : Number = pmZ - 2 * dotProduct * _c;
			
			// r(om) = r(op) + r(pm) = op + dr(pm)
			out.set(rpmX + opX, rpmY + opY, rpmZ + opZ, 0);
			
			return out;
		}
		
		public function deltaReflect(v : Vector4, out : Vector4 = null) : Vector4
		{
			out ||= new Vector4();
			
			var dotProduct : Number = v.x * _a + v.y * _b + v.z * _c; 
			
			out.set(
				v._vector.x - 2 * dotProduct * _a,
				v._vector.y - 2 * dotProduct * _b,
				v._vector.z - 2 * dotProduct * _c,
				0
			);
			
			return out;
		}
		
		/**
		 * Test a 3D point is coinciding, infront or behind the plane.
		 * @return
		 */
		public final function testPoint(x 			: Number,
										y 			: Number,
										z 			: Number,
										thickness 	: Number = DEFAULT_THICKNESS) : uint
		{
			var dist	: Number	= _a * x + _b * y + _c * z - _d;

			return (dist >= -thickness && dist <= thickness) ? POINT_COINCIDING
														   	 : (dist < 0. ? POINT_BEHIND
											 							  : POINT_INFRONT);
		}

		/**
		 * Test a 3D ray is coinciding, infront or behind the plane.
		 * @param	myOrigin
		 * @param	myTarget
		 * @return
		 */
		public final function testRay(ray : Ray) : uint
		{
			var origin		: Vector4 	= ray.origin;
			var target		: Vector4	= ray.direction;
			var distOrigin 	: Number 	= _a * origin._vector.x + _b * origin._vector.y + _c * origin._vector.z - _d;
			var distTarget 	: Number 	= _a * target._vector.x + _b * target._vector.y + _c * target._vector.z - _d;
			var r 			: uint 		= 0;

			if (distOrigin > 0.)
				r &= A_INFRONT;
			else if (distOrigin < 0.)
				r &= A_BEHIND;
			else
				r &= A_COINCIDING;

			if (distTarget > 0.)
				r &= B_INFRONT;
			else if (distTarget < 0.)
				r &= B_BEHIND;
			else
				r &= B_COINCIDING;

			return r;
		}

		/**
		 * Test a polygon (triangle) is coinciding, infront or behind the plane.
		 * @param	myA
		 * @param	myB
		 * @param	myC
		 * @param	myThickness
		 * @return
		 */
		public final function testTriangle(x1 : Number, y1 : Number, z1 : Number,
										   x2 : Number, y2 : Number, z2 : Number,
										   x3 : Number, y3 : Number, z3 : Number,
										   thickness 	: Number = DEFAULT_THICKNESS)	: uint
		{
			var test	: Number	= 0;
			var result  : uint 		= 0;
			var s		: Number	= -thickness;
			var t		: Number	= thickness;

			test = _a * x1 + _b * y1 + _c * z1 - _d;
			result = ((test >= s && test <= t) ? POINT_COINCIDING
											 : ((test < 0.) ? POINT_BEHIND
														    : POINT_INFRONT)) << 16;

			test = _a * x2 + _b * y2 + _c * z2 - _d;
			result |= ((test >= s && test <= t) ? POINT_COINCIDING
											  : ((test < 0.) ? POINT_BEHIND
														     : POINT_INFRONT)) << 8;

			test = _a * x3 + _b * y3 + _c * z3 - _d;
			result |= (test >= s && test <= t) ? POINT_COINCIDING
											 : ((test < 0.) ? POINT_BEHIND
														    : POINT_INFRONT);

			// if coplanar
			if (result == POLYGON_COINCIDING)
			{
				var x1x2 : Number = x1 - x2;
				var x1x3 : Number = x1 - x3;
				var y1y2 : Number = y1 - y2;
				var y1y3 : Number = y1 - y3;
				var z1z2 : Number = z1 - z2;
				var z1z3 : Number = z1 - z3;
				var dot : Number = _a * (y1y3 * z1z2 - z1z3 * y1y2)
								   + _b * (z1z3 * x1x2 - x1x3 * z1z2)
								   + _c * (x1x3 * y1y2 - y1y3 * x1x2);

				result |= dot > 0. ? COLINEAR : OPPOSITE;
			}

			return result;
		}

		public function clone() : Plane
		{
			return new Plane(_a, _b, _c, _d);
		}

		public function toString() : String
		{
			return "(" + _a + ", " + _b + ", "
				   + _c + ", " + _d + ")";
		}

		private function createGaussianFromPlanes(plane1 : Plane, 
												  plane2 : Plane, 
												  plane3 : Plane) : Vector.<Vector.<Number>>
		{
			var planes	: Vector.<Plane>			= Vector.<Plane>([plane1, plane2, plane3]);
			var s		: Vector.<Vector.<Number>>	= new Vector.<Vector.<Number>>(3, true);
			
			for (var lineId : uint = 0; lineId < 3; ++lineId)
			{
				var plane	: Plane				= planes[lineId];
				var line	: Vector.<Number>	= s[lineId] = new Vector.<Number>(4, true);
				
				line[0] = plane._a;
				line[1] = plane._b;
				line[2] = plane._c;
				line[3] = plane._d;
			}
			return s;
		}
		
		private function sortGaussian(s : Vector.<Vector.<Number>>) : void
		{
			var swapWith : uint = 1;
			for (var lineId : uint = 0; lineId < 3; ++lineId)
			{
				if (s[lineId][lineId] == 0)
				{
					var tmp : Vector.<Number> = s[lineId];
					s[lineId] = s[lineId + swapWith];
					s[lineId + swapWith] = tmp;
					++swapWith;
					--lineId;
				}
				else
				{
					swapWith = 1;
				}
			}
		}
		
		private function simplifyGaussian(s					: Vector.<Vector.<Number>>,
										  targetLine		: uint,
										  sourceLine		: uint,
										  targetComponent	: uint) : void
		{
			var ratio : Number = s[targetLine][targetComponent] / s[targetComponent][targetComponent];
			s[targetLine][0] -= s[sourceLine][0] * ratio;
			s[targetLine][1] -= s[sourceLine][1] * ratio;
			s[targetLine][2] -= s[sourceLine][2] * ratio;
			s[targetLine][3] -= s[sourceLine][3] * ratio;
		}
		
	}
}
