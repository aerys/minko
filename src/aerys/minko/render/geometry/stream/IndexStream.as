package aerys.minko.render.geometry.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.type.Signal;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public final class IndexStream
	{
		use namespace minko_stream;

		minko_stream var _data			: ByteArray;
		minko_stream var _localDispose	: Boolean;

		private var _usage		: uint;
		private var _resource	: IndexBuffer3DResource;
		private var _length		: uint;
		
		private var _locked		: Boolean;
		
		private var _changed	: Signal;

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
			_data.length = value << 1;
			invalidate();
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}

		public function IndexStream(usage	: uint,
									data 	: ByteArray	= null,
									offset	: uint		= 0,
									length	: uint		= 0)
		{
			super();

			initialize(data, offset, length, usage);
		}

		minko_stream function invalidate() : void
		{
			_data.position = 0;
			_length = _data.length >>> 1;
			
			if (!_locked)
				_changed.execute(this);
		}

		private function initialize(data	: ByteArray,
									offset	: uint,
									length 	: uint,
									usage	: uint) : void
		{
			_changed = new Signal('IndexStream.changed');
			
			_usage = usage;
			_resource = new IndexBuffer3DResource(this);

			_data = new ByteArray();
			_data.endian = Endian.LITTLE_ENDIAN;
			if (data)
			{
				if (data.endian != Endian.LITTLE_ENDIAN)
					throw new Error('Endianness must be Endian.LITTLE_ENDIAN.');
				if (length == 0)
					length = data.bytesAvailable;
				if (length % 6 != 0)
					throw new Error();
				
				_data.writeBytes(data, offset, length);
			}
			else
			{
				_data = dummyData(length);
			}
			
			_data.position = 0;
			
			invalidate();
		}

		public function get(index : uint) : uint
		{
			var value : uint = 0;

			checkReadUsage(this);
			
			_data.position = index << 1;
			value = _data.readShort();
			_data.position = 0;
			
			return value;
		}

		public function set(index : uint, value : uint) : void
		{
			checkWriteUsage(this);
			
			_data.position = index << 1;
			_data.writeShort(value);
			_data.position = 0;
			
			invalidate();
		}

		public function deleteTriangleByIndex(index : uint) : void
		{
			checkWriteUsage(this);
			
			_data.position = 0;
			_data.writeBytes(_data, index * 12, 12);
			_data.position = 0;
			
			invalidate();
		}

		public function clone(usage : uint = 0) : IndexStream
		{
			return new IndexStream(usage || _usage, _data, length);
		}

		public function toString() : String
		{
			return _data.toString();
		}
		
		public function concat(indexStream : IndexStream,
							   firstIndex	: uint	= 0,
							   count		: uint	= 0,
							   offset		: uint 	= 0) : IndexStream
		{
			checkReadUsage(indexStream);
			checkWriteUsage(this);
			
			_data.position = _data.length;
			_data.writeBytes(indexStream._data);
			_data.position = 0;

			invalidate();
			
			return this;
		}

		public function push(indices 	: Vector.<uint>,
							 firstIndex	: uint	= 0,
							 count		: uint	= 0,
							 offset		: uint 	= 0) : void
		{
			checkWriteUsage(this);
			
			var numIndices : int = _data.length;

			count ||= indices.length;

			_data.position = _data.length;
			for (var i : int = 0; i < count; ++i)
				_data.writeShort(indices[int(firstIndex + i)] + offset);
			_data.position = 0;

			invalidate();
		}

		public function disposeLocalData(waitForUpload : Boolean = true) : void
		{
			if (waitForUpload && _length != resource.numIndices)
				_localDispose = true;
			else
			{
				_data = null;
				_usage = StreamUsage.STATIC;
			}
		}
		
		public function dispose() : void
		{
			_resource.dispose();
		}
		
		public function lock() : ByteArray
		{
			checkReadUsage(this);
			
			_data.position = 0;
			_locked = true;
			
			return _data;
		}
		
		public function unlock(hasChanged : Boolean = true) : void
		{
			_data.position = 0;
			
			if (hasChanged)
				_changed.execute(this);
		}
		
		private static function checkReadUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.READ))
				throw new Error(
					'Unable to read from vertex stream: stream usage is not set to StreamUsage.READ.'
				);
		}
		
		private static function checkWriteUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.WRITE))
				throw new Error(
					'Unable to write in vertex stream: stream usage is not set to StreamUsage.WRITE.'
				);
		}
		
		public static function dummyData(size 	: uint,
										 offset : uint = 0) : ByteArray
		{
			var indices : ByteArray = new ByteArray();
			
			indices.endian = Endian.LITTLE_ENDIAN;
			for (var i : int = 0; i < size; ++i)
				indices.writeShort(i + offset);
			indices.position = 0;
			
			return indices;
		}
		
		public static function fromVector(usage : uint, data : Vector.<uint>) : IndexStream
		{
			var stream 		: IndexStream 	= new IndexStream(usage);
			var numIndices 	: uint 			= data.length;
			
			for (var i : uint = 0; i < numIndices; ++i)
				stream._data.writeShort(data[i]);
			stream._data.position = 0;
			stream._length = numIndices;
			
			return stream;
		}
	}
}