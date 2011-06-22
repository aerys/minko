package aerys.minko.scene.node.texture
{
	import aerys.minko.scene.action.texture.DynamicTextureAction;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	
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