package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
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
		use namespace minko;
		
		minko var _min		: ConstVector4		= new ConstVector4();
		minko var _max		: ConstVector4		= new ConstVector4();
		
		minko var _vertices	: Vector.<Number>	= new Vector.<Number>(24, true);
		
		/**
		 * The bottom-left corner of the bounding box.
		 */
		public function get min()	: ConstVector4	{ return _min; }
		
		/**
		 * The top-right corner of the bounding box.
		 */
		public function get max()	: ConstVector4	{ return _max; }
		
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
			return new BoundingBox(new Vector4(Math.min(box1._min.x, box2._min.x),
											   Math.min(box1._min.y, box2._min.y),
											   Math.min(box1._min.z, box2._min.z)),
								   new Vector4(Math.max(box1._max.x, box2._max.x),
											   Math.max(box1._max.y, box2._max.y),
											   Math.max(box1._max.z, box2._max.z)));
		}
		
		public function getVertices() : Vector.<Number>
		{
			return _vertices.concat();
		}
		
		public function update(min : Vector4 = null,
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