package aerys.minko.render.shader.compiler.graph.visitors.tofinish
{
	
	import flash.utils.Dictionary;

	/**
	 * This is a modified version of the Sethi-Ullman algorithm, which was done to work on CPU (scalar) registers.
	 * 
	 * This does not take into account 
	 * - memory "lost" because registers (sometimes) need to be aligned.
	 * - this is optimal for a tree, not so much for a DAG (and we use DAGs)
	 * 
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */	
	public class SethiUllmann
	{
		/*
		public function work(outputPosition : INode,
							 interpolates	: Vector.<INode>,
							 ouputColor		: INode,
							 kills			: Vector.<INode>) : Dictionary
		{
			var tmpSpaceToCompute : Dictionary = new Dictionary();
			
			visit(outputPosition, tmpSpaceToCompute);
			for each (var interpolate : INode in interpolates)
				visit(Interpolate(interpolate).arg1, tmpSpaceToCompute);
			
			visit(ouputColor, tmpSpaceToCompute);
			for each (var kill : INode in kills)
				visit(kill, tmpSpaceToCompute);
				
			return tmpSpaceToCompute;
		}
		
		public function visit(node				: INode,
							  neededTmpSpace	: Dictionary) : uint
		{
			if (neededTmpSpace[node] != undefined)
				return uint.MAX_VALUE;
			
			else if (node is AbstractConstant || node is Attribute || node is Sampler)
				return 0;
			
			else if (node is Interpolate)
				return 0;
			
			else if (node is AbstractOperation)
			{
				var opNode							: AbstractOperation	= AbstractOperation(node);
				
				if (opNode.arg2 == null)
				{
					var spaceToComputeArg	: uint = visit(opNode.arg1, neededTmpSpace);
					var spaceToCompute		: uint = Math.max(spaceToComputeArg, 0 + opNode.arg1.size);
					neededTmpSpace[node] = spaceToCompute;
				}
				else
				{
					var spaceToComputeLeft				: uint				= visit(opNode.arg1, neededTmpSpace);
					var spaceToComputeRight				: uint				= visit(opNode.arg2, neededTmpSpace);
					
					var spaceToStoreLeft				: uint				= opNode.arg1.size;
					var spaceToStoreRight				: uint				= opNode.arg2.size;
					
					var spaceToComputeIfWeStartByLeft	: uint				= Math.max(spaceToComputeLeft, spaceToComputeRight + spaceToStoreLeft);
					var spaceToComputeIfWeStartByRight	: uint				= Math.max(spaceToComputeRight, spaceToComputeLeft + spaceToStoreRight);
					
					neededTmpSpace[node] = Math.min(spaceToComputeIfWeStartByLeft, spaceToComputeIfWeStartByRight);
				}
				
				return neededTmpSpace[node];
			}
			
			throw new Error();
		}
		*/
	}
}
