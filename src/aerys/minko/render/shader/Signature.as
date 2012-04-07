package aerys.minko.render.shader
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	/**
	 * ShaderSignature objects describe the "signature" of ActionScript shaders and make
	 * it posible to know whether a compatible program already exists or if the shader
	 * must be forked.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public final class Signature
	{
		public static const OPERATION_EXISTS	: uint				= 1;
		public static const OPERATION_GET		: uint				= 2;
		
		public static const SOURCE_MESH			: uint				= 4;
		public static const SOURCE_SCENE		: uint				= 8;
		
		private var _keys			: Vector.<String>	= new <String>[];
		private var _values			: Vector.<Object>	= new <Object>[];
		private var _flags			: Vector.<uint>		= new <uint>[];
		
		public function get numKeys() : uint
		{
			return _keys.length;
		}
		
		public function Signature(shaderName : String)
		{
		}
		
		public function getKey(index : uint) : String
		{
			return _keys[index];
		}
		
		public function getFlags(index : uint) : uint
		{
			return _flags[index];
		}
		
		public function update(key 		: String,
							   value 	: Object,
							   flags	: uint) : void
		{
			// quit if we already have this entry
			var numKeys : uint = _keys.length;
			
			for (var keyId : uint = 0; keyId < numKeys; ++keyId)
				if (_keys[keyId] == key && _flags[keyId] == flags)
					return;
			
			// add the test to the list
			_keys.push(key);
			_values.push(value);
			_flags.push(flags);
		}
		
		public function isValid(meshBindings 	: DataBindings,
								sceneBindings	: DataBindings) : Boolean
		{
			var numKeys	: uint	= _keys.length;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key		: String		= _keys[i];
				var flags	: uint			= _flags[i];
				var value	: Object		= _values[i];
				var source	: DataBindings	= flags & SOURCE_SCENE ? sceneBindings : meshBindings;
				
				if (flags & OPERATION_GET)
				{
					if (!source.propertyExists(key))
						return false;
					
					if (!compare(source.getProperty(key), value))
						return false;
				}
				else if (flags & OPERATION_EXISTS)
				{
					if (source.propertyExists(key) != value)
						return false;
				}
			}
			
			return true;
		}
		
		public function mergeWith(otherSignature : Signature) : void
		{
			var otherLength : uint = otherSignature._keys.length;
			
			for (var i : uint = 0; i < otherLength; ++i)
				update(
					otherSignature._keys[i],
					otherSignature._values[i],
					otherSignature._flags[i]
				);
			
			
		}
		
		public function clone() : Signature
		{
			var clone : Signature = new Signature(null);
			
			clone._flags	= _flags.slice();
			clone._keys		= _keys.slice();
			clone._values	= _values.slice();
			
			return clone;
		}
		
		
		/**
		 * @fixme This is n^2!!!!!
		 * @fixme optimize me with a dictionary!!!
		 */
		public function useProperties(properties	: Vector.<String>,
									  fromScene		: Boolean) : Boolean
		{
			var numProperties	: uint	= properties.length;
			var numKeys			: uint	= _keys.length;
			var flag			: uint	= fromScene ? SOURCE_SCENE : SOURCE_MESH;
			
			for (var j : uint = 0; j < numProperties; ++j)
				for (var i : uint = 0; i < numKeys; ++i)
					if (_keys[i] == properties[j] && (flag & _flags[i]) != 0)
						return true;
			
			return false;
		}
		
		private function compare(value1 : Object, value2 : Object) : Boolean
		{
			if ((value1 == null && value2 != null) || (value1 != null && value2 == null))
				return false;
			
			var constructor	: Object	= value1.constructor;
			var count		: int		= 0;
			
			if (constructor != value2.constructor)
				return false;
			
			if (constructor == Vector4)
				return (value1 as Vector4).compareTo(value2 as Vector4, true);
			
			if (constructor == Matrix4x4)
				return (value1 as Matrix4x4).compareTo(value2 as Matrix4x4);

			if (value1 is Vector.<Number>)
			{
				var numbers1	: Vector.<Number>	= value1 as Vector.<Number>;
				var numbers2	: Vector.<Number>	= value2 as Vector.<Number>;
				
				count = numbers1.length;
				
				if (count != numbers2.length)
					return false;
				
				while (count >= 0)
				{
					if (numbers1[count] != numbers2[count])
						return false;
					
					--count;
				}
			}
			else if (value1 is Vector.<Vector4>)
			{
				var vectors1	: Vector.<Vector4>	= value1 as Vector.<Vector4>;
				var vectors2	: Vector.<Vector4>	= value2 as Vector.<Vector4>;
				
				count = vectors1.length;
				
				if (count != vectors2.length)
					return false;
				
				while (count >= 0)
				{
					if (!(vectors1[count] as Vector4).compareTo(vectors2[count]))
						return false;
					
					--count;
				}
			}
			else if (value1 is Vector.<Matrix4x4>)
			{
				var matrices1 	: Vector.<Matrix4x4>	= value1 as Vector.<Matrix4x4>;
				var matrices2 	: Vector.<Matrix4x4>	= value2 as Vector.<Matrix4x4>;
				
				count = matrices1.length;
				
				if (count != matrices2.length)
					return false;
				
				while (count >= 0)
				{
					if (!(matrices1[count] as Matrix4x4).compareTo(matrices2[count]))
						return false;
					
					--count;
				}
			}
			
			return value1 === value2;
		}
	}
}