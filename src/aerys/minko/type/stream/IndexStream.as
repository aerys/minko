package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.type.Signal;
	
	import flash.utils.ByteArray;

	public final class IndexStream
	{
		use namespace minko_stream;

		minko_stream var _data	: Vector.<uint>			= null;

		private var _usage		: uint					= 0;
		private var _resource	: IndexBuffer3DResource	= null;
		private var _length		: uint					= 0;
		
		private var _changed	: Signal				= new Signal('IndexStream.changed');

		public function get usage() : uint
		{
			return _usage;
		}
		
		public function get resource() : IndexBuffer3DResource
		{
			return _resource;
		}
		
		public function get length() : uint
		{
			return _length;
		}

		public function set length(value : uint) : void
		{
			_data.length = value;
			invalidate();
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}

		public function IndexStream(usage	: uint,
									data 	: Vector.<uint> = null,
									length	: uint			= 0)
		{
			super();

			initialize(data, length, usage);
		}

		minko_stream function invalidate() : void
		{
			_length = _data.length;
			_changed.execute(this);
		}

		private function initialize(indices : Vector.<uint>,
									length 	: uint,
									usage	: uint) : void
		{
			_usage = usage;
			_resource = new IndexBuffer3DResource(this);

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

		/*override flash_proxy function getProperty(name : *) : *
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
		}*/

		public function get(index : int) : uint
		{
			checkReadUsage(this);
			
			return _data[index];
		}

		public function set(index : int, value : uint) : void
		{
			checkWriteUsage(this);
			
			_data[index] = value;
		}

		public function deleteTriangleByIndex(index : int) : Vector.<uint>
		{
			checkWriteUsage(this);
			
			var deletedIndices : Vector.<uint> = _data.splice(index, 3);

			invalidate();

			return deletedIndices;
		}

		public function getIndices(indices : Vector.<uint> = null) : Vector.<uint>
		{
			checkReadUsage(this);
			
			var numIndices : int = length;

			indices ||= new Vector.<uint>();
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = _data[i];

			return indices;
		}

		public function setIndices(indices : Vector.<uint>) : void
		{
			checkWriteUsage(this);
			
			var length : int = indices.length;

			for (var i : int = 0; i < length; ++i)
				_data[i] = indices[i];

			_data.length = length;

			invalidate();
		}

		public function clone() : IndexStream
		{
			return new IndexStream(_usage, _data, length);
		}

		public function toString() : String
		{
			return _data.toString();
		}
		
		public function concat(indexStream : IndexStream) : void
		{
			checkReadUsage(indexStream);
			checkWriteUsage(this);
			
			var numIndices 	: int 			= _data.length;
			var toConcat 	: Vector.<uint> = indexStream._data;
			var numIndices2 : int 			= toConcat.length;

			for (var i : int = 0; i < numIndices2; ++i, ++numIndices)
				_data[numIndices] = toConcat[i];

			invalidate();
		}

		public function push(indices 	: Vector.<uint>,
							 firstIndex	: uint	= 0,
							 count		: uint	= 0,
							 offset		: uint 	= 0) : void
		{
			checkWriteUsage(this);
			
			var numIndices : int = _data.length;

			count ||= indices.length;

			for (var i : int = 0; i < count; ++i)
				_data[int(numIndices++)] = indices[int(firstIndex + i)] + offset;

			invalidate();
		}

		public function disposeLocalData() : void
		{
			if (length != resource.numIndices)
			{
				throw new Error(
					"Unable to dispose local data: "
					+ "some indices have not been uploaded."
				);
			}

			_data = null;
			_usage = StreamUsage.STATIC;
		}
		
		public function dispose() : void
		{
			_resource.dispose();
		}
		
		/*
		public static function concat(usage 	: uint,
									  streams 	: Vector.<IndexStream>) : IndexStream
		{
			var numStreams		: int			= streams.length;
			var data			: Vector.<uint>	= new Vector.<uint>();
			var totalIndices	: int			= 0;
			var offset			: int			= 0;
			var stream			: IndexStream	= null;
			
			for (var i : int = 0; i < numStreams; ++i)
			{
				stream = streams[i];
				
				checkReadUsage(stream);
				
				var streamData 	: Vector.<uint>	= streams[i]._data;
				var numIndices	: int			= streamData.length;

				for (var j : int = 0; j < numIndices; ++j, ++totalIndices)
					data[totalIndices] = offset + streamData[j];

				offset = totalIndices;
			}

			stream = new IndexStream(usage);

			stream._data = data;
			stream._usage = usage;
			stream.invalidate();

			return stream;
		}*/
		
		private static function checkReadUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.READ))
				throw new Error();
		}
		
		private static function checkWriteUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.WRITE))
				throw new Error();
		}
		
		public static function dummyData(size : uint, offset : uint = 0) : Vector.<uint>
		{
			var indices : Vector.<uint> = new Vector.<uint>(size);
			
			for (var i : int = 0; i < size; ++i)
				indices[i] = i + offset;
			
			return indices;
		}
		
		public static function fromByteArray(usage		: uint,
											 data 		: ByteArray,
											 numIndices	: int) : IndexStream
		{
			var indices : Vector.<uint> = new Vector.<uint>(numIndices, true);
			var stream : IndexStream = new IndexStream(usage);
			
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = data.readInt();
			
			stream._data = indices;
			stream.invalidate();
			
			return stream;
		}
	}
}