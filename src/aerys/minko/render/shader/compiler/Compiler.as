package aerys.minko.render.shader.compiler
{
	import aerys.minko.Minko;
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.compiler.allocator.Allocation;
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.allocator.AttributeAllocator;
	import aerys.minko.render.shader.compiler.allocator.ConstantAllocator;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.compiler.allocator.VaryingAllocator;
	import aerys.minko.render.shader.compiler.register.RegisterLimit;
	import aerys.minko.render.shader.compiler.visitor.allocator.FragmentAllocator;
	import aerys.minko.render.shader.compiler.visitor.allocator.VertexAllocator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.ConstantDuplicator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.DummyRemover;
	import aerys.minko.render.shader.compiler.visitor.preprocess.Merger;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteAgal;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteByteCode;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteDot;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.RootWrapper;
	import aerys.minko.type.log.DebugLevel;
	import aerys.minko.type.stream.format.VertexComponent;

	import flash.display.BitmapData;
	import flash.geom.Matrix;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	public class Compiler
	{
		internal static const TMP_ALLOC_FIELD_WIDTH		: uint = 20;
		internal static const TMP_ALLOC_FIELD_HEIGHT	: uint = 25;

		// Nodes
		protected var _clipspacePosNode			: INode;
		protected var _vertexOps				: Vector.<INode>;
		protected var _colorNode				: INode;

		// Allocators
		protected var _attrAllocator			: AttributeAllocator;
		protected var _varyingAllocator			: VaryingAllocator;
		protected var _vsTmpAllocator			: Allocator;
		protected var _vsConstAllocator			: ConstantAllocator;
		protected var _fsTmpAllocator			: Allocator;
		protected var _fsConstAllocator			: ConstantAllocator;

		// Additional data that will be needed for shader usage
		protected var _vertexInputComponents	: Vector.<VertexComponent>;
		protected var _vertexInputIndices		: Vector.<uint>;
		protected var _vsConstData				: Vector.<Number>;
		protected var _fsConstData				: Vector.<Number>;
		protected var _vsParams					: Vector.<ParameterAllocation>;
		protected var _fsParams					: Vector.<ParameterAllocation>;
		protected var _samplers					: Vector.<int>;

		public function load(clipspacePos	: INode,
							 color			: INode) : void
		{
			reset();

			_clipspacePosNode	= clipspacePos;
			_colorNode			= color;

			if (Minko.debugLevel & DebugLevel.SHADER_ANTECOMPILE_DOTTY)
			{
				Minko.log(DebugLevel.SHADER_ANTECOMPILE_DOTTY, writeDotGraph());
			}

			wrapRootNodes();
			removeDummyNodes();

			preprocess();
			splitAndReportMemoryUsage();

			if (Minko.debugLevel & DebugLevel.SHADER_POSTCOMPILE_DOTTY)
			{
				Minko.log(DebugLevel.SHADER_POSTCOMPILE_DOTTY, writeDotGraph());
			}

			allocateRegistries();
			createAllocationTables();

			if (Minko.debugLevel & DebugLevel.SHADER_ATTR_ALLOC)
			{
				Minko.log(DebugLevel.SHADER_ATTR_ALLOC, writeAttributeAllocationSummary());
			}

			if (Minko.debugLevel & DebugLevel.SHADER_CONST_ALLOC)
			{
				Minko.log(DebugLevel.SHADER_CONST_ALLOC, writeConstantAllocationSummary(true));
				Minko.log(DebugLevel.SHADER_CONST_ALLOC, writeConstantAllocationSummary(false));
			}

			if (Minko.debugLevel & DebugLevel.SHADER_AGAL)
			{
				Minko.log(DebugLevel.SHADER_AGAL, compileAgalVertexShader());
				Minko.log(DebugLevel.SHADER_AGAL, compileAgalFragmentShader());
			}

		}

		protected function reset() : void
		{
			_vertexOps				= new Vector.<INode>();
			_vertexInputComponents	= new Vector.<VertexComponent>();
			_vertexInputIndices		= new Vector.<uint>();
			_colorNode				= null;

			_attrAllocator			= new AttributeAllocator();
			_varyingAllocator		= new VaryingAllocator();
			_vsTmpAllocator			= new Allocator(RegisterLimit.VS_MAX_TEMPORARY);
			_vsConstAllocator		= new ConstantAllocator(true);
			_fsTmpAllocator			= new Allocator(RegisterLimit.FG_MAX_TEMPORARY);
			_fsConstAllocator		= new ConstantAllocator(false);
		}

		protected function wrapRootNodes() : void
		{
			_clipspacePosNode	= new RootWrapper(_clipspacePosNode);
			_colorNode			= new RootWrapper(_colorNode);
		}

		protected function removeDummyNodes() : void
		{
			var dummyRemover : DummyRemover = new DummyRemover();

			dummyRemover.processShader(_clipspacePosNode, _colorNode);

			_clipspacePosNode	= dummyRemover.clipspacePos;
			_colorNode			= dummyRemover.color;
		}

		protected function preprocess() : void
		{
			new Merger().processShader(_clipspacePosNode, _colorNode);
			new ConstantDuplicator().processShader(_clipspacePosNode, _colorNode);
		}

		protected function splitAndReportMemoryUsage() : void
		{
			// Split vertex and fragment shader, report registry usage to allocators
			var vertexAllocator	: VertexAllocator =
				new VertexAllocator(_attrAllocator, _vsTmpAllocator, _vsConstAllocator);

			var fragmentAllocator	: FragmentAllocator =
				new FragmentAllocator(_fsTmpAllocator, _varyingAllocator, _fsConstAllocator);

			fragmentAllocator.processFragmentShader(_colorNode);

			_samplers	= fragmentAllocator.samplers;
			_vertexOps	= fragmentAllocator.vertexShaderOutputs;
			_vertexOps.unshift(_clipspacePosNode);

			vertexAllocator.processVertexShader(_vertexOps);
		}

		protected function allocateRegistries() : void
		{
			// Allocate all registries on both shaders (va, vt, vc, v, ft, fs, fc)
			_attrAllocator		.computeRegisterState(true);
			_varyingAllocator	.computeRegisterState(true);
			_vsTmpAllocator		.computeRegisterState(false);
			_vsConstAllocator	.computeRegisterState(false);
			_fsTmpAllocator		.computeRegisterState(false);
			_fsConstAllocator	.computeRegisterState(false);
		}

		protected function createAllocationTables() : void
		{
			var attributes		: Vector.<Attribute>	= _attrAllocator.getAllocations();
			var attributeCount	: uint					= attributes.length;
			for (var i : int = 0; i < attributeCount; ++i)
			{
				var attribute : Attribute = attributes[i];
				_vertexInputComponents.push(attribute.component);
				_vertexInputIndices.push(attribute.componentIndex);
			}

			_vsConstData	= _vsConstAllocator.computeConstantAllocation();
			_fsConstData	= _fsConstAllocator.computeConstantAllocation();
			_vsParams		= _vsConstAllocator.computeParameterAllocation();
			_fsParams		= _fsConstAllocator.computeParameterAllocation();
		}

		public function compileShader() : Shader
		{
			var vertexShader	: ByteArray = compileVertexShader();
			var fragmentShader	: ByteArray	= compileFragmentShader();

			return new Shader(
				vertexShader, fragmentShader,
				_vertexInputComponents, _vertexInputIndices,
				_samplers,
				_vsConstData, _fsConstData,
				_vsParams, _fsParams
			);
		}

		protected function compileVertexShader() : ByteArray
		{
			return new WriteByteCode(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processVertexShader(_vertexOps);
		}

		protected function compileFragmentShader() : ByteArray
		{
			return new WriteByteCode(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processFragmentShader(_colorNode);
		}

		public function compileAgalVertexShader() : String
		{
			return new WriteAgal(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processVertexShader(_vertexOps);
		}

		public function compileAgalFragmentShader() : String
		{
			return new WriteAgal(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processFragmentShader(_colorNode);
		}

		public function writeDotGraph() : String
		{
			return new WriteDot().processShader(_clipspacePosNode, _colorNode);
		}

		public function writeAttributeAllocationSummary() : String
		{
			var result : String = '';
			var inputCount : uint = _vertexInputComponents.length;
			for (var i : uint = 0; i < inputCount; ++i)
				result += 'va' + i + "             " + _vertexInputComponents[i].toString() + "\n";
			return result;
		}

		public function writeConstantAllocationSummary(vertexShader : Boolean	= true) : String
		{
			var offsetToLetter		: Vector.<String> = Vector.<String>(['x', 'y', 'z', 'w']);
			var summary				: String = '';
			var chars				: uint;

			var allocationTable 	: Vector.<ParameterAllocation>	= vertexShader ? _vsParams : _fsParams;
			var constantTable		: Vector.<Number>				= vertexShader ? _vsConstData : _fsConstData;

			var occupiedRegisters	: uint = constantTable.length / 4;
			for (var registerId : uint = 0; registerId < occupiedRegisters; ++registerId)
				for (var offsetId : uint = 0; offsetId < 4; ++offsetId)
				{
					var foundInParameterTable	: Boolean	= false;
					for each (var allocation : ParameterAllocation in allocationTable)
					{
						chars = summary.length;

						if (allocation.offset == 4 * registerId + offsetId)
						{
							foundInParameterTable = true;

							summary += (vertexShader ? "vc" : "fc");

							if (allocation.size < 4)
							{
								summary += registerId + '.';

								for (var i : uint = allocation.offset % 4; i < allocation.offset % 4 + allocation.size && i < 4; ++i)
								{
									summary += offsetToLetter[i];
								}
							}
							else if (allocation.size == 4)
							{
								summary += registerId;
							}
							else if (allocation.size > 4)
							{
								summary += '[' + registerId + '-' + (- 1 + registerId + allocation.size / 4) + ']';
							}

							chars = summary.length - chars;

							for (; chars < 16; ++chars)
								summary += " ";

							var parameter : AbstractParameter = allocation.parameter;
							if (parameter is StyleParameter)
							{
								var styleParam : StyleParameter = parameter as StyleParameter;
								summary += "StyleParameter['" + Style.getStyleName(styleParam.key as uint) + "']\n";
							}
							else if (parameter is WorldParameter)
							{
								var worldParam : WorldParameter = parameter as WorldParameter;
								summary += "WorldParameter[class='"
									+ worldParam.className + "', index='"
									+ worldParam.index + "', field='"
									+ worldParam.field + "']\n";
							}
							else if (parameter is TransformParameter)
							{
								var transformParam : TransformParameter = parameter as TransformParameter;
								summary += "TransformParameter[key='"
									+ transformParam.key + "']\n";
							}

							break;
						}
						else if (allocation.offset < 4 * registerId + offsetId
							&& allocation.offset + allocation.size > 4 * registerId + offsetId)
						{
							foundInParameterTable = true;
							break;
						}
					}

					if (!foundInParameterTable)
					{
						chars = summary.length;
						summary += (vertexShader ? "vc" : "fc") + registerId.toString()
							+ '.' + offsetToLetter[offsetId];
						chars = summary.length - chars;

						for (; chars < 16; ++chars)
							summary += " ";

						summary += "Constant[value='" + constantTable[4 * registerId + offsetId] + "']\n";
					}
				}

			return summary;
		}

		public function writeTemporaryAllocationSummary(vertexShader : Boolean = true) : BitmapData
		{
			var allocations : Vector.<Allocation>	= vertexShader ? _vsTmpAllocator.allocations : _fsTmpAllocator.allocations;
			var allocation	: Allocation;

			var maxOpId 	: uint = 0;
			var maxOffset	: uint = 0;
			for each (allocation in allocations)
			{
				if (allocation.endId > maxOpId)
					maxOpId = allocation.endId;
				if (allocation.offset + allocation.size > maxOffset)
					maxOffset = allocation.offset + allocation.size
			}
			var bitmapData	: BitmapData	= new BitmapData(
				TMP_ALLOC_FIELD_HEIGHT * maxOffset, TMP_ALLOC_FIELD_WIDTH * maxOpId, false, 0xFFFFFF);

			var textField	: TextField		= new TextField();
			for each (allocation in allocations)
			{
				var rect : Rectangle = new Rectangle(
					allocation.offset						* TMP_ALLOC_FIELD_HEIGHT,
					(allocation.beginId - 1)				* TMP_ALLOC_FIELD_WIDTH,
					allocation.size							* TMP_ALLOC_FIELD_HEIGHT,
					(allocation.endId - allocation.beginId)	* TMP_ALLOC_FIELD_WIDTH
				);

				bitmapData.fillRect(rect, Math.random() * 0x505050 + 0x909090);

				textField.text = AbstractOperation(allocation.node).instructionName;
				bitmapData.draw(textField, new Matrix(1, 0, 0, 1, rect.x, rect.y));
			}

			var offsetId : uint;
			for (offsetId = 1; offsetId < maxOffset; offsetId += 1)
				bitmapData.fillRect(new Rectangle(
					offsetId * TMP_ALLOC_FIELD_HEIGHT,
					0,
					1,
					TMP_ALLOC_FIELD_WIDTH * maxOpId
				), 0xBBBBBB);

			for (var operationId : uint = 1; operationId < maxOpId; ++operationId)
				bitmapData.fillRect(new Rectangle(
					0,
					operationId * TMP_ALLOC_FIELD_WIDTH,
					maxOffset * TMP_ALLOC_FIELD_HEIGHT,
					1
				), 0xBBBBBB);

			for (offsetId = 4; offsetId < maxOffset; offsetId += 4)
				bitmapData.fillRect(new Rectangle(
					offsetId * TMP_ALLOC_FIELD_HEIGHT,
					0,
					1,
					TMP_ALLOC_FIELD_WIDTH * maxOpId
				), 0);

			return bitmapData;
		}
	}
}
