package aerys.minko.render.target
{
	import flash.display3D.Context3D;

	public class BackBufferRenderTarget extends AbstractRenderTarget
	{
		public function BackBufferRenderTarget(width				: uint, 
											   height				: uint, 
											   backgroundColor		: uint		= 0, 
											   useDepthAndStencil	: Boolean	= true, 
											   antiAliasing			: int		= 0)
		{
			super(width, height, backgroundColor, useDepthAndStencil, antiAliasing);
		}
		
		override public function configureContext(context : Context3D) : void
		{
			context.setRenderToBackBuffer();
		}
	}
}
