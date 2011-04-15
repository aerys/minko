package aerys.minko.type.vertex
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	
	import flash.geom.Vector3D;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public class VertexIterator extends Proxy
	{
		use namespace minko;
		
		private var _index		: int				= 0;
		private var _offset		: int				= 0;
		
		private var _shallow	: Boolean			= true;
		
		private var _vertex		: VertexReference	= null;
		private var _vb			: VertexStream	= null;
		private var _ib			: IndexStream		= null;
		
		public function get length() : int
		{
			return _ib ? _ib.length : _vb.length;
		}
		
		public function VertexIterator(vertexStream	: VertexStream,
										 indexStream	: IndexStream = null,
										 shallow		: Boolean		= true)
		{
			super();
			
			_vb = vertexStream;
			_ib = indexStream;
			_shallow = shallow;
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = int(name);
			
			return new VertexReference(_vb, _ib ? _ib._indices[index] : index);
		}
		
		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index : int = int(name);
			
			if (_vb.deleteVertexByIndex(index))
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
			
			return _ib ? index < _ib.length : index < _vb.length;
		}
		
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			index -= _offset;
			_offset = 0;
			
			return _ib ? index < _ib.length ? index + 1 : 0
				: index < _vb.length ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			_index = index - 1;
			
			if (!_shallow || !_vertex)
				_vertex = new VertexReference(_vb, _ib ? _ib._indices[_index]
					: _index);
			
			if (_shallow)
				_vertex._index = -_offset + (_ib ? _ib._indices[_index]
					: _index);
			
			return _vertex;
		}
	}
}