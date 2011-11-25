package aerys.minko.scene.node.texture
{
	import aerys.minko.render.RenderTarget;
	
	public class RenderTargetTexture extends Texture
	{
		private var _renderTarget	: RenderTarget	= null;
		
		public function get renderTarget() : RenderTarget
		{
			return _renderTarget;
		}
		
		public function RenderTargetTexture(width 			: int,
											height 			: int,
											backGroundColor	: uint	= 0,
											styleProperty 	: int 	= -1)
		{
			_renderTarget = new RenderTarget(RenderTarget.TEXTURE, width, height, backGroundColor);
			
			super(_renderTarget.textureResource, styleProperty);
		}
	}
}