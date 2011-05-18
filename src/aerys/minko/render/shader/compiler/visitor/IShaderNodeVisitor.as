package aerys.minko.render.shader.compiler.visitor
{
	import aerys.minko.render.shader.node.INode;

	public interface IShaderNodeVisitor
	{
		function visit(shaderNode:INode) : void;
	}
}
