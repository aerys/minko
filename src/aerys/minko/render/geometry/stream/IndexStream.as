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
									length	: uint		= 0)
		{
			super();

			initialize(data, length, usage);
		}

		minko_stream function invalidate() : void
		{
			_data.position = 0;
			_length = _data.length >>> 1;
            
            if (_length % 3 != 0)
                throw new Error('Invalid size');
			
			if (!_locked)
				_changed.execute(this);
		}

		private function initialize(data	: ByteArray,
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
					throw new Error('Invalid size');
				
				data.readBytes(_data, 0, length);
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
			value = _data.readUnsignedShort();
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

		public function deleteTriangle(triangleIndex : uint) : void
		{
			checkWriteUsage(this);
						
			_data.position = _data.length - 6;
			var v0 : uint = _data.readShort();
			var v1 : uint = _data.readShort();
			var v2 : uint = _data.readShort();
			
			_data.position = triangleIndex * 12;
			_data.writeShort(v0);
			_data.writeShort(v1);
			_data.writeShort(v2);
			
//			_data.writeBytes(_data, triangleIndex * 12, 12);
			_data.length -= 6;
			_data.position = 0;
			
			invalidate();
		}

		public function clone(usage : uint = 0) : IndexStream
		{
			return new IndexStream(usage || _usage, _data);
		}

		public function toString() : String
		{
			return _data.toString();
		}
		
		public function concat(indexStream : IndexStream,
							   firstIndex	: uint	= 0,
							   count		: uint	= 0,
							   indexOffset	: uint 	= 0) : IndexStream
		{
			checkReadUsage(indexStream);
			checkWriteUsage(this);

			pushBytes(indexStream._data, firstIndex, count, indexOffset);
			indexStream._data.position = 0;
			
			return this;
		}

		public function pushBytes(bytes			: ByteArray,
								  firstIndex	: uint	= 0,
								  count			: uint	= 0,
								  indexOffset	: uint	= 0) : IndexStream
		{
			count ||= bytes.length >>> 1;
			
			_data.position = _data.length;
			
			if (indexOffset == 0)
				_data.writeBytes(bytes, firstIndex << 1, count << 1);
			else
			{
				bytes.position = firstIndex << 1;
				for (var i : uint = 0; i < count; ++i)
					_data.writeShort(bytes.readUnsignedShort() + indexOffset);
			}

			_data.position = 0;
			bytes.position = (firstIndex + count) << 1;
			
			invalidate();
			
			return this;
		}
		
		public function pushVector(indices 		: Vector.<uint>,
								   firstIndex	: uint	= 0,
								   count		: uint	= 0,
								   offset		: uint 	= 0) : IndexStream
		{
			checkWriteUsage(this);
			
			var numIndices : int = _data.length;

			count ||= indices.length;

			_data.position = _data.length;
			for (var i : int = 0; i < count; ++i)
				_data.writeShort(indices[int(firstIndex + i)] + offset);
			_data.position = 0;

			invalidate();
			
			return this;
		}
		
		public function pushTriangle(index1 : uint, index2 : uint, index3 : uint) : IndexStream
		{
			return setTriangle(length, index1, index2, index3);
		}
		
		public function setTriangle(triangleIndex	: uint,
									index1 			: uint,
									index2 			: uint,
									index3 			: uint) : IndexStream
		{
			_data.position = triangleIndex << 1;
			_data.writeShort(index1);
			_data.writeShort(index2);
			_data.writeShort(index3);
			_data.position = 0;
			
			invalidate();
			
			return this;
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
			
			_locked = false;
			
			if (hasChanged)
				invalidate();
		}
		
		/**
		 * Clones the IndexStream by creating a new underlying ByteArray and applying an offset on the indices.
		 *
		 * @param indexStream The IndexStream to clone.
		 * @param offset The offset to apply on the indices.
		 * 
		 * @return The offseted new IndexStream.
		 * 
		 */
		public static function cloneWithOffset(indexStream	: IndexStream,
											   offset 		: uint 			= 0) : IndexStream
		{
			if (!(indexStream.usage & StreamUsage.READ))
			{
				throw new Error('Unable to read from index stream: stream usage is not set to StreamUsage.READ.');
			}
			
			var newData		: ByteArray	= new ByteArray();
			newData.endian 				= Endian.LITTLE_ENDIAN;
			
			var oldData		: ByteArray	= indexStream._data;
			oldData.position = 0;
			while (oldData.bytesAvailable)
			{
				var index	: uint		= oldData.readUnsignedShort() + offset;
				newData.writeShort(index);
			}
			newData.position = 0;
			
			return new IndexStream(indexStream.usage, newData);
		}
		
		private static function checkReadUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.READ))
				throw new Error(
					'Unable to read from index stream: stream usage is not set to StreamUsage.READ.'
				);
		}
		
		private static function checkWriteUsage(stream : IndexStream) : void
		{
			if (!(stream._usage & StreamUsage.WRITE))
				throw new Error(
					'Unable to write in index stream: stream usage is not set to StreamUsage.WRITE.'
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