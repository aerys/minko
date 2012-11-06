package aerys.minko.render.geometry.stream.iterator
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	
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
		
		private var _index				: uint;
		private var _offset				: uint;

		private var _singleReference	: Boolean;

		private var _vertex				: VertexReference;
		private var _vstream			: IVertexStream;
		private var _istream			: IndexStream;

		private var _propertyToStream	: Object;

		public function get vertexStream() : IVertexStream
		{
			return _vstream;
		}
		
		public function get length() : uint
		{
			return _istream ? _istream.length : _vstream.numVertices;
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
			_propertyToStream = {};
			
			var format			: VertexFormat	= _vstream.format;
			var numComponents	: uint			= format.numComponents;

			for (var componentIndex : uint = 0; componentIndex < numComponents; ++componentIndex)
			{
				var component : VertexComponent = format.getComponent(componentIndex);
				
				var numProperties : uint = component.numProperties;
				for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
				{
					_propertyToStream[component.getProperty(propertyId)] = _vstream.getStreamByComponent(
						component
					);
				}
			}
		}

		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = int(name);

			if (_istream)
				index = _istream.get(index);

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
					_vertex.index = index;
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
			var ref				: VertexReference	= flash_proxy::getProperty(int(name));
			var obj 			: Object 			= Object(value);
			var format			: VertexFormat		= _vstream.format;
			var numComponents	: uint				= format.numComponents;
			
			for (var componentIndex : uint = 0; componentIndex < numComponents; ++componentIndex)
			{
				var component 		: VertexComponent 	= format.getComponent(componentIndex);
				var numProperties 	: uint 				= component.numProperties;
				
				for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
				{
					var propertyName : String = component.getProperty(propertyId);
					
					ref[propertyName] = Number(obj[propertyName]);
				}
			}
		}

		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index : int = int(name);

			if (_vstream.deleteVertex(index))
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
				: index < _vstream.numVertices;
		}


		override flash_proxy function nextNameIndex(index : int) : int
		{
			index -= _offset;
			_offset = 0;

			return _istream
				? index < _istream.length ? index + 1 : 0
				: index < _vstream.numVertices ? index + 1 : 0;
		}

		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}

		override flash_proxy function nextValue(index : int) : *
		{
			_index = index - 1;
			index = _istream ? _istream.get(_index) : _index;

			if (!_singleReference || !_vertex)
			{
				_vertex = new VertexReference(_vstream, index);
				_vertex._propertyToStream = _propertyToStream;
			}

			if (_singleReference)
				_vertex.index = -_offset + index;

			return _vertex;
		}
	}
}