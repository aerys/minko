package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	
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
			super(filter, mipmap, wrapping, dimension);
			
			_bindingName = bindingName;
		}
		
		override protected function computeHash() : uint
		{
			return CRC32.computeForString('ParametrizedSampler' + _bindingName);
		}
		
		override public function toString() : String
		{
			return 'BindedSampler';
		}
		
		override public function clone() : AbstractNode
		{
			return new BindableSampler(_bindingName, filter, mipmap, wrapping, dimension);
		}
	}
}
