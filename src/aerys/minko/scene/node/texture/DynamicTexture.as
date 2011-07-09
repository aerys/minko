package aerys.minko.scene.node.texture
{
	import aerys.minko.scene.action.texture.DynamicTextureAction;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	
	/**
	 * DynamicTexture objects make it possible to use any DisplayObject
	 * object as a dynamic animated texture.
	 * 
	 * <p>
	 * Everytime the texture is traversed during rendering, it will try
	 * to rasterize the source DisplayObject object and upload it has the
	 * new texture data. Texture data will be uploaded only once per
	 * rendering. Therefore, if you use the same DynamicTexture object
	 * multiple times, the rasterizing and the upload of the bitmap data
	 * will be done only once.
	 * </p>
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class DynamicTexture extends BitmapTexture
	{
		private var _source		: DisplayObject	= null;
		
		public function get source() : DisplayObject	{ return _source; }
		
		public function DynamicTexture(source 		: DisplayObject,
									   mipmapping	: Boolean	= true)
		{
			super(null, mipmapping);
			
			_source = source;
			
			actions.unshift(new DynamicTextureAction());
		}
	}
}