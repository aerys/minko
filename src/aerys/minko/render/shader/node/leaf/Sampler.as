package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	
	public class Sampler extends AbstractLeaf implements IFragmentNode
	{
		public static const FILTER_NEAREST	: uint = 0;
		public static const FILTER_LINEAR	: uint = 1;
		
		public static const MIPMAP_DISABLE	: uint = 0;
		public static const MIPMAP_NEAREST	: uint = 1;
		public static const MIPMAP_LINEAR	: uint = 2;
		
		public static const WRAPPING_CLAMP	: uint = 0;
		public static const WRAPPING_REPEAT	: uint = 1;
		
		public static const DIMENSION_2D	: uint = 0;
		public static const DIMENSION_CUBE	: uint = 1;
		
		protected var _id			: uint;
		protected var _styleName	: String;
		protected var _filter		: uint;
		protected var _mipmap		: uint;
		protected var _wrapping		: uint;
		protected var _dimension	: uint;
		
		override public function get size() : uint
		{
			return 0;
		}
		
		public function get styleName():String
		{
			return _styleName;
		}

		public function get filter():uint
		{
			return _filter;
		}

		public function get mipmap():uint
		{
			return _mipmap;
		}

		public function get wrapping():uint
		{
			return _wrapping;
		}

		public function get dimension():uint
		{
			return _dimension;
		}

		public function get samplerId() : uint
		{ 
			return _id; 
		}
		
		public function set samplerId(value : uint) : void 
		{ 
			_id = value; 
		}
		
		public function Sampler(styleName	: String,
								filter		: uint	= FILTER_LINEAR,
								mipmap		: uint	= MIPMAP_DISABLE, 
								wrapping	: uint	= WRAPPING_REPEAT,
								dimension	: uint	= DIMENSION_2D)
		{
			super();
			
			_styleName	= styleName;
			_filter		= filter;
			_mipmap		= mipmap;
			_wrapping	= wrapping;
			_dimension	= dimension;
		}
		
		override public function isSame(otherNode : INode) : Boolean
		{
			var samplerOtherNode : Sampler = otherNode as Sampler;
			if (samplerOtherNode == null)
				return false;
			
			return _styleName	== samplerOtherNode._styleName
				&& _filter		== samplerOtherNode._filter
				&& _mipmap		== samplerOtherNode._mipmap
				&& _wrapping	== samplerOtherNode._wrapping
				&& _dimension	== samplerOtherNode._dimension
		}
		
		override public function toString() : String
		{
			return "Sampler\\nname=" + styleName;
		}
	}
}