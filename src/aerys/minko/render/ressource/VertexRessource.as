package aerys.minko.render.ressource
{
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;

	public class VertexRessource implements IRessource3D
	{
		public function VertexRessource()
		{
		}
		
		public function getNativeBuffer(context : Context3D) : VertexBuffer3D
		{
			return null;
		}
		
		public function dispose():Boolean
		{
			return false;
		}
	}
}