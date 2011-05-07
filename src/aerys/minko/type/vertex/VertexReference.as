package aerys.minko.type.vertex
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
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
		
		minko var _index		: int		= 0;
		
		private var _stream 		: IVertexStream	= null;
		private var _propToStream	: Object	= new Object();
		
		public function get index() : int	{ return _index; }
		
		override flash_proxy function getProperty(name : *) : *
		{
			var stream : VertexStream = _propToStream[name];
			var format : VertexFormat = stream.format;
			
			return stream._data[int(_index * format.dwordsPerVertex + format.getOffsetForField(name))];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var stream : VertexStream = _propToStream[name];
			var format : VertexFormat = stream.format;
			
			stream._update = true;
			stream._version++;
			stream._data[int(_index * format.dwordsPerVertex + format.getOffsetForField(name))] = value as Number;
		}
		
		public function VertexReference(stream 				: IVertexStream,
								 		index				: int,
										propertyToStream	: Object = null)
		{
			_stream = stream;
			_index = index;
			_propToStream = propertyToStream;
		}		
	}

}