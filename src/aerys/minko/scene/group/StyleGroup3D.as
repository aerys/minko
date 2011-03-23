package aerys.minko.scene.group
{
	import aerys.minko.effect.IStyled3D;
	import aerys.minko.effect.Style3D;
	import aerys.minko.query.rendering.RenderingQuery;
	
	/**
	 * StyleGroup3D enables setting style properties that will
	 * be used by all its children nodes.
	 *   
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class StyleGroup3D extends Group3D implements IStyled3D
	{
		private var _style		: Style3D	= new Style3D();
		
		public function StyleGroup3D(...children)
		{
			super(children);
		}
		
		public function get style() : Style3D
		{
			return _style;
		}
		
		override protected function acceptRenderingQuery(q:RenderingQuery):void
		{
			q.style.push(_style);
			
			super.acceptRenderingQuery(q);
			
			q.style.pop();
		}
	}
}