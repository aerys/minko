package aerys.minko.render.geometry.stream.iterator
{
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

	/**
	 * VertexReference object are high-level OOP-oriented vertices data proxy.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic final class VertexReference extends Proxy
	{
		use namespace minko_stream;
		use namespace flash_proxy;

		minko_stream var _propertyToStream	: Object	= null;

		private var _index	: uint			= 0;
		private var _stream	: IVertexStream	= null;

		/**
		 * The index of the vertex in the source VertexStream.
		 *
		 * @return
		 *
		 */
		public function get index() : uint
		{
			return _index;
		}
		public function set index(value : uint) : void
		{
			_index = value;
		}

		public function VertexReference(stream 	: IVertexStream,
								 		index	: int	= -1)
		{
			_stream = stream;
			_index = index == -1 ? stream.numVertices : index;
		}

		override flash_proxy function getProperty(name : *) : *
		{
			if (!_propertyToStream)
				initialize();

			var propertyName 	: String 		= name;
			var stream 			: VertexStream	= _propertyToStream[propertyName];
			
			return stream.get(stream.format.getOffsetForProperty(propertyName, _index));
		}

		override flash_proxy function setProperty(name : *, value : *) : void
		{
			if (!_propertyToStream)
				initialize();

			var propertyName 	: String 		= name;
			var stream 			: VertexStream 	= _propertyToStream[propertyName];
			
			stream.set(stream.format.getOffsetForProperty(name, _index), value as Number);
		}

		private function initialize() : void
		{
			var format			: VertexFormat	= _stream.format;
			var numComponents	: uint			= format.numComponents;
			
			_propertyToStream = new Object();
			
			for (var componentIndex : uint = 0; componentIndex < numComponents; ++componentIndex)
			{
				var component : VertexComponent = format.getComponent(componentIndex);

				var numProperties : uint = component.numProperties;
				for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
				{
					_propertyToStream[component.getProperty(propertyId)] = _stream.getStreamByComponent(
						component
					);
				}
			}
		}

		public function toString() : String
		{
			var str : String	= 'Vertex(index=' + _index;

			for (var field : String in _propertyToStream)
				str += ', ' + field + '=' + getProperty(field);

			str += ')';

			return str;
		}
	}

}