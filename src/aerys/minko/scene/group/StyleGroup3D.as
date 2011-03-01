package aerys.minko.scene.group
{
	import aerys.minko.effect.Effect3DStyle;
	import aerys.minko.effect.IEffect3DStyle;
	import aerys.minko.effect.IStyled3D;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	
	public class StyleGroup3D extends Group3D implements IStyled3D
	{
		private var _emptyStyle	: IEffect3DStyle	= new Effect3DStyle();
		private var _style		: IEffect3DStyle	= new Effect3DStyle();
		
		public function StyleGroup3D(...children)
		{
			super(children);
		}
		
		public function get style() : IEffect3DStyle
		{
			return _style;
		}
		
		override protected function acceptRenderingQuery(query:RenderingQuery):void
		{
			_style.override(query.style);
			_emptyStyle.override(_style);
			query.style = style;
			
			super.acceptRenderingQuery(query);
			
			query.style = _style.override().override();
			_emptyStyle.clear();
		}
	}
}