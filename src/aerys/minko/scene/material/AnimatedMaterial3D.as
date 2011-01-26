package aerys.minko.scene.material
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.scene.group.Group3D;
	
	import flash.display.BitmapData;
	
	public class AnimatedMaterial3D extends Group3D implements IMaterial3D
	{
		private var _frame		: int	= 0;
		private var _lastFrame	: uint	= 0;
		
		public function AnimatedMaterial3D(...children)
		{
			super(children);
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			var q : RenderingQuery = query as RenderingQuery;
			
			if (q && _lastFrame != q.frameId)
			{
				_lastFrame = q.frameId;
				nextFrame(q);
			}
		}
		
		protected function nextFrame(query : RenderingQuery) : void
		{	
			query.query(children[int(_frame++ % numChildren)]);
		}
	}
}