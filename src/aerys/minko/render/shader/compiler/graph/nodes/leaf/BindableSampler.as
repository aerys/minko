package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class BindableSampler extends AbstractSampler
	{
		private var _bindingName : String;
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function BindableSampler(bindingName	: String,
										filter		: uint = 1, // SamplerFilter.LINEAR
										mipmap		: uint = 0, // SamplerMipmap.DISABLE
										wrapping	: uint = 1, // SamplerWrapping.REPEAT
										dimension	: uint = 0) // SamplerDimension.FLAT
		{
			var hash : uint = CRC32.computeForString('ParametrizedSampler' + bindingName);
			
			super(hash, filter, mipmap, wrapping, dimension);
			
			_bindingName = bindingName;
		}
		
		override public function toString() : String
		{
			return 'BindedSampler';
		}
	}
}
