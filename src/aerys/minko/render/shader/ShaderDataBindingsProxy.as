package aerys.minko.render.shader
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.Signature;
	import aerys.minko.render.DataBindingsProxy;

	/**
	 * The wrapper used to expose scene/mesh data bindings in ActionScript shaders.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class ShaderDataBindingsProxy extends DataBindingsProxy
	{
		public function ShaderDataBindingsProxy(bindings		: DataBindings,
												signature		: Signature,
												signatureFlags	: uint)
		{
			super(bindings, signature, signatureFlags);
		}
		
		public function getParameter(name			: String,
									 size			: uint,
									 defaultValue	: Object = null) : SFloat
		{
			if (defaultValue != null && !propertyExists(name))
			{
				var constantValue 	: Vector.<Number> 	= new Vector.<Number>();
				var serializer 		: Serializer		= new Serializer();
				
				serializer.serializeKnownLength(defaultValue, constantValue, 0, size);
				
				return new SFloat(new Constant(constantValue));
			}
			
			return new SFloat(new BindableConstant(name, size));
		}
		
		public function getTextureParameter(bindingName		: String,
											filter			: uint				= 1,
											mipmap			: uint				= 0,
											wrapping		: uint				= 1,
											dimension		: uint				= 0,
											format          : uint              = 0,
											defaultValue	: TextureResource	= null) : SFloat
		{
			if (defaultValue != null && !propertyExists(bindingName))
				return new SFloat(new Sampler(defaultValue, filter, mipmap, wrapping, dimension, format));
			
			return new SFloat(
				new BindableSampler(bindingName, filter, mipmap, wrapping, dimension, format)
			);
		}
	}
}