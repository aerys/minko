package mx.core
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	
	public class BitmapAsset extends Bitmap
	{
		public function BitmapAsset(bitmapData:BitmapData=null, pixelSnapping:String="auto", smoothing:Boolean=false)
		{
			super(bitmapData, pixelSnapping, smoothing);
		}
	}
}