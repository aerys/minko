package aerys.minko.type.vertex
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.geom.Vector3D;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public class VertexIterator extends Proxy
	{
		use namespace minko;
		
		private var _index			: int				= 0;
		private var _offset			: int				= 0;
		
		private var _shallow		: Boolean			= true;
		
		private var _vertex			: VertexReference	= null;
		private var _vstream		: IVertexStream		= null;
		private var _istream		: IndexStream		= null;
		
		private var _propToStream	: Object			= new Object();
		
		public function get length() : int
		{
			return _istream ? _istream.length : _vstream.length;
		}
		
		public function VertexIterator(vertexStream	: IVertexStream,
									   indexStream	: IndexStream 	= null,
									   shallow		: Boolean		= true)
		{
			super();
			
			_vstream = vertexStream;
			_istream = indexStream;
			_shallow = shallow;
			
			initialize();
		}
		
		private function initialize() : void
		{
			var components : Object = _vstream.format.components;
			
			for each (var component : VertexComponent in components)
			{
				for each (var field : String in component.fields)
					_propToStream[field] = _vstream.getVertexStreamByComponent(component);
			}
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = int(name);
			
			if (_istream)
				index = _istream._indices[index];
			
			return new VertexReference(_vstream, index, _propToStream);
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
			
			return _istream ? index < _istream.length : index < _vstream.length;
		}
		
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			index -= _offset;
			_offset = 0;
			
			return _istream ? index < _istream.length ? index + 1 : 0
				: index < _vstream.length ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			_index = index - 1;
			
			if (!_shallow || !_vertex)
				_vertex = new VertexReference(_vstream, _istream ? _istream._indices[_index]
					: _index);
			
			if (_shallow)
				_vertex._index = -_offset + (_istream ? _istream._indices[_index]
					: _index);
			
			return _vertex;
		}
	}
}