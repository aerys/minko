package aerys.minko.render.shader.compiler.sequence
{
	import flash.utils.ByteArray;

	public interface IAgalSource
	{
		function getBytecode(destination : ByteArray) : void;
		function getAgal(isVertexShader : Boolean) : String;
	}
}
