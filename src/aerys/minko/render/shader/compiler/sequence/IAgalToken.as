package aerys.minko.render.shader.compiler.sequence
{
	import flash.utils.ByteArray;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public interface IAgalToken
	{
		function getBytecode(destination : ByteArray) : void;
		function getAgal(isVertexShader : Boolean) : String;
	}
}
