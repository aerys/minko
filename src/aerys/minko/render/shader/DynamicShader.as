package aerys.minko.render.shader
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.type.log.LogLevel;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	public class DynamicShader extends Shader
	{
		use namespace minko;
		
		protected var _vsConstData	: Vector.<Number>;
		protected var _fsConstData	: Vector.<Number>;
		protected var _vsParams		: Vector.<ParameterAllocation>;
		protected var _fsParams		: Vector.<ParameterAllocation>;
		protected var _samplers		: Vector.<int>;
		
		public static function create(clipspacePosition	: INode,
									  color				: INode) : DynamicShader
		{
			var compiler : Compiler = new Compiler();
			compiler.load(clipspacePosition, color);
			
			if (Minko.debugLevel & LogLevel.SHADER_AGAL)
			{
				Minko.log(LogLevel.SHADER_AGAL, compiler.compileAgalVertexShader(), compiler);
				Minko.log(LogLevel.SHADER_AGAL, compiler.compileAgalFragmentShader(), compiler);
			}
			
			if (Minko.debugLevel & LogLevel.SHADER_DOTTY)
				Minko.log(LogLevel.SHADER_DOTTY, compiler.writeDotGraph(), compiler);
			
			return compiler.compileShader();
		}
	
		public function DynamicShader(vertexShader					: ByteArray,
									  fragmentShader				: ByteArray,
									  vertexInput					: Vector.<VertexComponent>,
									  vertexShaderConstantData		: Vector.<Number>,
									  fragmentShaderConstantData	: Vector.<Number>,
									  vertexShaderParameters		: Vector.<ParameterAllocation>,
									  fragmentShaderParameters		: Vector.<ParameterAllocation>,
									  samplers						: Vector.<int>)
		{
			_vsConstData	= vertexShaderConstantData;
			_fsConstData	= fragmentShaderConstantData;
			_vsParams		= vertexShaderParameters;
			_fsParams		= fragmentShaderParameters;
			_samplers		= samplers;
			
			super(vertexShader, fragmentShader, vertexInput);
		}
		
		public function fillRenderState(state	: RendererState, 
										style	: StyleStack, 
										local	: LocalData, 
										world	: Dictionary) : Boolean
		{
			setTextures(state, style, local, world);
			setConstants(state, style, local, world);
			
			state.shader = this;
			
			return true;
		}
		
		protected function setTextures(state		: RendererState,
									   styleStack	: StyleStack,
								   	   localData	: LocalData,
									   worldData	: Object) : void
		{
			var texture 		: TextureRessource	= null;
			var samplerStyleId 	: int				= 0;
			var samplerCount 	: uint 				= _samplers.length;
			
			for (var i : int = 0; i < samplerCount; ++i)
			{
				samplerStyleId = _samplers[i];
				texture = styleStack.get(samplerStyleId) as TextureRessource;
				state.setTextureAt(i, texture);
			}
		}
		
		protected function setConstants(state		: RendererState,
									    styleStack	: StyleStack,
										local		: LocalData,
										world		: Dictionary) : void
		{
			updateConstData(_vsConstData, _vsParams, styleStack, local, world);
			updateConstData(_fsConstData, _fsParams, styleStack, local, world);
			
			state.setVertexConstants(0, _vsConstData);
			state.setFragmentConstants(0, _fsConstData);
		}
		
		protected function updateConstData(constData	: Vector.<Number>, 
										   paramsAllocs	: Vector.<ParameterAllocation>, 
										   styleStack	: StyleStack,
										   local		: LocalData,
										   world		: Dictionary) : void
		{
			var paramLength	: int = paramsAllocs.length;
			
			for (var i : int = 0; i < paramLength; ++i)
			{
				var paramAlloc	: ParameterAllocation	= paramsAllocs[i];
				var param		: AbstractParameter		= paramAlloc._parameter;
				
				var data : Object = getParameterData(param, styleStack, local, world);
				
				loadParameterData(paramAlloc, constData, data);
			}
		}
		
		private function getParameterData(param			: AbstractParameter,
										  styleStack	: StyleStack,
										  local			: LocalData,
										  world			: Dictionary) : Object
		{
			if (param is StyleParameter)
			{
				if (param._index != -1)
				{
					return styleStack.get(param._key as int).getItem(param._index)[param._field];
				}
				else if (param._field != null)
				{
					return styleStack.get(param._key as int)[param._field];
				}
				else
				{
					return styleStack.get(param._key as int, null);
				}
			}
			else if (param is WorldParameter)
			{
				var paramClass : Class = WorldParameter(param)._class;
				
				if (param._index != -1)
				{
					return world[paramClass].getItem(param._index)[param._field];
				}
				else if (param._field != null)
				{
					return world[paramClass][param._field];
				}
				else
				{
					return world[paramClass];
				}
			}
			else if (param is TransformParameter)
			{
				return local[param._key];
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
			
			if (data is int)
			{
				var intData : int = data as int;
				
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
			else if (data is uint)
			{
				var uintData : uint = data as uint;
				
				if (size == 1)
				{
					constData[offset] = uintData;
				}
				else if (size == 2)
				{
					constData[offset] = ((uintData & 0xFFFF0000) >>> 16) / Number(0xffff);
					constData[int(offset + 2)] = (uintData & 0x0000FFFF) / Number(0xffff);
				}
				else if (size == 3)
				{
					constData[offset] = ((uintData & 0xFF0000) >>> 16) / 255.;
					constData[int(offset + 1)] = ((uintData & 0x00FF00) >>> 8) / 255.;
					constData[int(offset + 2)] = (uintData & 0x0000FF) / 255.;
				}
				else if (size == 4)
				{
					
					constData[offset] = ((uintData & 0xFF000000) >>> 24) / 255.;
					constData[int(offset + 1)] = ((uintData & 0x00FF0000) >>> 16) / 255.;
					constData[int(offset + 2)] = ((uintData & 0x0000FF00) >>> 8) / 255.;
					constData[int(offset + 3)] = ((uintData & 0x000000FF)) / 255.;
				}
			}
			else if (data is Number)
			{
				if (size != 1)
					throw new Error('Parameter ' + paramAlloc.toString() + ' is ' +
						'defined as size=' + size + ' but only a Number was found');
				
				constData[offset] = data as Number;
			}
			else if (data is Vector4)
			{
				var vectorData : Vector4 = data as Vector4;
				
				constData[offset] = vectorData.x;
				size >= 2 && (constData[int(offset + 1)] = vectorData.y);
				size >= 3 && (constData[int(offset + 2)] = vectorData.z);
				size >= 4 && (constData[int(offset + 3)] = vectorData.w);
			}
			else if (data is Matrix4x4)
			{
				(data as Matrix4x4).getRawData(constData, offset, true);
			}
			else if (data is Vector.<Vector4>)
			{
				var vectorVectorData		: Vector.<Vector4>	= data as Vector.<Vector4>;
				var vectorVectorDataLength	: uint				= vectorVectorData.length;
				
				for (var j : uint = 0; j < vectorVectorDataLength; ++j)
				{
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
				{
					matrixVectorData[i].getRawData(constData, offset + i * 16, true);
				}
			}
			else if (data == null)
			{
				throw new Error('Parameter ' + paramAlloc.toString() + ' is ' +
					'null and required by automatic shader');	
			}
			else
			{
				throw new Error('Parameter ' + paramAlloc.toString() + ' is ' +
					'neither a int, a Number, a Vector4 or a Matrix4x4. Unable to ' +
					'map it to a shader constant.');	
			}
		}
		
	}
}