package aerys.minko.scene.graph.texture
{
	import aerys.minko.scene.graph.texture.BitmapTexture;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	
	public class DynamicTexture extends BitmapTexture
	{
		private var _source		: DisplayObject	= null;
		private var _bitmapData	: BitmapData	= null;
		private var _lastFrame	: uint			= uint(-1);
		
		public function DynamicTexture(source 		: DisplayObject,
										  mipmapping	: Boolean	= true)
		{
			_source = source;
			_bitmapData ||= new BitmapData(_source.width, _source.height);
			
			super(null, mipmapping);
		}
		
		override protected function visitedByRenderingVisitor(visitor : RenderingVisitor) : void
		{
			if (visitor.frameId != _lastFrame)
			{
				_lastFrame = visitor.frameId;
				_bitmapData.draw(_source);
				updateFromBitmapData(_bitmapData, false);
			}
			
			super.visitedByRenderingVisitor(visitor);
		}
	}
}