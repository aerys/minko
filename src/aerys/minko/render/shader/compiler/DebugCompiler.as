package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.allocator.Allocation;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteAgal;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteDot;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	
	import flash.display.BitmapData;
	import flash.geom.Matrix;
	import flash.geom.Rectangle;
	import flash.text.TextField;

	public class DebugCompiler extends Compiler
	{
		internal static const FIELD_WIDTH	: uint = 20;
		internal static const FIELD_HEIGHT	: uint = 25;
		
		public function DebugCompiler()
		{
			super();
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
		
		public function writeConstantAllocationSummary(vertexShader : Boolean	= true) : String
		{
			var offsetToLetter		: Vector.<String> = Vector.<String>(['x', 'y', 'z', 'w']);
			var summary				: String = '';
			
			var allocationTable 	: Vector.<ParameterAllocation>	= vertexShader ? _vsParams : _fsParams;
			var constantTable		: Vector.<Number>				= vertexShader ? _vsConstData : _fsConstData;
			
			var occupiedRegisters	: uint = constantTable.length / 4;
			for (var registerId : uint = 0; registerId < occupiedRegisters; ++registerId)
				for (var offsetId : uint = 0; offsetId < 4; ++offsetId)
				{
					var foundInParameterTable : Boolean = false;
					
					for each (var allocation : ParameterAllocation in allocationTable)
					{
						if (allocation.offset == 4 * registerId + offsetId)
						{
							foundInParameterTable = true;
							
							summary += (vertexShader ? "vc" : "fc");
							if (allocation.size < 4)
							{
								summary += registerId + '.';
								for (var i : uint = allocation.offset % 4; i < allocation.offset % 4 + allocation.size && i < 4; ++i)
									summary += offsetToLetter[i];
							}
							else if (allocation.size == 16)
							{
								summary += '[' + registerId + '-' + (registerId + 3) + ']';
							}
							
							var parameter : AbstractParameter = allocation.parameter;
							if (parameter is StyleParameter)
							{
								var styleParam : StyleParameter = parameter as StyleParameter;
								summary += "\t\tStyleParameter['" + styleParam.key + "']\n";
							}
							else if (parameter is WorldParameter)
							{
								var worldParam : WorldParameter = parameter as WorldParameter;
								summary += "\t\tWorldParameter[class='" 
									+ worldParam.className + "', index='" 
									+ worldParam.index + "', field='" 
									+ worldParam.field + "']\n";
							}
							else if (parameter is TransformParameter)
							{
								var transformParam : TransformParameter = parameter as TransformParameter;
								summary += "\t\tTransformParameter[key='" 
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
						summary += (vertexShader ? "vc" : "fc") + registerId.toString()
							+ '.' + offsetToLetter[offsetId]
							+ "\t\tConstant[value='" + constantTable[4 * registerId + offsetId] + "']\n";
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
			var bitmapData	: BitmapData	= new BitmapData(FIELD_HEIGHT * maxOffset, FIELD_WIDTH * maxOpId, false, 0xFFFFFF);
			
			var textField	: TextField		= new TextField();
			for each (allocation in allocations)
			{
				var rect : Rectangle = new Rectangle(
					allocation.offset						* FIELD_HEIGHT,
					(allocation.beginId - 1)				* FIELD_WIDTH,
					allocation.size							* FIELD_HEIGHT,
					(allocation.endId - allocation.beginId)	* FIELD_WIDTH
				);
				
				bitmapData.fillRect(rect, Math.random() * 0x505050 + 0x909090);
				
				textField.text = AbstractOperation(allocation.node).instructionName;
				bitmapData.draw(textField, new Matrix(1, 0, 0, 1, rect.x, rect.y));
			}
			
			var offsetId : uint;
			for (offsetId = 1; offsetId < maxOffset; offsetId += 1)
				bitmapData.fillRect(new Rectangle(
					offsetId * FIELD_HEIGHT, 
					0, 
					1,
					FIELD_WIDTH * maxOpId
				), 0xBBBBBB);
			
			for (var operationId : uint = 1; operationId < maxOpId; ++operationId)
				bitmapData.fillRect(new Rectangle(
					0, 
					operationId * FIELD_WIDTH, 
					maxOffset * FIELD_HEIGHT,
					1
				), 0xBBBBBB);
				
			for (offsetId = 4; offsetId < maxOffset; offsetId += 4)
				bitmapData.fillRect(new Rectangle(
					offsetId * FIELD_HEIGHT, 
					0, 
					1,
					FIELD_WIDTH * maxOpId 
				), 0);
			
			return bitmapData;
		}
		
	}
}