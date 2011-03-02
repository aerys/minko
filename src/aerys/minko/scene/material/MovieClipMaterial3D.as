package aerys.minko.scene.material
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	
	import flash.display.BitmapData;
	import flash.display.MovieClip;

	public class MovieClipMaterial3D extends AnimatedMaterial3D
	{
		private var _source		: MovieClip		= null;
		
		public function MovieClipMaterial3D(source : MovieClip)
		{
			super();
			
			_source = source;
			_source.gotoAndStop(0);
		}
		
		override protected function nextFrame(query : RenderingQuery) : void
		{
			if (_source)
			{
				addChild(BitmapMaterial3D.fromDisplayObject(_source));
				_source.nextFrame();
				
				if (_source.currentFrame >= _source.totalFrames)
					_source = null;
			}
			
			super.nextFrame(query);
		}
	}
}