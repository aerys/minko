package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.scene.action.style.PopStyleAction;
	import aerys.minko.scene.action.style.PushStyleAction;
	import aerys.minko.scene.node.IStylableScene;
	
	/**
	 * StyleGroup enables setting style properties that will
	 * be used by all its children nodes.
	 *   
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class StyleGroup extends Group implements IStylableScene
	{
		private var _style			: Style		= new Style();
		private var _styleEnabled	: Boolean	= true;
		
		public function get style() 		: Style		{ return _style; }
		public function get styleEnabled()	: Boolean	{ return _styleEnabled; }
		
		public function set style(value : Style) : void
		{
			_style = value;
		}
		
		public function set styleEnabled(value : Boolean) : void
		{
			_styleEnabled = value;
		}
		
		public function StyleGroup(...children)
		{
			super(children);
			
			actions.unshift(PushStyleAction.pushStyleAction);
			actions.push(PopStyleAction.popStyleAction);
		}
	}
}