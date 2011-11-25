package aerys.minko.scene.node.texture
{
	import flash.utils.ByteArray;

	public class ATFTexture extends Texture implements ITexture
	{
		public function ATFTexture(atf 				: ByteArray = null,
								   styleProperty	: int		= -1)
		{
			super(null, styleProperty);
			
			resource.setContentFromATF(atf);
		}
	}
}