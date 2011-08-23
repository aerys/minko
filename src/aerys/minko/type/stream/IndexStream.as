package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.IndexBufferResource;
	import aerys.minko.type.IVersionnable;
	
	import flash.utils.ByteArray;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public final dynamic class IndexStream extends Proxy implements IVersionnable
	{
		use namespace minko_stream;
		
		minko_stream var _data	: Vector.<uint>			= null;
		
		private var _version	: uint					= 0;
		private var _dynamic	: Boolean				= false;
		private var _resource	: IndexBufferResource	= null;
		private var _length		: uint					= 0;
		
		public function get version() 	: uint					{ return _version; }
		public function get dynamic()	: Boolean				{ return _dynamic; }
		public function get resource()	: IndexBufferResource	{ return _resource; }
		public function get length()	: uint					{ return _length; }
		
		public function set length(value : uint) : void
		{
			_data.length = value;
			invalidate();
		}
		
		minko_stream static function dummyData(size : uint, offset : uint = 0) : Vector.<uint>
		{
			var indices : Vector.<uint> = new Vector.<uint>(size);
			
			for (var i : int = 0; i < size; ++i)
				indices[i] = i + offset;
			
			return indices;
		}
		
		public static function fromByteArray(data 		: ByteArray,
											 numIndices	: int) : IndexStream
		{
			var indices : Vector.<uint> = new Vector.<uint>(numIndices, true);
			var stream : IndexStream = new IndexStream();
			
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = data.readInt();
			
			stream._data = indices;
			stream.invalidate();
			
			return stream;
		}
		
		public function IndexStream(data 	: Vector.<uint> = null,
									length	: uint			= 0,
									dynamic	: Boolean		= false)
		{
			super();
			
			initialize(data, length, dynamic);
		}
		
		minko_stream function invalidate() : void
		{
			_length = _data.length;
		}
		
		private function initialize(indices : Vector.<uint>,
									length 	: uint,
									dynamic	: Boolean) : void
		{
			_dynamic = dynamic;
			_resource = new IndexBufferResource(this);
			
			if (indices)
			{
				var numIndices : int = indices && length == 0
									   ? indices.length
									   : Math.min(indices.length, length);
				
				_data = new Vector.<uint>(numIndices);
				for (var i : int = 0; i < numIndices; ++i)
					_data[i] = indices[i];
			}
			else
			{
				_data = dummyData(length);
			}
			
			invalidate();
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			return _data[int(name)];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var index : int = int(name);
				
			_data[index] = int(value);
			
			invalidate();
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index >= _data.length ? 0 : index + 1;
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _data[int(index - 1)];
		}
		
		public function getIndices(indices : Vector.<uint> = null) : Vector.<uint>
		{
			var numIndices : int = length;
			
			indices ||= new Vector.<uint>();
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = _data[i];
			
			return indices;
		}
		
		public function setIndices(indices : Vector.<uint>) : void
		{
			var length : int = indices.length;
			
			for (var i : int = 0; i < length; ++i)
				_data[i] = indices[i];
			
			_data.length = length;
			
			invalidate();
		}
		
		public function clone() : IndexStream
		{
			return new IndexStream(_data, length, _dynamic);
		}
		
		public function toString() : String
		{
			return _data.toString();
		}
		
		public function concat(myIndexBuffer : IndexStream) : void
		{
			var numIndices 	: int 			= _data.length;
			var toConcat 	: Vector.<uint> = myIndexBuffer._data;
			var numIndices2 : int 			= toConcat.length;
			
			for (var i : int = 0; i < numIndices2; ++i, ++numIndices)
				_data[numIndices] = toConcat[i];
			
			invalidate();
		}
		
		public function push(...parameters) : IndexStream
		{
			var numIndices 	: int = parameters.length;
			var length 		: int = length;

			for (var i : int = 0; i < numIndices; ++i, ++length)
				_data[length] = int(parameters[i]);
			
			invalidate();
			
			return this;
		}
		
		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index 	: int = int(name);
			var length 	: int = length;
			
			if (index > length)
				return false;
			
			for (var i : int = index; i < length - 1; ++i)
				_data[i] = _data[int(i + 1)];
			
			_data.length = length - 1;
			
			invalidate();
			
			return true;
		}
		
		public function pushIndices(indices : Vector.<uint>,
									offset	: uint 	= 0,
									count	: uint	= 0) : void
		{
			var l : int = _data.length;
			
			count ||= indices.length;
			
			for (var i : int = 0; i < count; ++i)
				_data[int(l++)] = indices[i] + offset;

			invalidate();
		}
		
		public function disposeLocalData() : void
		{
			if (length != resource.numIndices)
				throw new Error("Unable to dispose local data: "
								+ "some intices have not been uploaded.");
			
			_data = null;
			_dynamic = false;
		}
	}
}