package aerys.minko.render.shader.node
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	
	import flash.utils.ByteArray;

	public interface INode
	{
		function get name() : String;
		function get size() : uint;
		
		function accept(v	: IShaderNodeVisitor) : void;
		function isSame(node : INode) : Boolean;
		function toString() : String;
	}
}
