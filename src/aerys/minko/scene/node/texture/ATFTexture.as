package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
	import flash.utils.ByteArray;

	public class ATFTexture extends Texture implements ITexture
	{
		public function ATFTexture(atf 			: ByteArray = null,
								   styleProp	: int		= 0)
		{
			super(null, styleProp);
			
			resource.setContentFromATF(atf);
		}
	}
}