package aerys.minko.render.shader
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.type.binding.DataBindings;

	/**
	 * The wrapper used to expose scene/mesh data bindings in ActionScript shaders.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class ShaderDataBindings
	{
		private var _dataBindings	: DataBindings;
		private var _signature		: Signature;
		private var _signatureFlags	: uint;
		
		private var _serializer		: Serializer;
		
		public function ShaderDataBindings(bindings			: DataBindings,
										   signature		: Signature,
										   signatureFlags	: uint)
		{
			_dataBindings = bindings;
			_signature = signature;
			_signatureFlags = signatureFlags;
			
			_serializer = new Serializer();
		}
		
		public function getParameter(name			: String,
									 size			: uint,
									 defaultValue	: Object = null) : SFloat
		{
			if (defaultValue != null && !propertyExists(name))
			{
				var constantValue : Vector.<Number> = new Vector.<Number>();
				
				_serializer.serializeKnownLength(defaultValue, constantValue, 0, size);
				
				return new SFloat(new Constant(constantValue));
			}
			
			return new SFloat(new BindableConstant(name, size));
		}
		
		public function getTextureParameter(bindingName		: String,
											filter			: uint				= 1,
											mipmap			: uint				= 0,
											wrapping		: uint				= 1,
											dimension		: uint				= 0,
											defaultValue	: TextureResource	= null) : SFloat
		{
			if (defaultValue != null && !propertyExists(bindingName))
				return new SFloat(new Sampler(defaultValue, filter, mipmap, wrapping, dimension));
			
			return new SFloat(
				new BindableSampler(bindingName, filter, mipmap, wrapping, dimension)
			);
		}
		
		public function propertyExists(propertyName : String) : Boolean
		{
			var value 	: Boolean	= _dataBindings.propertyExists(propertyName);
			
			_signature.update(
				propertyName,
				value,
				Signature.OPERATION_EXISTS | _signatureFlags
			);
			
			return value;
		}
		
		public function getConstant(propertyName : String,
									defaultValue : Object	= null) : *
		{
			if (!_dataBindings.propertyExists(propertyName))
			{
				if (defaultValue === null)
				{
					throw new Error(
						'The property \'' + propertyName
						+ '\' does not exist and no default value was provided.'
					);
				}
				
				_signature.update(
					propertyName,
					false,
					Signature.OPERATION_EXISTS | _signatureFlags
				);
				
				return defaultValue;
			}
			
			var value : Object = _dataBindings.getProperty(propertyName);

			_signature.update(
				propertyName,
				value,
				Signature.OPERATION_GET | _signatureFlags
			);
			
			return value;
		}
	}
}