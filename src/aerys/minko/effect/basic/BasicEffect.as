package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect;
	
	public class BasicEffect extends AbstractEffect
	{
		public function BasicEffect()
		{
			super();
			
			passes.push(new BasicPass());
		}
	}
}
