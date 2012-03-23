package aerys.minko.type.stream.iterator
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	/**
	 * VertexIterator allow per-vertex access on VertexStream objects.
	 *
	 * The class overrides the "[]" and "delete" operators and is iterable
	 * using "for" and "for each" loops. It enable an higher level and easier
	 * traversal and manipulation of the underlaying data stream while
	 * minimizing the memory footprint.
	 *
	 * <pre>
	 * public function printPositions(mesh : IMesh) : void
	 * {
	 * 	var vertices : VertexIterator = new VertexIterator(mesh.vertexStream);
	 *
	 * 	for each (var vertex : VertexReference in vertices)
	 * 		trac e(vertex.x, vertex.y, vertex.z);
	 * }
	 * </pre>
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public dynamic final class VertexIterator extends Proxy
	{
		use namespace minko_stream;

		private var _index				: int				= 0;
		private var _offset				: int				= 0;

		private var _singleReference	: Boolean			= true;

		private var _vertex				: VertexReference	= null;
		private var _vstream			: IVertexStream		= null;
		private var _istream			: IndexStream		= null;

		private var _propertyToStream	: Object			= new Object();

		public function get vertexStream() : IVertexStream
		{
			return _vstream;
		}
		
		public function get length() : int
		{
			return _istream ? _istream.length : _vstream.length;
		}

		/**
		 * Create a new VertexIterator instance.
		 *
		 * If the "singleReference" argument is true, only one VertexReference
		 * will be created during a "for each" loop on the iterator. This
		 * VertexReference will be updated at each iteration to provide
		 * the data of the current vertex. Otherwise, a dedicated
		 * VertexReference will be provided at each iteration. Using the
		 * "single reference" mode is nicer on the memory footprint, but makes
		 * it impossible to keep a VertexReference object and re-use it later
		 * as it will be automatically updated at each iteration.
		 *
		 * @param vertexStream The VertexStream to use.
		 *
		 * @param indexStream The IndexStream to use. If none is
		 * provided, the iterator will iterate on each vertex of the
		 * VertexStream. Otherwise, the iterator will iterate
		 * on vertices as provided by the indices in the IndexStream.
		 *
		 * @param shallow Whether to use the "single reference" mode.
		 *
		 * @return
		 *
		 */
		public function VertexIterator(vertexStream		: IVertexStream,
									   indexStream		: IndexStream 	= null,
									   singleReference	: Boolean		= true)
		{
			super();

			_vstream = vertexStream;
			_istream = indexStream;
			_singleReference = singleReference;

			initialize();
		}

		private function initialize() : void
		{
			var components : Object = _vstream.format.components;

			for each (var component : VertexComponent in components)
				for each (var field : String in component.fields)
					_propertyToStream[field] = _vstream.getStreamByComponent(component);
		}

		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = int(name);

			if (_istream)
				index = _istream._data[index];

			var vertex : VertexReference	= _vertex;

			if (_singleReference)
			{
				if (!_vertex)
				{
					_vertex = new VertexReference(_vstream, index);
					_vertex._propertyToStream = _propertyToStream;
				}
				else
				{
					_vertex._index = index;
				}

				vertex = _vertex;
			}
			else
			{
				vertex = new VertexReference(_vstream, index);
				vertex._propertyToStream = _propertyToStream;
			}

			return vertex;
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var ref			: VertexReference			= flash_proxy::getProperty(int(name));
			var obj 		: Object 					= Object(value);
			var components	: Vector.<VertexComponent>	= _vstream.format.components;
			
			for each (var component : VertexComponent in components)
			for each (var field : String in component.fields)
			ref[field] = Number(obj[field]);
		}

		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index : int = int(name);

			if (_vstream.deleteVertexByIndex(index))
			{
				if (index <= _index)
					++_offset;

				return true;
			}

			return false;
		}

		override flash_proxy function hasProperty(name : *) : Boolean
		{
			var index : int = int(name);

			return _istream
				   ? index < _istream.length
				   : index < _vstream.length;
		}


		override flash_proxy function nextNameIndex(index : int) : int
		{
			index -= _offset;
			_offset = 0;

			return _istream
				   ? index < _istream.length ? index + 1 : 0
				   : index < _vstream.length ? index + 1 : 0;
		}

		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}

		override flash_proxy function nextValue(index : int) : *
		{
			_index = index - 1;
			index = _istream ? _istream._data[_index] : _index;

			if (!_singleReference || !_vertex)
			{
				_vertex = new VertexReference(_vstream, index);
				_vertex._propertyToStream = _propertyToStream;
			}

			if (_singleReference)
				_vertex._index = -_offset + index;

			return _vertex;
		}
	}
}