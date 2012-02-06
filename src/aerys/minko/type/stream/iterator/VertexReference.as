package aerys.minko.type.stream.iterator
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	/**
	 * VertexReference object are high-level OOP-oriented vertices data proxy.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic final class VertexReference extends Proxy
	{
		use namespace minko_stream;
		use namespace flash_proxy;

		minko_stream var _index				: int		= 0;
		minko_stream var _propertyToStream	: Object	= null;

		private var _stream	: IVertexStream	= null;

		/**
		 * The index of the vertex in the source VertexStream.
		 *
		 * @return
		 *
		 */
		public function get index() : int	{ return _index; }

		public function VertexReference(stream 	: IVertexStream,
								 		index	: int	= -1)
		{
			_stream = stream;
			_index = index == -1 ? stream.length : index;
		}

		override flash_proxy function getProperty(name : *) : *
		{
			if (!_propertyToStream)
				initialize();

			var propertyName 	: String 		= name;
			var stream 			: VertexStream	= _propertyToStream[propertyName];
			
			if (!(stream.usage & StreamUsage.READ))
				throw new Error("Unable to read data from vertex stream.");
			
			var format 			: VertexFormat 	= stream.format;
			var index			: int			= _index * format.dwordsPerVertex
												  + format.getOffsetForField(name);

			return stream.get(index);
		}

		override flash_proxy function setProperty(name : *, value : *) : void
		{
			if (!_propertyToStream)
				initialize();

			var propertyName 	: String 		= name;
			var stream 			: VertexStream 	= _propertyToStream[propertyName];
			
			if (!(stream.usage & StreamUsage.WRITE))
				throw new Error("Unable to write data into vertex stream.");
			
			var format 			: VertexFormat 	= stream.format;
			var index			: int			= _index * format.dwordsPerVertex
												  + format.getOffsetForField(name);

			stream.set(index, value as Number);
		}

		private function initialize() : void
		{
			var components : Object = _stream.format.components;

			_propertyToStream = new Object();

			for each (var component : VertexComponent in components)
				for each (var field : String in component.fields)
					_propertyToStream[field] = _stream.getStreamByComponent(component);
		}

		public function toString() : String
		{
			var str : String	= "Vertex(index=" + _index;

			for (var field : String in _propertyToStream)
				str += ", " + field + "=" + getProperty(field);

			str += ")";

			return str;
		}
	}

}