package aerys.minko.type.stream
{
	import aerys.minko.type.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_stream;
	
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.ByteArray;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public final dynamic class IndexStream extends Proxy implements IVersionnable
	{
		use namespace minko_stream;
		
		minko_stream var _indices		: Vector.<uint>		= null;
		minko_stream var _version		: uint				= 0;
		
		minko_stream var _nativeBuffer 	: IndexBuffer3D		= null;
		
		private var _update			: Boolean			= true;
		private var _length			: int				= 0;
		private var _dynamic		: Boolean			= false;
		private var _bufferLength	: int				= 0;
		
		public function get version() : uint
		{
			return _version;
		}
		
		public function get length() : int	{ return _indices ? _indices.length : _length; }
		
		public function set length(value : int) : void
		{
			_indices.length = value;
			_update = true;
		}
		
		public static function dummyData(size : uint, offset : uint = 0) : Vector.<uint>
		{
			var indices : Vector.<uint> = new Vector.<uint>(size);
			
			for (var i : int = 0; i < size; ++i)
				indices[i] = i + offset;
			
			return indices;
		}
		
		public function IndexStream(data 	: Vector.<uint> = null,
									length	: uint			= 0,
									dynamic	: Boolean		= false)
		{
			super();
			
			_length = length || (data ? data.length : 0);
			_dynamic = dynamic;
			
			initialize(data);
		}
		
		private function initialize(indices : Vector.<uint>,
									length 	: uint	= 0) : void
		{
			if (indices)
			{
				var numIndices : int = Math.min(indices.length, _length);
				
				_indices = new Vector.<uint>(numIndices);
				for (var i : int = 0; i < numIndices; ++i)
					_indices[i] = indices[i];
			}
			else
			{
				_indices = dummyData(_length);
			}
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			return _indices[int(name)];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var index : int = int(name);
				
			_indices[index] = int(value);
			_update = true;
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index >= _indices.length ? 0 : index + 1;
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _indices[int(index - 1)];
		}
		
		public function getIndices() : Vector.<uint>
		{
			return _indices.concat();
		}
		
		public function setIndices(indices : Vector.<uint>) : void
		{
			var length : int = indices.length;
			
			for (var i : int = 0; i < length; ++i)
				_indices[i] = indices[i];
			
			_indices.length = length;
			_update = true;
		}
		
		public function clone() : IndexStream
		{
			return new IndexStream(_indices, _length, _dynamic);
		}
		
		public function toString() : String
		{
			return _indices.toString();
		}
		
		public function concat(myIndexBuffer : IndexStream) : void
		{
			var numIndices 	: int 			= _indices.length;
			var toConcat 	: Vector.<uint> = myIndexBuffer._indices;
			var numIndices2 : int 			= toConcat.length;
			
			for (var i : int = 0; i < numIndices2; ++i, ++numIndices)
				_indices[numIndices] = toConcat[i];
			
			_update = true;
		}
		
		public function push(...parameters) : IndexStream
		{
			var numIndices 	: int = parameters.length;
			var length 		: int = length;

			for (var i : int = 0; i < numIndices; ++i, ++length)
				_indices[length] = int(parameters[i]);
			
			_update = true;
			++_version;
			
			return this;
		}
		
		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index 	: int = int(name);
			var length 	: int = length;
			
			if (index > length)
				return false;
			
			for (var i : int = index; i < length - 1; ++i)
				_indices[i] = _indices[int(i + 1)];
			
			_indices.length = length - 1;
			_update = true;
			++_version;
			
			return true;
		}
		
		public function pushIndices(indices : Vector.<uint>,
									offset	: uint 	= 0,
									count	: uint	= 0) : void
		{
			var l : int = _indices.length;
			
			count ||= indices.length;
			
			for (var i : int = 0; i < count; ++i)
				_indices[int(l++)] = indices[i] + offset;

			_update = true;
			++_version;
		}
		
		minko function getIndexBuffer3D(context : Context3D) : IndexBuffer3D
		{
			if (!_nativeBuffer && _indices.length)
				_nativeBuffer = context.createIndexBuffer(_indices.length);
			
			if (_nativeBuffer && _update)
			{
				_update = false;
				
				if (_indices.length != _length)
				{
					_length = _indices.length;
					_nativeBuffer = context.createIndexBuffer(_length);
				}
				
				_nativeBuffer.uploadFromVector(_indices, 0, _indices.length);
				
				if (!_dynamic)
					_indices = null;
			}
			
			return _nativeBuffer;
		}
		
		public static function fromByteArray(data 		: ByteArray,
											 numIndices	: int) : IndexStream
		{
			var indices : Vector.<uint> = new Vector.<uint>(numIndices, true);
			var stream : IndexStream = new IndexStream();
			
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = data.readInt();
			
			stream._indices = indices;
			
			return stream;
		}
	}
}