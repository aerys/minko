package aerys.minko.scene.node.group
{
	import aerys.minko.scene.action.StyledAction;
	import aerys.minko.scene.node.IStyled;
	import aerys.minko.scene.visitor.data.Style;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	/**
	 * StyleGroup enables setting style properties that will
	 * be used by all its children nodes.
	 *   
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class StyleGroup extends Group implements IStyled
	{
		private var _style		: Style	= new Style();
		
		public function get style() : Style	{ return _style; }
		
		public function StyleGroup(...children)
		{
			super(children);
			
			actions.unshift(StyledAction.styledAction);
		}
	}
}