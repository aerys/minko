package aerys.minko.render.shader.compiler.graph.nodes
{
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public interface INode
	{
		function get hash() : uint;
		function get size() : uint;
		
		function toString() : String;
	}
}