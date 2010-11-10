package aerys.minko.type.math
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;

	/**
	 * The Plane class represents a geometrical plane through its equation
	 * ax + by + cz + d = 0 _dhere (a, b, c) is the normal of the plane and
	 * d the distance along that normal.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public class Plane3D
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
		public function Plane3D(myA : Number 	= 0.,
							  	myB : Number 	= 0.,
							  	myC	: Number 	= 0.,
							  	myD	: Number 	= 0.)
		{
			_a = myA;
			_b = myB;
			_c = myC;
			_d = myD;
			
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
		
		/**
		 * Test _dhether a 3D point is coinciding, infront or behind the plane.
		 * @param	myPoint
		 * @param	myThickness
		 * @return
		 */
		public final function testPoint(myX 		: Number,
										myY 		: Number,
										myZ 		: Number,
										myThickness : Number = DEFAULT_THICKNESS) : uint
		{
			var dist	: Number	= _a * myX + _b * myY + _c * myZ - _d;
			
			return (dist >= -myThickness && dist <= myThickness) ? POINT_COINCIDING
														   		 : (dist < 0. ? POINT_BEHIND
											 								  : POINT_INFRONT);
		}
		
		/**
		 * Test _dhether a polygon (triangle) is coinciding, infront or behind the plane.
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
		
		public function clone() : Plane3D
		{
			return new Plane3D(_a, _b, _c, _d);
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
											myX3 : Number, myY3 : Number, myZ3 : Number) : Plane3D
		{
			var nx : Number = (myY1 - myY3) * (myZ1 - myZ2) - (myZ1 - myZ3) * (myY1 - myY2);
			var ny : Number = (myZ1 - myZ3) * (myX1 - myX2) - (myX1 - myX3) * (myZ1 - myZ2);
			var nz : Number = (myX1 - myX3) * (myY1 - myY2) - (myY1 - myY3) * (myX1 - myX2);
			
			return new Plane3D(nx, ny, nz, myX1 * nx + myY1 * ny + myZ1 * nz);
		}
	}
}