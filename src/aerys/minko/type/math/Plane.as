package aerys.minko.type.math
{
	import aerys.minko.ns.minko;
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
	public class Plane
	{
		use namespace minko;

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

		minko var _a	: Number	= 0.;
		minko var _b	: Number	= 0.;
		minko var _c	: Number	= 0.;
		minko var _d	: Number	= 0.;

		public function get a() : Number	{ return _a; }
		public function get b() : Number	{ return _b; }
		public function get c() : Number	{ return _c; }
		public function get d() : Number	{ return _d; }

		/**
		 * Creates a new Plane object.
		 * @param	myA
		 * @param	myB
		 * @param	myC
		 * @param	myD
		 */
		public function Plane(a 	: Number 	= 0.,
							  	b 	: Number 	= 0.,
							  	c	: Number 	= 0.,
							  	d	: Number 	= 0.)
		{
			_a = a;
			_b = b;
			_c = c;
			_d = d;

			normalize();
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

		public function project(v : Vector4) : Vector4
		{
			var scale : Number = _a * v.x + _b * v.y + _c * v.z;

			return new Vector4(v.x - scale * _a,
							   v.y - scale * _b,
							   v.z - scale * _c);
		}

		/**
		 * Test a 3D point is coinciding, infront or behind the plane.
		 * @param	myPoint
		 * @param	myThickness
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
		public final function testRay(origin 	: Vector4,
									  target	: Vector4) : uint
		{
			var distOrigin	: Number	= _a * origin.x + _b * origin.y + _c * origin.z - _d;
			var distTarget	: Number	= _a * target.x + _b * target.y + _c * target.z - _d;

			var r : uint = 0;

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
		public final function testTriangle(myX1 : Number, myY1 : Number, myZ1 : Number,
										   myX2 : Number, myY2 : Number, myZ2 : Number,
										   myX3 : Number, myY3 : Number, myZ3 : Number,
										   myThickness 	: Number = DEFAULT_THICKNESS)	: uint
		{
			var test	: Number	= 0;
			var result  : uint 		= 0;
			var s		: Number	= -myThickness;
			var t		: Number	= myThickness;

			test = _a * myX1 + _b * myY1 + _c * myZ1 - _d;
			result = ((test >= s && test <= t) ? POINT_COINCIDING
											 : ((test < 0.) ? POINT_BEHIND
														    : POINT_INFRONT)) << 16;

			test = _a * myX2 + _b * myY2 + _c * myZ2 - _d;
			result |= ((test >= s && test <= t) ? POINT_COINCIDING
											  : ((test < 0.) ? POINT_BEHIND
														     : POINT_INFRONT)) << 8;

			test = _a * myX3 + _b * myY3 + _c * myZ3 - _d;
			result |= (test >= s && test <= t) ? POINT_COINCIDING
											 : ((test < 0.) ? POINT_BEHIND
														    : POINT_INFRONT);

			// if coplanar
			if (result == POLYGON_COINCIDING)
			{
				var x1x2 : Number = myX1 - myX2;
				var x1x3 : Number = myX1 - myX3;
				var y1y2 : Number = myY1 - myY2;
				var y1y3 : Number = myY1 - myY3;
				var z1z2 : Number = myZ1 - myZ2;
				var z1z3 : Number = myZ1 - myZ3;
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

		/**
		 * Create a new Plane from a polygon defined by 3 set of (x, _b, _c) tuples.
		 * @param	myX1
		 * @param	myY1
		 * @param	myZ1
		 * @param	myX2
		 * @param	myY2
		 * @param	myZ2
		 * @param	myX3
		 * @param	myY3
		 * @param	myZ3
		 * @return The Plane containing the specified polygon.
		 */
		static public function fromTriangle(myX1 : Number, myY1 : Number, myZ1 : Number,
											myX2 : Number, myY2 : Number, myZ2 : Number,
											myX3 : Number, myY3 : Number, myZ3 : Number) : Plane
		{
			var nx : Number = (myY1 - myY3) * (myZ1 - myZ2) - (myZ1 - myZ3) * (myY1 - myY2);
			var ny : Number = (myZ1 - myZ3) * (myX1 - myX2) - (myX1 - myX3) * (myZ1 - myZ2);
			var nz : Number = (myX1 - myX3) * (myY1 - myY2) - (myY1 - myY3) * (myX1 - myX2);

			return new Plane(nx, ny, nz, myX1 * nx + myY1 * ny + myZ1 * nz);
		}
	}
}
