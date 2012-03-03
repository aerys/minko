package aerys.minko.render.shader.compiler.allocation
{
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public interface IAllocation
	{
		function get type()				: uint;
		function get aligned()			: Boolean;
		function get maxSize()			: uint;
		function get registerId()		: uint;
		function get registerOffset()	: uint;
//		function get writeMask()		: uint;
		
		function set offset(v : uint) : void;
		
		function getReadSwizzle(readingOpWriteOffset	: uint,
								readingOpComponents		: uint) : uint;
		
		function overlapsWith(other : IAllocation, readOnly : Boolean) : Boolean;
		
		/**
		 * Report that an allocation will need to be accessed at a given operationId
		 * @param operationId The operationId of the read.
		 */
		function extendLifeTime(operationId : uint) : void;
	}
}
