package aerys.minko.scene.graph.group
{
	import aerys.minko.scene.graph.IStyled;
	import aerys.minko.scene.visitor.data.Style;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	/**
	 * StyleGroup3D enables setting style properties that will
	 * be used by all its children nodes.
	 *   
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class StyleGroup extends Group implements IStyled
	{
		private var _style		: Style	= new Style();
		
		public function StyleGroup(...children)
		{
			super(children);
		}
		
		public function get style() : Style
		{
			return _style;
		}
		
//		override protected function visitedByRenderingVisitor(q:RenderingVisitor):void
//		{
//			q.style.push(_style);
//			
//			super.visitedByRenderingVisitor(q);
//			
//			q.style.pop();
//		}
	}
}