package aerys.minko.render.shader.compiler.sequence
{
	import flash.utils.ByteArray;

	public class AgalSourceEmpty implements IAgalSource
	{
		public function AgalSourceEmpty()
		{
		}
		
		public function getBytecode(destination : ByteArray) : void
		{
			destination.writeUnsignedInt(0);
			destination.writeUnsignedInt(0);
		}
		
		public function getAgal(isVertexShader : Boolean) : String
		{
			return '';
		}
	}
}
