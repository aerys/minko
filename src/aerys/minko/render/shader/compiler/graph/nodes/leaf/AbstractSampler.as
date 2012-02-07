package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	public class AbstractSampler implements INode
	{
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
		
		public function AbstractSampler(hash			: uint,
										filter			: uint = 1, // SamplerFilter.LINEAR
										mipmap			: uint = 0, // SamplerMipmap.DISABLE
										wrapping		: uint = 1, // SamplerWrapping.REPEAT
										dimension		: uint = 0)
		{
			_hash		= hash;
			_filter		= filter;
			_mipmap		= mipmap;
			_wrapping	= wrapping;
			_dimension	= dimension;
		}
		
		public function toString() : String
		{
			throw new Error('This method must be overriden.');
		}
	}
}