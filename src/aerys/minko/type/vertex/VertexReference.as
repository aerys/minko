package aerys.minko.type.vertex
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public dynamic final class VertexReference extends Proxy
	{
		use namespace minko;
		use namespace flash_proxy;
		
		minko var _index		: int				= 0;
		
		private var _stream 	: VertexStream 	= null;
		private var _format		: VertexFormat	= null;
		
		public function get index() : int	{ return _index; }
		
		override flash_proxy function getProperty(name : *) : *
		{
			return _stream._data[int(_index * _format.dwordsPerVertex + _format.getOffsetForField(name))];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			_stream._update = true;
			_stream._version++;
			_stream._data[int(_index * _format.dwordsPerVertex + _format.getOffsetForField(name))] = value as Number;
		}
		
		public function VertexReference(stream 	: VertexStream,
								 		index	: int)
		{
			_stream = stream;
			_format = stream.format;
			_index = index;
		}		
	}

}