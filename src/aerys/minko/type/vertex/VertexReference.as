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
		
		minko var _index		: int			= 0;
		
		private var _stream 		: IVertexStream	= null;
		private var _prop2stream	: Object		= new Object();
		
		public function get index() : int	{ return _index; }
		
		override flash_proxy function getProperty(name : *) : *
		{
			var stream : VertexStream = getVertexStreamByProperty(name);
			var format : VertexFormat = stream.format;
			
			return stream._data[int(_index * format.dwordsPerVertex + format.getOffsetForField(name))];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var stream : VertexStream = getVertexStreamByProperty(name);
			var format : VertexFormat = stream.format;
			
			stream._update = true;
			stream._version++;
			stream._data[int(_index * format.dwordsPerVertex + format.getOffsetForField(name))] = value as Number;
		}
		
		private function getVertexStreamByProperty(property : String) : VertexStream
		{
			var stream 	: VertexStream 	= _prop2stream[property];
			
			if (!stream)
			{
				var components : Object = _stream.format.components;
				
				for each (var component : VertexComponent in components)
				{
					if (component.hasField(property))
						return _prop2stream[property] = stream = _stream.getStreamByComponent(component);
				}
				
				if (!stream)
					throw new Error("Unable to find vertex component for property '" + property + "'");
			}
			
			return stream;
		}
		
		public function VertexReference(stream 	: IVertexStream,
								 		index	: int)
		{
			_stream = stream;
			_index = index;
		}		
	}

}