package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko_math;
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
			throw new Error("This property is read-only.");
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
	}
}