package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Vector3D;

	/**
	 * The BoundingBox class represents a six-sided (8 vertices) box that bounds the
	 * maximum extent of an object (ie an IMeshFilter object).
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.mesh.filter.IMeshFilter
	 */
	public final class BoundingBox
	{
		use namespace minko_math;

		private static const TMP_VECTOR4_1	: Vector4	= new Vector4();
		private static const TMP_VECTOR4_2	: Vector4	= new Vector4();
		
		minko_math var _min			: Vector4			= new Vector4();
		minko_math var _max			: Vector4			= new Vector4();

		minko_math var _vertices	: Vector.<Number>	= new Vector.<Number>(24, true);

		/**
		 * The bottom-left corner of the bounding box.
		 */
		public function get min() : Vector4
		{
			return _min;
		}

		/**
		 * The top-right corner of the bounding box.
		 */
		public function get max() : Vector4
		{
			return _max;
		}

		/**
		 * Creates a new BoundingBox object.
		 * @param	min The position of the bottom-left vertex.
		 * @param	max The position of the top-right vertex.
		 */
		public function BoundingBox(min : Vector4, max : Vector4)
		{
			update(min, max);
		}

		public function testPoint(point : Vector4) : Boolean
		{
			return point.x >= _min.x && point.x <= _max.x
				   && point.y >= _min.y && point.y <= _max.y
				   && point.z >= _min.z && point.z <= _max.z;
		}
		
		/**
		 * http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
		 * http://gitorious.org/dimension/dimension/blobs/master/libdimension/prtree.c#line490
		 *  
		 * @param ray
		 * @param transform
		 * @return The distance to the collision point on the box if the ray hits it, -1 otherwise.
		 * 
		 */
		public function testRay(ray 		: Ray,
								transform	: Matrix4x4	= null,
								maxDistance	: Number	= Number.POSITIVE_INFINITY) : Number
		{
			var localOrigin 	: Vector4	= ray.origin;
			var localDirection	: Vector4	= ray.direction;
			
			if (transform)
			{
				localOrigin = transform.transformVector(ray.origin, TMP_VECTOR4_1);
				
				localDirection = transform.deltaTransformVector(ray.direction, TMP_VECTOR4_2);
				localDirection.normalize();
			}

			var ox		: Number	= localOrigin.x;
			var oy		: Number	= localOrigin.y;
			var oz		: Number	= localOrigin.z;
			
			var dx		: Number	= 1.0 / localDirection.x;
			var dy		: Number	= 1.0 / localDirection.y;
			var dz		: Number	= 1.0 / localDirection.z;
			
			var minX	: Number	= _min.x;
			var minY	: Number	= _min.y;
			var minZ	: Number	= _min.z;
			var maxX	: Number	= _max.x;
			var maxY	: Number	= _max.y;
			var maxZ	: Number	= _max.z;
			
			var tx1		: Number	= (minX - ox) * dx;
			var tx2		: Number	= (maxX - ox) * dx;
			
			var min		: Number	= tx1 < tx2 ? tx1 : tx2;
			var max		: Number	= tx1 > tx2 ? tx1 : tx2;
			var tmin	: Number	= min;
			var tmax	: Number	= max;
			
			var ty1		: Number	= (minY - oy) * dy;
			var ty2		: Number	= (maxY - oy) * dy;
			
			min = ty1 < ty2 ? ty1 : ty2;
			max = ty1 > ty2 ? ty1 : ty2;
			tmin = tmin > min ? tmin : min;
			tmax = tmax < max ? tmax : max;
			
			var tz1		: Number	= (minZ - oz) * dz;
			var tz2		: Number	= (maxZ - oz) * dz;
			
			min = tz1 < tz2 ? tz1 : tz2;
			max = tz1 > tz2 ? tz1 : tz2;
			tmin = tmin > min ? tmin : min;
			tmax = tmax < max ? tmax : max;
			
			return tmax >= Math.max(0, tmin) && tmin < maxDistance
				? tmin
				: -1.0;
		}

		public function clone() : BoundingBox
		{
			return new BoundingBox(_min, _max);
		}

		public function merge(box : BoundingBox) : void
		{
			var min : Vector3D	= _min._vector;
			var max : Vector3D	= _max._vector;

			min.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = Math.min(min.x, box._min.x);
			min.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = Math.min(min.y, box._min.y);
			min.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = Math.min(min.z, box._min.z);

			max.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = Math.max(max.x, box._max.x);
			max.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = Math.max(max.y, box._max.y);
			max.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = Math.max(max.z, box._max.z);
		}

		public static function merge(box1 : BoundingBox, box2 : BoundingBox) : BoundingBox
		{
			return new BoundingBox(
				new Vector4(Math.min(box1._min.x, box2._min.x),
					Math.min(box1._min.y, box2._min.y),
					Math.min(box1._min.z, box2._min.z)),
				new Vector4(Math.max(box1._max.x, box2._max.x),
					Math.max(box1._max.y, box2._max.y),
					Math.max(box1._max.z, box2._max.z))
			);
		}

		public function update(min : Vector4 	= null,
							   max : Vector4	= null) : void
		{
			var vmin : Vector3D	= _min._vector;
			var vmax : Vector3D	= _max._vector;

			vmin.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = min.x;
			vmin.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = min.y;
			vmin.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = min.z;

			vmax.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = max.x;
			vmax.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = max.y;
			vmax.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = max.z;
		}
	}
}