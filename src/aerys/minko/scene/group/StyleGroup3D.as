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
		
		override protected function acceptRenderingQuery(q:RenderingQuery):void
		{
			var newStyle:IEffect3DStyle;
			newStyle = _style.override(q.style);
			newStyle = _emptyStyle.override(newStyle);
			q.style = newStyle;
			
			super.acceptRenderingQuery(q);
			
			q.style = q.style.override().override();
			_emptyStyle.clear();
		}
	}
}