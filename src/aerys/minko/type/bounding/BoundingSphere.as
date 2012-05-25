package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;


	/**
	 * The BoundingSphere class represents a sphere that contains the maximum extend of an object
	 * (ie. an IMeshFilter object).
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.mesh.filter.IMeshFilter
	 */
	public final class BoundingSphere
	{
		use namespace minko_math;

		private static const TMP_VECTOR4_1	: Vector4	= new Vector4();
		private static const TMP_VECTOR4_2	: Vector4	= new Vector4();
		
		private var _center	: Vector4	= new Vector4();
		private var _radius	: Number	= 0;

		/**
		 * The position of the center of the bounding sphere.
		 */
		public function get center() : Vector4
		{
			return _center;
		}

		/**
		 * The radius of the bounding sphere.
		 */
		public function get radius() : Number
		{
			return _radius;
		}

		/**
		 * Creates a new BoundingSphere object with the specified center and radius.
		 * @param	myCenter
		 * @param	myRadius
		 */
		public function BoundingSphere(center : Vector4, radius : Number)
		{
			_center._vector.x = center.x;
			_center._vector.y = center.y;
			_center._vector.z = center.z;

			_radius = radius;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_center.changed.add(centerChangedHandler);
		}
		
		private function centerChangedHandler(center : Vector4, property : String) : void
		{
			throw new Error('This property is read-only.');
		}

		/**
		 * Create a new BoundingSphere object by computing its center and radius from
		 * the bottom-left and top-right vertices of a bounding box.
		 * @param	min
		 * @param	max
		 * @return
		 */
		public static function fromMinMax(min : Vector4, max : Vector4) : BoundingSphere
		{
			var center : Vector4 	= new Vector4((max.x + min.x) * .5,
												  (max.y + min.y) * .5,
												  (max.z + min.z) * .5);
			var radius	: Number	= Math.max(Vector4.distance(center, max),
											   Vector4.distance(center, min));

			return new BoundingSphere(center, radius);
		}

		minko_math function update(min : Vector4, max : Vector4) : void
		{
			_center._vector.x = (max.x + min.x) / 2.;
			_center._vector.y = (max.y + min.y) / 2.;
			_center._vector.z =	(max.z + min.z) / 2.;

			_radius = Math.max(Vector4.distance(_center, max),
							   Vector4.distance(_center, min));
		}

		public function clone() : BoundingSphere
		{
			return new BoundingSphere(_center, radius);
		}
		
		public function testRay(ray			: Ray,
								transform 	: Matrix4x4	= null,
								maxDistance	: Number	= Number.POSITIVE_INFINITY) : Number
		{
			var localOrigin		: Vector4	= ray.origin;
			var localDirection 	: Vector4 	= ray.direction;
			
			if (transform)
			{
				localOrigin = transform.transformVector(ray.origin, TMP_VECTOR4_1);
				
				localDirection = transform.deltaTransformVector(ray.direction, TMP_VECTOR4_2);
				localDirection.normalize();
			}
			
			if (localOrigin.length > maxDistance)
				return -1.0;
			
			var a 		: Number = localDirection.lengthSquared;
			var b 		: Number = 2 * Vector4.dotProduct(localDirection, localOrigin);
			var c 		: Number = localOrigin.lengthSquared - (_radius * _radius);
			var disc 	: Number = b * b - 4 * a * c;
			
			if (disc < 0)
				return -1.0;
			
			var distSqrt	: Number = Math.sqrt(disc);
			var q 			: Number = 0.;
			
			if (b < 0)
				q = (-b - distSqrt) * 0.5;
			else
				q = (-b + distSqrt) * 0.5;
			
			var t0 : Number = q / a;
			var t1 : Number = c / q;
			
			if (t0 > t1)
			{
				var tmp : Number = t0;
				
				t0 = t1;
				t1 = tmp;
			}
			
			if (t1 < 0)
				return -1.0;
			
			return t0 < 0 ? t1 : t0;
		}
	}
}