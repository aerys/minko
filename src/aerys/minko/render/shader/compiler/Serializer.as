package aerys.minko.render.shader.compiler
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import avmplus.getQualifiedClassName;
	
	import flash.geom.Point;
	import flash.geom.Vector3D;

	/**
	 * The Serializer class provides a set of static methods to serialize
	 * CPU values into Vector.&lt;Number&gt; than can be used as shader constants.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Serializer
	{
		public static function serializeKnownLength(data	: Object, 
													target	: Vector.<Number>, 
													offset	: uint, 
													size	: uint) : void
		{
			if (data is Number)
			{
				var intData : Number = Number(data);
				
				if (size == 1)
				{
					target[offset] = intData;
				}
				else if (size == 2)
				{
					target[offset] = ((intData & 0xFFFF0000) >>> 16) / Number(0xffff);
					target[int(offset + 2)] = (intData & 0x0000FFFF) / Number(0xffff);
				}
				else if (size == 3)
				{
					target[offset] = ((intData & 0xFF0000) >>> 16) / 255.;
					target[int(offset + 1)] = ((intData & 0x00FF00) >>> 8) / 255.;
					target[int(offset + 2)] = (intData & 0x0000FF) / 255.;
				}
				else if (size == 4)
				{
					target[offset] = ((intData & 0xFF000000) >>> 24) / 255.;
					target[int(offset + 1)] = ((intData & 0x00FF0000) >>> 16) / 255.;
					target[int(offset + 2)] = ((intData & 0x0000FF00) >>> 8) / 255.;
					target[int(offset + 3)] = ((intData & 0x000000FF)) / 255.;
				}
			}
			else if (data is Vector4)
			{
				var vectorData : Vector3D = Vector4(data).minko_math::_vector;
				
				target[offset] = vectorData.x;
				size >= 2 && (target[int(offset + 1)] = vectorData.y);
				size >= 3 && (target[int(offset + 2)] = vectorData.z);
				size >= 4 && (target[int(offset + 3)] = vectorData.w);
			}
			else if (data is Matrix4x4)
			{
				Matrix4x4(data).getRawData(target, offset, true);
			}
			else if (data is Vector.<Number>)
			{
				var vectorNumberData		: Vector.<Number>	= Vector.<Number>(data);
				var vectorNumberDatalength	: uint				= vectorNumberData.length;
				
				for (var k : int = 0; k < size && k < vectorNumberDatalength; ++k)
					target[offset + k] = vectorNumberData[k];
			}
			else if (data is Vector.<Vector4>)
			{
				var vectorVectorData		: Vector.<Vector4>	= Vector.<Vector4>(data);
				var vectorVectorDataLength	: uint				= vectorVectorData.length;
				
				for (var j : uint = 0; j < vectorVectorDataLength; ++j)
				{
					vectorData = vectorVectorData[j].minko_math::_vector;
					
					target[offset + 4 * j] = vectorData.x;
					target[int(offset + 4 * j + 1)] = vectorData.y;
					target[int(offset + 4 * j + 2)] = vectorData.z;
					target[int(offset + 4 * j + 3)] = vectorData.w;
				}
			}
			else if (data is Vector.<Matrix4x4>)
			{
				var matrixVectorData		: Vector.<Matrix4x4>	= Vector.<Matrix4x4>(data);
				var matrixVectorDataLength	: uint					= matrixVectorData.length;
				
				for (var i : uint = 0; i < matrixVectorDataLength; ++i)
					matrixVectorData[i].getRawData(target, offset + i * 16, true);
			}
			else if (data == null)
			{
				throw new Error('You cannot serialize a null value.');
			}
			else
			{
				throw new Error('Unsupported type:' + getQualifiedClassName(data));
			}
		}
		
		public static function serializeUnknownLength(data		: Object, 
													  target	: Vector.<Number>, 
													  offset	: uint) : void
		{
			if (data is Number)
			{
				target[offset] = Number(data);
			}
			else if (data is Point)
			{
				var pointData : Point = Point(data);
				
				target[offset]			= pointData.x;
				target[int(offset + 1)]	= pointData.y;
			}
			else if (data is Vector4)
			{
				var vectorData : Vector3D = Vector4(data).minko_math::_vector;
				
				target[offset]			= vectorData.x;
				target[int(offset + 1)]	= vectorData.y;
				target[int(offset + 2)]	= vectorData.z;
				target[int(offset + 3)]	= vectorData.w;
			}
			else if (data is Matrix4x4)
			{
				Matrix4x4(data).getRawData(target, offset, true);
			}
			else if (data is Array || data is Vector.<Number> || data is Vector.<Vector4> || data is Vector.<Matrix4x4>)
			{
				for each (var datum : Object in data)
				{
					serializeUnknownLength(datum, target, offset);
					offset = target.length;
				}
			}
			else if (data == null)
			{
				throw new Error('You cannot serialize a null value.');
			}
			else
			{
				throw new Error('Unsupported type:' + getQualifiedClassName(data));
			}
		}
	}
}