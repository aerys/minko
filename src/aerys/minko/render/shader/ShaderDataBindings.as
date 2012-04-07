package aerys.minko.render.shader
{
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.type.data.DataBindings;

	/**
	 * The wrapper used to expose scene/mesh data bindings in ActionScript shaders.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class ShaderDataBindings
	{
		private var _dataBindings	: DataBindings	= null;
		private var _signature		: Signature		= null;
		private var _signatureFlags	: uint			= 0;
		
		public function ShaderDataBindings(bindings			: DataBindings,
										   signature		: Signature,
										   signatureFlags	: uint)
		{
			_dataBindings = bindings;
			_signature = signature;
			_signatureFlags = signatureFlags;
		}
		
		public function getParameter(name : String, size : uint) : SFloat
		{
			return new SFloat(new BindableConstant(name, size));
		}
		
		public function getTextureParameter(bindingName	: String,
											filter		: uint = 1,
											mipmap		: uint = 0,
											wrapping	: uint = 1,
											dimension	: uint = 0) : SFloat
		{
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
						"The property '" + propertyName
						+ "' does not exist and no default value was provided."
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