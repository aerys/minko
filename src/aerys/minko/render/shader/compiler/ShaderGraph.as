package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;

	public class ShaderGraph
	{
		public var outputPosition			: INode;
		public var outputPositionComponents	: uint;
		
		public var outputColor				: INode;
		public var outputColorComponents	: uint;
		
		public var kills					: Vector.<INode>;
		public var killsComponents			: Vector.<uint>;
		
		public var interpolates				: Vector.<INode>;
		
		public function ShaderGraph(outputPosition	: INode,
									outputColor		: INode,
									kills			: Vector.<INode>)
		{
			this.outputPosition				= outputPosition;
			this.outputColor				= outputColor;
			this.kills						= kills;
			
			this.outputPositionComponents	= Components.createContinuous(0, 0, 4, outputPosition.size);
			this.outputColorComponents		= Components.createContinuous(0, 0, 4, outputColor.size);
			
			var numKills : uint = kills.length;
			this.killsComponents= new Vector.<uint>();
			
			for (var killId : uint = 0; killId < numKills; ++killId)
				this.killsComponents[killId] = Components.createContinuous(0, 0, 1, 1);
			
			
		}
	}
}