package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.IResource;
	
	import flash.display.BitmapData;
	import flash.display3D.textures.TextureBase;
	import flash.utils.ByteArray;

	/**
	 * ITextureResource objects handle texture allocation
	 * and disposal using the Stage3D API.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface ITextureResource extends IResource
	{
		function get width() : uint;
		function get height() : uint;
		
		function setContentFromBitmapData(bitmapData 	: BitmapData,
										  mipmap 		: Boolean,
										  downSample	: Boolean	= false) : void;
		
		function setContentFromATF(atf : ByteArray) : void;
		
		function getNativeTexture(context : Context3DResource) : TextureBase;
		
		function setSize(w : uint, h : uint) : void;
	}
}
