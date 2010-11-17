package aerys.minko.type.stream
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.ByteArray;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public final dynamic class IndexStream3D extends Proxy
	{
		use namespace minko;
		
		minko var _indices		: Vector.<uint>		= null;
		minko var _nativeBuffer : IndexBuffer3D		= null;
		
		private var _update			: Boolean			= true;
		private var _length			: int				= 0;
		private var _dynamic		: Boolean			= false;
		private var _bufferLength	: int				= 0;
		
		public function get length() : int			{ return _length; }
		
		public function set length(value : int) : void
		{
			_indices.length = value;
			_update = true;
		}
		
		public static function dummy(size : int) : IndexStream3D
		{
			var indices : Vector.<uint> = new Vector.<uint>();
			
			indices.length = size;
			for (var i : int = 0; i < size; ++i)
				indices[i] = i;
			
			return new IndexStream3D(indices, size, false);
		}
		
		public function IndexStream3D(data 		: Vector.<uint> = null,
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
			var numIndices : int = Math.min(indices ? indices.length : 0,
											_length);
			
			_indices = new Vector.<uint>(_length, _dynamic);
			for (var i : int = 0; i < numIndices; ++i)
				_indices[i] = indices[i];
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
		
		public function clone() : IndexStream3D
		{
			return new IndexStream3D(_indices, _length, _dynamic);
		}
		
		public function toString() : String
		{
			return _indices.toString();
		}
		
		public function concat(myIndexBuffer : IndexStream3D) : void
		{
			var numIndices : int = _indices.length;
			var toConcat : Vector.<uint> = myIndexBuffer._indices;
			var numIndices2 : int = toConcat.length;
			
			for (var i : int = 0; i < numIndices2; ++i, ++numIndices)
				_indices[numIndices] = toConcat[i];
			
			_update = true;
		}
		
		public function push(...parameters) : IndexStream3D
		{
			var numIndices : int = parameters.length;
			var length : int = length;

			for (var i : int = 0; i < numIndices; ++i, ++length)
				_indices[length] = int(parameters[i]);
			
			_update = true;
			
			return this;
		}
		
		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			var index : int = int(name);
			var length : int = length;
			
			if (index > length)
				return false;
			
			for (var i : int = index; i < length - 1; ++i)
				_indices[i] = _indices[int(i + 1)];
			
			_indices.length = length - 1;
			_update = true;
			
			return true;
		}
		
		public function pushIndices(indices : Vector.<uint>,
									offset	: uint 	= 0,
									count	: uint	= 0) : void
		{
			var l : int = _indices.length;
			
			count ||= indices.length - offset;
			
			for (var i : int = 0; i < count; ++i)
				_indices[int(l++)] = indices[int(offset + i)];
			
			_update = true;
		}
		
		public function prepare(context : Context3D) : void
		{
			if (!_nativeBuffer && _length)
				_nativeBuffer = context.createIndexBuffer(_length);
			
			if (_update)
			{
				if (_indices.length != _length)
				{
					_length = _indices.length;
					_nativeBuffer = context.createIndexBuffer(_length);
				}
				
				_nativeBuffer.upload(_indices, 0, _indices.length);
				_update = false;
			}
		}
		
		public static function fromByteArray(data 		: ByteArray,
											 numIndices	: int) : IndexStream3D
		{
			var indices : Vector.<uint> = new Vector.<uint>(numIndices, true);
			var stream : IndexStream3D = new IndexStream3D();
			
			for (var i : int = 0; i < numIndices; ++i)
				indices[i] = data.readInt();
			
			stream._indices = indices;
			
			return stream;
		}
	}
}