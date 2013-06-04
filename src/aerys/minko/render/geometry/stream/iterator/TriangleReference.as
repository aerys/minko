package aerys.minko.render.geometry.stream.iterator
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;

	public final class TriangleReference
	{
		use namespace minko_stream;

		private static const DUMMY_INDICES			: Vector.<uint>	= new <uint>[0, 1, 2];
		
		minko_stream static const UPDATE_NONE		: uint		= 0;
		minko_stream static const UPDATE_NORMAL		: uint		= 1;
		minko_stream static const UPDATE_PLANE		: uint		= 2;
		minko_stream static const UPDATE_CENTER		: uint		= 4;
		minko_stream static const UPDATE_ALL		: uint		= 1 | 2 | 4;

		minko_stream var _index		: uint				= 0;
		minko_stream var _update	: uint				= UPDATE_ALL;

		private var _indexStream	: IndexStream		= null;

		private var _v0				: VertexReference	= null;
		private var _v1				: VertexReference	= null;
		private var _v2				: VertexReference	= null;

		private var _i0				: uint				= 0;
		private var _i1				: uint				= 0;
		private var _i2				: uint				= 0;

		public function get index()	: int
		{
			return _index;
		}

		public function get v0() : VertexReference
		{
			return _v0;
		}
		public function get v1() : VertexReference
		{
			return _v1;
		}
		public function get v2() : VertexReference
		{
			return _v2;
		}

		public function get i0() : int
		{
			return _indexStream._data[int(_index * 3)];
		}
		public function set i0(value : int) : void
		{
			_indexStream._data[int(_index * 3)] = value;
			_indexStream.invalidate();
			_v0._index = value;
			_update |= UPDATE_ALL;
		}
		
		public function get i1() : int
		{
			return _indexStream._data[int(_index * 3 + 1)];
		}
		public function set i1(value : int) : void
		{
			_indexStream._data[int(_index * 3 + 1)] = value;
			_indexStream.invalidate();
			_v1._index = value;
			_update |= UPDATE_ALL;
		}
		
		public function get i2() : int
		{
			return _indexStream._data[int(_index * 3 + 2)];
		}
		public function set i2(value : int) : void
		{
			_indexStream._data[int(_index * 3 + 2)] = value;
			_indexStream.invalidate();
			_v2._index = value;
			_update |= UPDATE_ALL;
		}
		
		public function TriangleReference(vertexStream 	: IVertexStream,
										  indexStream	: IndexStream,
										  index 		: uint)
		{
			_indexStream = indexStream;
			_index = index;

			initialize(vertexStream);
		}
		
		private function initialize(vertexStream : IVertexStream) : void
		{
			var ii : uint = _index * 3;

			// create a new triangle if it does not exist
			if (ii >= _indexStream.length)
				_indexStream.pushVector(DUMMY_INDICES);
			
			_v0 = new VertexReference(vertexStream, _indexStream.get(ii));
			_v1 = new VertexReference(vertexStream, _indexStream.get(ii + 1));
			_v2 = new VertexReference(vertexStream, _indexStream.get(ii + 2));
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