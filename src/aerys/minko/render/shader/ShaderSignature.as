package aerys.minko.render.shader
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public final class ShaderSignature
	{
		public static const OPERATION_COUNT		: uint				= 1;
		public static const OPERATION_GET		: uint				= 2;
		
		public static const SOURCE_MESH			: uint				= 4;
		public static const SOURCE_SCENE		: uint				= 8;
		
		private static const TMP_NUMBERS		: Vector.<Number>	= new <Number>[];
		
		private var _shader	: Shader			= null;
		private var _hash	: String			= null;
		private var _keys	: Vector.<String>	= new <String>[];
		private var _values	: Vector.<Object>	= new <Object>[];
		private var _flags	: Vector.<uint>		= new <uint>[];
		
		public function get shader() : Shader
		{
			return _shader;
		}
		
		public function get hash() : String
		{
			return _hash;
		}
		
		public function get numKeys() : uint
		{
			return _keys.length;
		}
		
		public function ShaderSignature(shader : Shader)
		{
			_shader = shader;
			_hash = _shader.name;
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
			_keys.push(key);
			_values.push(value);
			_flags.push(flags);
			
			// update hash
			_hash += "_";
			
			if (flags & OPERATION_GET)
			{
				if (flags & SOURCE_SCENE)
					_hash += "scene.get('" + key + "')";
				else
					_hash += "mesh.get('" + key + "')";
			}
			else if (flags & OPERATION_COUNT)
			{
				if (flags & SOURCE_SCENE)
					_hash += "scene.count('" + key + "')";
				else
					_hash += "mesh.count('" + key + "')";
			}
			else
				throw new Error();
			
			_hash += "=" + value;
		}
		
		public function isValid(meshBindings 	: DataBindings,
								sceneBindings	: DataBindings) : Boolean
		{
			var numKeys	: int	= _keys.length;
			
			for (var i : int = 0; i < numKeys; ++i)
			{
				var key		: String		= _keys[i];
				var flags	: uint			= _flags[i];
				var value	: Object		= _values[i];
				var source	: DataBindings	= flags & SOURCE_SCENE
					? sceneBindings
					: meshBindings;
				
				if (flags & OPERATION_GET)
					return compare(source.getProperty(key), value);
				else if (flags & OPERATION_COUNT)
					return (source.getProperty(key) ? 1 : 0) == value;
			}
			
			return true;
		}
		
		public function checkProperty(propertyName	: String,
									  value			: Object,
									  checkFlags	: uint) : Boolean
		{
			var propertyId	: int	= _keys.length - 1;
			
			while (propertyId >= 0 && _keys[propertyId] != propertyName)
				--propertyId;
			
			if (propertyId >= 0)
			{
				var flags : uint = _flags[propertyId];
				
				if ((flags & checkFlags) == 0)
					return false;
				
				if (flags & OPERATION_GET)
					return !compare(value, _values[propertyId]);
				else if (flags & OPERATION_COUNT)
					return value == null && _values[propertyId] == 0;
			}
			
			return true;
		}
		
		private function compare(value1 : Object, value2 : Object) : Boolean
		{
			var constructor	: Object	= value1.constructor;
			var count		: int		= 0;
			
			if (constructor != value2.constructor)
				return true;
			
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