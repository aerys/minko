package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.IResource;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.textures.TextureBase;
	import flash.utils.ByteArray;

	public interface ITextureResource extends IResource
	{
		function setContentFromBitmapData(bitmapData 	: BitmapData,
										  mipmap 		: Boolean,
										  smooth		: Boolean	= true,
										  downSample	: Boolean	= false) : void;
		
		function setContentFromATF(atf : ByteArray) : void;
		
		function getNativeTexture(context : Context3D) : TextureBase;
	}
}