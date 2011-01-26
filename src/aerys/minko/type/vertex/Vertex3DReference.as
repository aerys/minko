package aerys.minko.type.vertex
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	/**
	 * ...
	 * @author Jean-Marc Le Roux
	 */
	public dynamic final class Vertex3DReference extends Proxy
	{
		use namespace minko;
		use namespace flash_proxy;
		
		minko var _index		: int				= 0;
		
		private var _stream 	: VertexStream3D 	= null;
		private var _format		: IVertex3DFormat	= null;
		
		public function get index() : int	{ return _index; }
		
		override flash_proxy function getProperty(name : *) : *
		{
			return _stream._data[int(_index * _format.dwordsPerVertex + _format.offsets[name])];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			_stream._update = true;
			_stream._version++;
			_stream._data[int(_index * _format.dwordsPerVertex + _format.offsets[name])] = value as Number;
		}
		
		public function Vertex3DReference(myStream 	: VertexStream3D,
								 		  myIndex	: int)
		{
			_stream = myStream;
			_format = _stream.format;
			_index = myIndex;
		}		
	}

}