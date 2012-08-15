package aerys.minko.render.geometry.stream
{
	/**
	 * The StreamUsage enumeration class lists the possible
	 * usage for a stream (VertexStream or IndexStream)
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class StreamUsage
	{
		/**
		 * Indicates the stream can neither be written to nor read from. 
		 */
		public static const STATIC	: uint	= 1;
		/**
		 * Indicates the stream can be written into. 
		 */
		public static const WRITE	: uint	= 2;
		/**
		 * Indicated the strram can be read from. 
		 */
		public static const READ	: uint	= 4;
		
		public static const DYNAMIC	: uint	= READ | WRITE;
	}
}