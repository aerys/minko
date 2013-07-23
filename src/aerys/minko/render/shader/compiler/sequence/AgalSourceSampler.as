package aerys.minko.render.shader.compiler.sequence
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerFormat;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	import flash.utils.ByteArray;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class AgalSourceSampler implements IAgalToken
	{
		private var _index		: uint;
        private var _format     : uint;      
		private var _dimension	: uint;
		private var _wrapping	: uint;
		private var _filter		: uint;
		private var _mipmap		: uint;
		
		public function get index() : uint
        {
            return _index;
        }
        
		public function get dimension()	: uint
        {
            return _dimension;
        }

		public function get format() : uint
		{
			return _format;
		}
        
		public function get wrapping() : uint
        {
            return _wrapping;
        }
        
		public function get filter() : uint
        {
            return _filter;
        }
        
		public function get mipmap() : uint
        {
            return _mipmap;
        }
		
		public function AgalSourceSampler(index		: uint,
                                          format    : uint,
                                          dimension	: uint,
                                          wrapping	: uint,
                                          filter	: uint,
                                          mipmap	: uint)
		{
			_index		= index;
            _format     = format;
			_dimension	= dimension;
			_wrapping	= wrapping;
			_filter		= filter;
			_mipmap		= mipmap;
		}
		
		public function getBytecode(destination : ByteArray) : void
		{
			destination.writeShort(index);					    //	NNNNNNNNNNNNNNNN = Sampler index (16 bits)
			destination.writeShort(0x0);					    //	---------------- = Nothing
			destination.writeByte(RegisterType.SAMPLER);    	//	----TTTT = Register type, must be 5, Sampler
			destination.writeByte((dimension << 4) | _format);	//	DDDDFFFF = Dimension (0=2D,1=Cube), Format (0=RGBA,1=DXT1,2=DXT5)
			destination.writeByte(wrapping << 4);			    //	WWWWSSSS = Wrapping (0=clamp,1=repeat), Special flag bits (must be 0)
			destination.writeByte((filter << 4) | _mipmap);	    //	FFFFMMMM = Filter (0=nearest,1=linear) (4bits), Mipmap (0=disable,1=nearest,2=linear)
		}
		
		public function getAgal(isVertexShader : Boolean) : String
		{
			var agal : String  = SamplerFiltering.minko_shader::STRINGS[filter] + ','
								+ SamplerMipMapping.minko_shader::STRINGS[mipmap] + ','
								+ SamplerWrapping.minko_shader::STRINGS[wrapping] + ','
								+ SamplerDimension.minko_shader::STRINGS[dimension];
			var atf : String = SamplerFormat.RGBA == format ? '' :
					',' + SamplerFormat.minko_shader::STRINGS_DESKTOP[format];

			return 'fs' + index + ' <' + agal + atf + '>';
		}
	}
}