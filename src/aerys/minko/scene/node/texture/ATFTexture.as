package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
	import flash.utils.ByteArray;
	
	public class ATFTexture extends AbstractScene implements ITexture
	{
		private var _ressource	: TextureResource;
		private var _styleProp	: int;
		
		public function get styleProperty()	: int				{ return _styleProp;	}
		public function get resource()		: TextureResource	{ return _ressource;	}
		public function get version()		: uint				{ return 0;				}
		
		public function ATFTexture(atf 			: ByteArray = null,
								   styleProp	: int		= -1)
		{
			_ressource = new TextureResource();
			_ressource.setContentFromATF(atf);
			
			_styleProp = styleProp != -1 ? styleProp : BasicStyle.DIFFUSE;
			
			actions[0] = TextureAction.textureAction;
		}
		
		
		
	}
}