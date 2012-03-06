package aerys.minko.type.animation.timeline
{
	import aerys.minko.ns.minko_animation;

	/**
	 * The ColorTimeline animates RGBA or ARGB formatted uint color values.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class ColorTimeline extends AbstractTimeline
	{
		use namespace minko_animation;
		
		minko_animation var _deltaTime		: uint			= 0;
		minko_animation var _values			: Vector.<uint>	= null;
		
		public function ColorTimeline(propertyPath	: String,
									  deltaTime		: uint,
									  values		: Vector.<uint>)
		{
			super(propertyPath, deltaTime * (values.length - 1));
			
			_deltaTime		= deltaTime;
			_values			= values;
		}
		
		override public function updateAt(t:int, target:Object):void
		{
			super.updateAt(time, target);
			
			var time				: uint		= t < 0 ? duration + t : t;
			var previousTimeId		: uint		= Math.floor(time / _deltaTime);
			var nextTimeId			: uint		= Math.ceil(time / _deltaTime);
			var interpolationRatio	: Number	= (time % _deltaTime) / _deltaTime;
			
			if (t < 0)
				interpolationRatio = 1 - interpolationRatio;
			
			var color1		: uint		= _values[previousTimeId];
			var color2		: uint		= _values[nextTimeId];
			var invRatio 	: Number 	= 1 - interpolationRatio;
			
			var r : uint = invRatio * ((color1 >>> 24) & 0xff)
						   + interpolationRatio * ((color2 >>> 24) & 0xff);
			var g : uint = invRatio * ((color1 >>> 16) & 0xff)
						   + interpolationRatio * ((color2 >>> 16) & 0xff);
			var b : uint = invRatio * ((color1 >>> 8) & 0xff)
						   + interpolationRatio * ((color2 >>> 8) & 0xff);
			var a : uint = invRatio * (color1 & 0xff)
						   + interpolationRatio * (color2 & 0xff);
			
			currentTarget[propertyName] = (r << 24) | (g << 16) | (b << 8) | (a & 0xff);
		}
		
		override public function clone():ITimeline
		{
			return new ColorTimeline(propertyPath, _deltaTime, _values.slice());
		}
	}
}