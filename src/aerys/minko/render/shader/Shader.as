package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	public class Shader implements IShader
	{
		use namespace minko;

		protected var _name						: String;
		
		protected var _resource					: Program3DResource;

		protected var _lastFrameId				: uint;
		protected var _lastStyleStackVersion	: uint;
		protected var _lastTransformVersion		: uint;

		protected var _vsConstData				: Vector.<Number>;
		protected var _fsConstData				: Vector.<Number>;
		protected var _vsParams					: Vector.<ParameterAllocation>;
		protected var _fsParams					: Vector.<ParameterAllocation>;
		protected var _samplers					: Vector.<int>;
		
		public function get name() : String
		{
			return _name;
		}

		public function get resource() : Program3DResource
		{
			return _resource;
		}

		public static function create(name				: String,
									  outputPosition	: INode,
									  outputColor		: INode) : Shader
		{
			var compiler 	: Compiler 	= new Compiler();
			
			compiler.load(outputPosition, outputColor);
			
			return compiler.compileShader(name);
		}

		public function Shader(name						: String,
							   vertexShader				: ByteArray,
							   fragmentShader			: ByteArray,
							   vertexInputComponents	: Vector.<VertexComponent>,
							   vertexInputIndices		: Vector.<uint>,
							   samplers					: Vector.<int>,
							   vertexConstantData		: Vector.<Number>,
							   fragmentConstantData		: Vector.<Number>,
							   vertexParameters			: Vector.<ParameterAllocation>,
							   fragmentParameters		: Vector.<ParameterAllocation>)
		{
			_lastFrameId			= uint.MAX_VALUE;
			_lastStyleStackVersion	= uint.MAX_VALUE;
			_lastTransformVersion	= uint.MAX_VALUE;

			_vsConstData	= vertexConstantData;
			_fsConstData	= fragmentConstantData;
			_vsParams		= vertexParameters;
			_fsParams		= fragmentParameters;
			_samplers		= samplers;

			_resource 		= new Program3DResource(name,
													vertexShader,
													fragmentShader,
													vertexInputComponents,
													vertexInputIndices);
		}

		public function fillRenderState(state			: RendererState,
										styleData		: StyleData,
										transformData	: TransformData,
										worldData		: Dictionary) : void
		{
			if (_lastFrameId != worldData[ViewportData].frameId)
			{
				_lastFrameId			= worldData[ViewportData].frameId;
				_lastStyleStackVersion	= uint.MAX_VALUE;
				_lastTransformVersion	= uint.MAX_VALUE;
			}

			setTextures(state, styleData, transformData, worldData);
			setConstants(state, styleData, transformData, worldData);

			state.program = _resource;
			_lastStyleStackVersion	= styleData.version;
			_lastTransformVersion	= transformData.version;
		}

		protected function setTextures(state			: RendererState,
									   styleData		: StyleData,
								   	   transformData	: TransformData,
									   worldData		: Object) : void
		{
			var texture 		: Texture3DResource	= null;
			var samplerStyleId 	: int				= 0;
			var samplerCount 	: uint 				= _samplers.length;

			for (var i : int = 0; i < samplerCount; ++i)
			{
				samplerStyleId	= _samplers[i];
				texture			= styleData.get(samplerStyleId) as Texture3DResource;

				if (!texture)
				{
					throw new Error("Texture '" + Style.getStyleName(samplerStyleId)
									+ "' is required by shader.");
				}
				
				state.setTextureAt(i, texture);
			}
		}

		protected function setConstants(state			: RendererState,
									    styleData		: StyleData,
										transformData	: TransformData,
										worldData		: Dictionary) : void
		{
			updateConstData(_vsConstData, _vsParams, styleData, transformData, worldData);
			updateConstData(_fsConstData, _fsParams, styleData, transformData, worldData);

			state.setVertexConstants(_vsConstData);
			state.setFragmentConstants(_fsConstData);
		}

		protected function updateConstData(constData		: Vector.<Number>,
										   paramsAllocs		: Vector.<ParameterAllocation>,
										   styleData		: StyleData,
										   transformData	: TransformData,
										   worldData		: Dictionary) : void
		{
			var paramLength	: int = paramsAllocs.length;

			for (var i : int = 0; i < paramLength; ++i)
			{
				var paramAlloc	: ParameterAllocation	= paramsAllocs[i];
				var param		: AbstractParameter		= paramAlloc._parameter;

				if ((param is StyleParameter && styleData.version == _lastStyleStackVersion) ||
					(param is TransformParameter && transformData.version == _lastTransformVersion))
					continue;

				var data : Object = getParameterData(param, styleData, transformData, worldData);

				loadParameterData(paramAlloc, constData, data);
			}
		}

		private function getParameterData(param			: AbstractParameter,
										  styleData		: StyleData,
										  transformData	: TransformData,
										  worldData		: Dictionary) : Object
		{
			if (param is StyleParameter)
			{
				var defaultValue	: Object	= (param as StyleParameter).defaultValue;
				var value			: Object	= styleData.get(param._key as int, defaultValue);

				if (param._index != -1)
					return value.getItem(param._index)[param._field];
				else if (param._field != null)
					return value[param._field];
				else
					return value;
			}
			else if (param is WorldParameter)
			{
				var paramClass : Class = WorldParameter(param)._class;

				if (param._index != -1)
					return worldData[paramClass].getItem(param._index)[param._field];
				else if (param._field != null)
					return worldData[paramClass][param._field];
				else
					return worldData[paramClass];
			}
			else if (param is TransformParameter)
			{
				return transformData[param._key];
			}
			else
				throw new Error('Unknown parameter type');
		}

		private function loadParameterData(paramAlloc	: ParameterAllocation,
										   constData	: Vector.<Number>,
										   data			: Object) : void
		{
			var offset	: uint	= paramAlloc._offset;
			var size	: uint	= paramAlloc._parameter._size;

//			if (data is int)
			if (data is Number)
			{
				//var intData : int = data as int;
				var intData : Number = data as Number;

				if (size == 1)
				{
					constData[offset] = intData;
				}
				else if (size == 2)
				{
					constData[offset] = ((intData & 0xFFFF0000) >>> 16) / Number(0xffff);
					constData[int(offset + 2)] = (intData & 0x0000FFFF) / Number(0xffff);
				}
				else if (size == 3)
				{
					constData[offset] = ((intData & 0xFF0000) >>> 16) / 255.;
					constData[int(offset + 1)] = ((intData & 0x00FF00) >>> 8) / 255.;
					constData[int(offset + 2)] = (intData & 0x0000FF) / 255.;
				}
				else if (size == 4)
				{
					constData[offset] = ((intData & 0xFF000000) >>> 24) / 255.;
					constData[int(offset + 1)] = ((intData & 0x00FF0000) >>> 16) / 255.;
					constData[int(offset + 2)] = ((intData & 0x0000FF00) >>> 8) / 255.;
					constData[int(offset + 3)] = ((intData & 0x000000FF)) / 255.;
				}
			}
			else if (data is Vector4)
			{
				var vectorData : Vector3D = (data as Vector4)._vector;

				constData[offset] = vectorData.x;
				size >= 2 && (constData[int(offset + 1)] = vectorData.y);
				size >= 3 && (constData[int(offset + 2)] = vectorData.z);
				size >= 4 && (constData[int(offset + 3)] = vectorData.w);
			}
			else if (data is Matrix4x4)
			{
				(data as Matrix4x4).getRawData(constData, offset, true);
			}
			else if (data is Vector.<Number>)
			{
				var vectorNumberData		: Vector.<Number>	= data as Vector.<Number>;
				var vectorNumberDatalength	: uint				= vectorNumberData.length;
				
				for (var k : int = 0; k < size; ++k)
				{
					constData[offset + k] = vectorNumberData[k];
				}
			}
			else if (data is Vector.<Vector4>)
			{
				var vectorVectorData		: Vector.<Vector4>	= data as Vector.<Vector4>;
				var vectorVectorDataLength	: uint				= vectorVectorData.length;

				for (var j : uint = 0; j < vectorVectorDataLength; ++j)
				{
					vectorData = vectorVectorData[j]._vector;

					constData[offset + 4 * j] = vectorData.x;
					constData[int(offset + 4 * j + 1)] = vectorData.y;
					constData[int(offset + 4 * j + 2)] = vectorData.z;
					constData[int(offset + 4 * j + 3)] = vectorData.w;
				}
			}
			else if (data is Vector.<Matrix4x4>)
			{
				var matrixVectorData		: Vector.<Matrix4x4>	= data as Vector.<Matrix4x4>;
				var matrixVectorDataLength	: uint					= matrixVectorData.length;

				for (var i : uint = 0; i < matrixVectorDataLength; ++i)
					matrixVectorData[i].getRawData(constData, offset + i * 16, true);
			}
			else if (data == null)
			{
				throw new Error(paramAlloc.toString() + ' is null but required by shader.');
			}
			else
			{
				throw new Error(paramAlloc.toString() + ' has an unsupported type.');
			}
		}
	}
}
