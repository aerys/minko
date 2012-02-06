package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.accessor.IParameterAccessor;
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	public class Sampler implements INode
	{
		private var _name		: String;
		private var _accessor	: IParameterAccessor;
		private var _hash		: uint;
		
		private var _filter		: uint;
		private var _mipmap		: uint;
		private var _wrapping	: uint;
		private var _dimension	: uint;
		
		public function get hash() : uint
		{
			return _hash;
		}
		
		public function get size() : uint
		{
			throw new Error('A Sampler has no size');
		}
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get accessor() : IParameterAccessor
		{
			return _accessor;
		}
		
		public function get filter() : uint 
		{ 
			return _filter;
		}
		
		public function get mipmap() : uint
		{
			return _mipmap;	
		}
		
		public function get wrapping() : uint 
		{ 
			return _wrapping;	
		}	
		
		public function get dimension() : uint
		{
			return _dimension;
		}
		
		public function Sampler(name		: String,
								accessor	: IParameterAccessor	= null,
								filter		: uint					= 1, // SamplerFilter.LINEAR
								mipmap		: uint					= 0, // SamplerMipmap.DISABLE
								wrapping	: uint					= 1, // SamplerWrapping.REPEAT
								dimension	: uint					= 0) // SamplerDimension.FLAT
		{
			_name		= name;
			_accessor	= accessor;
			_filter		= filter;
			_mipmap		= mipmap;
			_wrapping	= wrapping;
			_dimension	= dimension;
			
			_hash		= CRC32.computeForString('Sampler' + name + (accessor == null ? '' : accessor.hash.toString())); 
		}
		
		public function toString() : String
		{
			return 'Sampler';
		}
	}
}