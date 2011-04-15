package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.Vector4;

	/**
	 * The BoundingBox class represents a six-sided (8 vertices) box that bounds the
	 * maximum extent of an object (ie an IMeshFilter object).
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.mesh.filter.IMeshFilter
	 */
	public final class BoundingBox
	{
		use namespace minko;
		
		minko var _min		: Vector4			= new Vector4();
		minko var _max		: Vector4			= new Vector4();
		
		minko var _vertices	: Vector.<Number>	= new Vector.<Number>(24, true);
		/**
		 * The bottom-left corner of the bounding box.
		 */
		public function get minX() : Number		{ return _min.x; }
		public function get minY() : Number		{ return _min.y; }
		public function get minZ() : Number		{ return _min.z; }
		/**
		 * The top-right corner of the bounding box.
		 */
		public function get maxX() : Number		{ return _max.x; }
		public function get maxY() : Number		{ return _max.y; }
		public function get maxZ() : Number		{ return _max.z; }
		
		/**
		 * Creates a new BoundingBox object.
		 * @param	min The position of the bottom-left vertex.
		 * @param	max The position of the top-right vertex.
		 */
		public function BoundingBox(min : Vector4, max : Vector4)
		{
			update(min, max);
		}
		
		//{ region methods
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
			_min.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = Math.min(_min.x, box._min.x);
			_min.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = Math.min(_min.y, box._min.y);
			_min.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = Math.min(_min.z, box._min.z);
			
			_max.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = Math.max(_max.x, box._max.x);
			_max.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = Math.max(_max.y, box._max.y);
			_max.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = Math.max(_max.z, box._max.z);
		}
		//} endregion
		
		//{ region static
		public static function merge(myBB1 : BoundingBox, myBB2 : BoundingBox) : BoundingBox
		{
			return new BoundingBox(new Vector4(Math.min(myBB1._min.x, myBB2._min.x),
												 Math.min(myBB1._min.y, myBB2._min.y),
												 Math.min(myBB1._min.z, myBB2._min.z)),
								   	 new Vector4(Math.max(myBB1._max.x, myBB2._max.x),
												 Math.max(myBB1._max.y, myBB2._max.y),
												 Math.max(myBB1._max.z, myBB2._max.z)));
		}
		
		public function getVertices() : Vector.<Number>
		{
			return _vertices.concat();
		}
		//} endregion
		
		//{ region internals
		minko function update(min : Vector4 = null,
							  max : Vector4	= null) : void
		{
			min ||= _min;
			max ||= _max;
			
			_min.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = min.x;
			_min.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = min.y;
			_min.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = min.z;
			
			_max.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = max.x;
			_max.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = max.y;
			_max.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = max.z;
		}
		//} endregion
	}
}