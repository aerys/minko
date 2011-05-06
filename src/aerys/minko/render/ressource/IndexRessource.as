package aerys.minko.render.ressource
{
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;

	public class IndexRessource implements IRessource
	{
		public function IndexRessource()
		{
		}
		
		public function getNativeBuffer(context : Context3D) : IndexBuffer3D
		{
			return null;
		}
		
		public function dispose():Boolean
		{
			return false;
		}
	}
}
