package aerys.minko.type.stream
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public final dynamic class IndexStream3D extends Proxy
	{
		use namespace minko;
		
		private static const INDEX_BUFFER	: Factory	= Factory.getFactory(IndexStream3D);
		
		minko var _indices		: Vector.<uint>		= null;
		minko var _nativeBuffer : IndexBuffer3D		= null;
		
		private var _update		: Boolean			= true;
		
		public function get length() : int			{ return _indices.length; }
		
		public function set length(value : int) : void
		{
			_indices.length = value;
			_update = true;
		}
		
		public static function dummy(myLength : int) : IndexStream3D
		{
			var ib : IndexStream3D = INDEX_BUFFER.create(); 
			var indices : Vector.<uint> = ib._indices;
			
			indices.length = myLength;
			for (var i : int = 0; i < myLength; ++i)
				indices[i] = i;
			
			return ib;
		}
		
		public function IndexStream3D(myData : Vector.<uint> = null)
		{
			super();
			
			initialize(myData);
		}
		
		private function initialize(myIndices : Vector.<uint>) : void
		{
			var numIndices : int = myIndices ? myIndices.length : 0;
			
			_indices = new Vector.<uint>(numIndices);
			for (var i : int = 0; i < numIndices; ++i)
				_indices[i] = myIndices[i];
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			return _indices[int(name)];
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			_indices[int(name)] = int(value);
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
		
		public function setIndices(myIndices : Vector.<uint>) : void
		{
			var length : int = myIndices.length;

			for (var i : int = 0; i < length; ++i)
				_indices[i] = myIndices[i];
			
			_indices.length = length;
			_update = true;
		}
		
		public function clone(myIndexBuffer : IndexStream3D = null) : IndexStream3D
		{
			var clone : IndexStream3D = myIndexBuffer || INDEX_BUFFER.create();
			var cloneIndices : Vector.<uint> = clone._indices;
			var numIndices : int = _indices.length;
			
			cloneIndices.length = numIndices;
			for (var i : int = 0; i < numIndices; ++i)
				cloneIndices[i] = _indices[i];
			
			return clone;
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
		
		public function prepare(myContext : Context3D) : void
		{
			if (!_nativeBuffer)
			{
				_nativeBuffer = myContext.createIndexBuffer(length);
			}
			
			if (_update)
			{
				_update = false;
				_nativeBuffer.upload(_indices, 0, length);
			}
		}
	}
}