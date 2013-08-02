package aerys.minko.render.resource.texture
{
	import flash.display.BitmapData;
	import flash.display3D.textures.TextureBase;
	import flash.utils.ByteArray;
	
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.IResource;

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
        
        function get format() : uint;
        function get mipMapping() : Boolean;
		
		function setContentFromBitmapData(bitmapData 	: BitmapData,
										  mipmap 		: Boolean,
										  downSample	: Boolean	= false) : void;
		
		function setContentFromATF(atf			: ByteArray) : void;
		
		function getTexture(context : Context3DResource) : TextureBase;
	}
}
