package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko;
	
	import flash.geom.Vector3D;
	/**
	 * The BoundingBox class represents a six-sided (8 vertices) box that bounds the
	 * maximum extent of an object (ie an IMeshFilter object).
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.mesh.filter.IMeshFilter
	 */
	public final class BoundingBox3D
	{
		use namespace minko;
		
		private var _min		: Vector3D			= new Vector3D();
		private var _max		: Vector3D			= new Vector3D();
		
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
		 * @param	myMin The position of the bottom-left vertex.
		 * @param	myMax The position of the top-right vertex.
		 */
		public function BoundingBox3D(myMin : Vector3D, myMax : Vector3D)
		{
			update(myMin, myMax);
		}
		
		//{ region methods
		public function testPoint(myPoint : Vector3D) : Boolean
		{
			return myPoint.x >= _min.x && myPoint.x <= _max.x
				   && myPoint.y >= _min.y && myPoint.y <= _max.y
				   && myPoint.z >= _min.z && myPoint.z <= _max.z;
		}
		
		public function clone() : BoundingBox3D
		{
			return new BoundingBox3D(_min, _max);
		}
		
		public function merge(myBox : BoundingBox3D) : void
		{
			_min.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = Math.min(_min.x, myBox._min.x);
			_min.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = Math.min(_min.y, myBox._min.y);
			_min.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = Math.min(_min.z, myBox._min.z);
			
			_max.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = Math.max(_max.x, myBox._max.x);
			_max.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = Math.max(_max.y, myBox._max.y);
			_max.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = Math.max(_max.z, myBox._max.z);
		}
		//} endregion
		
		//{ region static
		public static function merge(myBB1 : BoundingBox3D, myBB2 : BoundingBox3D) : BoundingBox3D
		{
			return new BoundingBox3D(new Vector3D(Math.min(myBB1._min.x, myBB2._min.x),
												Math.min(myBB1._min.y, myBB2._min.y),
												Math.min(myBB1._min.z, myBB2._min.z)),
								   new Vector3D(Math.max(myBB1._max.x, myBB2._max.x),
												Math.max(myBB1._max.y, myBB2._max.y),
												Math.max(myBB1._max.z, myBB2._max.z)));
		}
		
		public function getVertices() : Vector.<Number>
		{
			return _vertices.concat();
		}
		//} endregion
		
		//{ region internals
		minko function get min() : Vector3D { return _min; }
		
		minko function get max() : Vector3D { return _max; }
		
		minko function update(myMin : Vector3D	= null,
							  myMax : Vector3D	= null) : void
		{
			myMin ||= _min;
			myMax ||= _max;
			
			_min.x = _vertices[0] = _vertices[9] = _vertices[15] = _vertices[18] = myMin.x;
			_min.y = _vertices[1] = _vertices[4] = _vertices[7] = _vertices[10] = myMin.y;
			_min.z = _vertices[2] = _vertices[5] = _vertices[20] = _vertices[23] = myMin.z;
			
			_max.x = _vertices[3] = _vertices[6] = _vertices[12] = _vertices[21] = myMax.x;
			_max.y = _vertices[13] = _vertices[16] = _vertices[19] = _vertices[22] = myMax.y;
			_max.z = _vertices[8] = _vertices[11] = _vertices[14] = _vertices[17] = myMax.z;
		}
		//} endregion
	}
}