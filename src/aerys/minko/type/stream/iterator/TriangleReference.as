package aerys.minko.type.stream.iterator
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.Factory;
	import aerys.minko.type.math.Plane;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;

	public final class TriangleReference
	{
		use namespace minko_stream;
		use namespace minko_math;

		private static const VECTOR4				: Factory	= Factory.getFactory(Vector4);

		minko_stream static const UPDATE_NONE		: uint		= 0;
		minko_stream static const UPDATE_NORMAL		: uint		= 1;
		minko_stream static const UPDATE_PLANE		: uint		= 2;
		minko_stream static const UPDATE_CENTER		: uint		= 4;
		minko_stream static const UPDATE_ALL		: uint		= 1 | 2 | 4;

		minko_stream var _index		: int			= 0;
		minko_stream var _update	: uint			= UPDATE_ALL;

		private var _istream	: IndexStream		= null;

		private var _v0			: VertexReference	= null;
		private var _v1			: VertexReference	= null;
		private var _v2			: VertexReference	= null;

		private var _i0			: int				= 0;
		private var _i1			: int				= 0;
		private var _i2			: int				= 0;

		private var _v0v		: uint				= 0;
		private var _v1v		: uint				= 0;
		private var _v2v		: uint				= 0;

		private var _normal		: Vector4			= null;
		private var _plane		: Plane				= null;
		private var _center		: Vector4			= null;

		public function get index()	: int				{ return _index; }

		public function get v0()	: VertexReference	{ return _v0; }
		public function get v1() 	: VertexReference	{ return _v1; }
		public function get v2() 	: VertexReference	{ return _v2; }

		public function get i0() 	: int				{ return _istream._data[int(_index * 3)]; }
		public function get i1() 	: int				{ return _istream._data[int(_index * 3 + 1)]; }
		public function get i2() 	: int				{ return _istream._data[int(_index * 3 + 2)]; }

		private function get updateNormal() : Boolean	{ return (_update & UPDATE_NORMAL) || _v0.version != _v0v
																 || _v1.version != _v1v || _v2.version != _v2v; }

		private function get updatePlane() : Boolean	{ return (_update & UPDATE_PLANE) || _v0.version != _v0v
																 || _v1.version != _v1v || _v2.version != _v2v; }

		private function get updateCenter() : Boolean	{ return (_update & UPDATE_CENTER) || _v0.version != _v0v
																 || _v1.version != _v1v || _v2.version != _v2v; }

		private function get verticesHaveChanged() : Boolean { return _v0.version != _v0v || _v1.version != _v1v
																	  || _v2.version != _v2v; }

		public function get plane() : Plane
		{
			if (updatePlane)
				invalidatePlane();

			return _plane;
		}

		public function get normalX() : Number
		{
			if (updateNormal)
				invalidateNormal();

			return _normal.x;
		}

		public function get normalY() : Number
		{
			if (updateNormal)
				invalidateNormal();

			return _normal.y;
		}

		public function get normalZ() : Number
		{
			if (updateNormal)
				invalidateNormal();

			return _normal.z;
		}

		public function get centerX() : Number
		{
			if (updateCenter)
				invalidateCenter();

			return _center.x;
		}

		public function get centerY() : Number
		{
			if (updateCenter)
				invalidateCenter();

			return _center.y;
		}

		public function get centerZ() : Number
		{
			if (updateCenter)
				invalidateCenter();

			return _center.z;
		}

		public function set i0(value : int) : void
		{
			_istream._data[int(_index * 3)] = value;
			_istream.invalidate();
			_v0._index = value;
			_update |= UPDATE_ALL;
		}

		public function set i1(value : int) : void
		{
			_istream._data[int(_index * 3 + 1)] = value;
			_istream.invalidate();
			_v1._index = value;
			_update |= UPDATE_ALL;
		}

		public function set i2(value : int) : void
		{
			_istream._data[int(_index * 3 + 2)] = value;
			_istream.invalidate();
			_v2._index = value;
			_update |= UPDATE_ALL;
		}

		public function TriangleReference(vertexStream 	: IVertexStream,
										  indexStream	: IndexStream,
										  index 		: int)
		{
			_istream = indexStream;

			_index = index;

			_v0 = new VertexReference(vertexStream,
									  indexStream._data[int(_index * 3)]);
			_v1 = new VertexReference(vertexStream,
									  indexStream._data[int(_index * 3 + 1)]);
			_v2 = new VertexReference(vertexStream,
									  indexStream._data[int(_index * 3 + 2)]);
		}

		private function invalidateNormal() : void
		{
			if (!_normal)
				_normal = new Vector4();

			var x0 : Number = _v0.x;
			var y0 : Number = _v0.y;
			var z0 : Number = _v0.z;
			var x1 : Number = _v1.x;
			var y1 : Number = _v1.y;
			var z1 : Number = _v1.z;
			var x2 : Number = _v2.x;
			var y2 : Number = _v2.y;
			var z2 : Number = _v2.z;

			_normal.x = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
			_normal.y = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
			_normal.z = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);

			_normal.normalize();

			_update ^= UPDATE_NORMAL;
			_v0v = _v0.version;
			_v1v = _v1.version;
			_v2v = _v2.version;
		}

		public function getNormal(out : Vector4) : Vector4
		{
			return Vector4.copy(_normal, out);
		}

		public function getCenter(out : Vector4) : Vector4
		{
			return Vector4.copy(_center, out);
		}

		private function invalidatePlane() : void
		{
			if (updateNormal)
			{
				invalidateNormal();
			}
			else
			{
				_v0v = _v0.version;
				_v1v = _v1.version;
				_v2v = _v2.version;
			}

			if (!_plane)
				_plane = new Plane();

			var nx : Number = _normal.x;
			var ny : Number = _normal.y;
			var nz : Number = _normal.z;

			_plane._a = nx;
			_plane._b = ny;
			_plane._c = nz;
			_plane._d = _v0.x * nx + _v0.y * ny + _v0.z * nz;

			_plane.normalize();

			_update ^= UPDATE_PLANE;
		}

		private function invalidateCenter() : void
		{
			if (verticesHaveChanged)
			{
				_update = UPDATE_ALL ^ UPDATE_CENTER;
				_v0v = _v0.version;
				_v1v = _v1.version;
				_v2v = _v2.version;
			}

			_center.x = (_v0.x + _v1.x + _v2.x) * .33333333;
			_center.y = (_v0.y + _v1.y + _v2.y) * .33333333;
			_center.z = (_v0.z + _v1.z + _v2.z) * .33333333;
		}

		public function invertWinding() : void
		{
			_update = UPDATE_ALL;

			var tmp : int = _i0;

			_v0._index = _i0 = _i1;
			_v1._index = tmp;

			i0 = _i1;
			i1 = tmp;
		}
	}
}