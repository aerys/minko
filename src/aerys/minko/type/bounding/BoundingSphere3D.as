package aerys.minko.type.bounding
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Vector3D;
	
	/**
	 * The BoundingSphere class represents a sphere that contains the maximum extend of an object
	 * (ie. an IMeshFilter object).
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.mesh.filter.IMeshFilter
	 */
	public final class BoundingSphere3D
	{
		use namespace minko;
		
		//{ region vars
		minko var _center	: Vector4	= null;
		
		private var _radius	: Number	= 0;
		//} endregion
		
		//{ region getters/setters
		/**
		 * The position of the center of the bounding sphere.
		 */
		public function get centerX()	: Number	{ return _center.x; }
		public function get centerY()	: Number	{ return _center.y; }
		public function get centerZ()	: Number	{ return _center.z; }
		
		/**
		 * The radius of the bounding sphere.
		 */
		public function get radius()	: Number	{return _radius;}
		//} endregion
		
		/**
		 * Creates a new BoundingSphere object with the specified center and radius.
		 * @param	myCenter
		 * @param	myRadius
		 */
		public function BoundingSphere3D(center : Vector4, radius : Number)
		{
			_center = center.clone();
			_radius = radius;
		}
		/* ! CONSTRUCTOR */
		
		//{ region methods
		/**
		 * Create a new BoundingSphere object by computing its center and radius from
		 * the bottom-left and top-right vertices of a bounding box.
		 * @param	myMin
		 * @param	myMax
		 * @return
		 */
		public static function fromMinMax(min : Vector4, max : Vector4) : BoundingSphere3D
		{
			var center : Vector4 	= new Vector4((max.x + min.x) / 2.0,
												  (max.y + min.y) / 2.0,
												  (max.z + min.z) / 2.0);
			var radius	: Number	= Math.max(Vector4.distance(center, max),
											   Vector4.distance(center, min));
			
			return new BoundingSphere3D(center, radius);
		}
		//} endregion
		
		//{ region internal
		minko function update(min : Vector4, max : Vector4) : void
		{
			_center.x = (max.x + min.x) / 2.;
			_center.y = (max.y + min.y) / 2.;
			_center.z =	(max.z + min.z) / 2.;
			
			_radius = Math.max(Vector4.distance(_center, max),
							   Vector4.distance(_center, min));
		}
		
		public function getCenter() : Vector4
		{
			return _center.clone();
		}
		
		public function clone() : BoundingSphere3D
		{
			return new BoundingSphere3D(_center, radius);
		}
		//} endregion
	}
}