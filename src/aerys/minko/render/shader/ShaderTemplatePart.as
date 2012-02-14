package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.AbstractSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFilter;
	import aerys.minko.type.enum.SamplerMipmap;
	import aerys.minko.type.enum.SamplerWrapping;
	import aerys.minko.type.stream.format.VertexComponent;

	public class ShaderTemplatePart
	{
		use namespace minko_shader;
		
		private var _main	: ShaderTemplate	= null;
		
		protected final function get main() : ShaderTemplate
		{
			return _main;
		}

		protected function get vertexId() : SValue
		{
			return new SValue(new Attribute(VertexComponent.ID));
		}
		
		/**
		 * The position of the current vertex in local space.
		 * @return
		 *
		 */
		protected function get vertexXYZ() : SValue
		{
			return new SValue(new Attribute(VertexComponent.XYZ));
		}
		
		protected function get vertexXY() : SValue
		{
			return new SValue(new Attribute(VertexComponent.XY));
		}

		/**
		 * The RGB color of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexRGBColor() : SValue
		{
			return new SValue(new Attribute(VertexComponent.RGB));
		}

		/**
		 * The RGBA color of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexRGBAColor() : SValue
		{
			return new SValue(new Attribute(VertexComponent.RGBA));
		}

		/**
		 * The UV texture coordinates of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexUV() : SValue
		{
			return new SValue(new Attribute(VertexComponent.UV));
		}

		/**
		 * The normal of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexNormal() : SValue
		{
			return new SValue(new Attribute(VertexComponent.NORMAL));
		}

		/**
		 * The tangent of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexTangent() : SValue
		{
			return new SValue(new Attribute(VertexComponent.TANGENT));
		}
		
		protected function get localToWorld() : SValue
		{
			return getParameter("local to world", 16);
		}
		
		protected function get worldToView() : SValue
		{
			return getParameter("world to view", 16);
		}
		
		protected function get worldToScreen() : SValue
		{
			return getParameter("world to screen", 16);
		}
		
		protected function get projection() : SValue
		{
			return getParameter("projection", 16);
		}

		public function ShaderTemplatePart(main : ShaderTemplate) : void
		{
			_main = main;
		}
		
		/**
		 * Interpolate vertex shader values to make them usable inside the
		 * fragment shader.
		 *
		 * <p>Any SValue object comming from the vertex shader but used inside
		 * the fragment shader should be interpolated first. This is what the
		 * "interpolate" method does.</p>
		 *
		 * <p>The interpolate method will make it possible for the shader
		 * compiler to resolve varying registers allocations.</p>
		 *
		 * @param value
		 * @return
		 *
		 */
		protected final function interpolate(value : SValue) : SValue
		{
			return new SValue(new Interpolate(getNode(value)));
		}

		/**
		 * Create a new value by combining others.
		 *
		 * <p>The resulting value will be made out off all the arguments
		 * put together one after the other. The size of the final value
		 * will be the sum of the sizes of the combined values.</p>
		 *
		 * <p>For example, combining an RGB color (1.0, 0.0, 1.0) with
		 * a scalar (1.0) will return a value of size 4 that could be
		 * used as an RGBA value:</p>
		 *
		 * <pre>
		 * override protected function getOutputColor() : SValue
		 * {
		 * 	// interpolate the vertex RGB color
		 * 	var color : SValue = interpolate(vertexRGBColor);
		 *
		 * 	// combine the RGB color with a constant alpha = 1.0
		 * 	color = combine(color, 1.0);
		 *
		 * 	return color;
		 * }
		 * </pre>
		 *
		 * <p>This fragment shader function will output the vertex color
		 * with a constant alpha value equal to 0.5.</p>
		 *
		 * @param value1
		 * @param value2
		 * @param values
		 * @return
		 *
		 */
		private function toFloat(size : int, values : Array) : SValue
		{
			var currentOffset	: uint = 0;
			
			var args			: Vector.<INode>	= new Vector.<INode>();
			var components		: Vector.<uint>		= new Vector.<uint>();
			
			for each (var value : Object in values)
			{
				var node		: INode	= getNode(value);
				var nodeSize	: uint	= node.size;
				
				if (currentOffset + nodeSize > size)
					throw new Error('Invalid size specified: buffer is too big');
				
				args.push(node);
				components.push(Components.createContinuous(currentOffset, 0, nodeSize, nodeSize));
				currentOffset += nodeSize;
			}
			
			if (currentOffset != size)
				throw new Error('Invalid size specified. Expected ' + size + ' got ' + currentOffset + '.');
			
			return new SValue(new Overwriter(args, components));
		}

		protected final function float(x : Object) : SValue
		{
			var node : INode = getNode(x);
			if (node.size == 1)
				return new SValue(node);
			else
				throw new Error('Invalid argument');
		}

		protected final function float2(x : Object, y : Object = null) : SValue
		{
			if (x != null && y == null)
				return toFloat(2, [x]);
			else if (x != null && y != null)
				return toFloat(2, [x, y]);
			else
				throw new Error('Invalid arguments');
		}

		/**
		 * Create a new SValue object of size 3 by combining 3 scalar values.
		 *
		 * <p>This method is an alias of the "combine" method. You should prefer this
		 * method everytime you want to build an SValue object of size 3 because this
		 * method will actually enforce it and "combine" will not.</p>
		 *
		 * @param x
		 * @param y
		 * @param z
		 * @return
		 *
		 */
		protected final function float3(x : Object,
										y : Object = null,
										z : Object = null) : SValue
		{
			if (x != null && y == null && z == null)
				return toFloat(3, [x]);
			else if (x != null && y != null && z == null)
				return toFloat(3, [x, y]);
			else if (x != null && y != null && z != null)
				return toFloat(3, [x, y, z]);
			else
				throw new Error('Invalid arguments');
		}

		/**
		 * Create a new SVAlue object of size 4 by combining 4 scalar values.
		 *
		 * <p>This method is an alias of the "combine" method. You should prefer this
		 * method everytime you want to build an SValue object of size 4 because
		 * this method will actually enforce it and "combine" will not.</p>
		 *
		 * @param x
		 * @param y
		 * @param z
		 * @param w
		 * @return
		 *
		 */
		protected final function float4(x : Object,
										y : Object = null,
										z : Object = null,
										w : Object = null) : SValue
		{
			if (x != null && y == null && z == null && w == null)
				return toFloat(4, [x]);
			else if (x != null && y != null && z == null && w == null)
				return toFloat(4, [x, y]);
			else if (x != null && y != null && z != null && w == null)
				return toFloat(4, [x, y, z]);
			else if (x != null && y != null && z != null && w != null)
				return toFloat(4, [x, y, z, w]);
			else
				throw new Error('Invalid arguments');
		}
		
		/**
		 * Pack a [0 .. 1] scalar value into a float4 RGBA color value. 
		 * @param scalar
		 * @return 
		 * 
		 */
		protected final function pack(scalar : Object) : SValue
		{
			var bitSh	: SValue = float4(256. * 256. * 256., 256. * 256, 256., 1.);
			var bitMsk	: SValue = float4(0., 1. / 256., 1. / 256., 1. / 256.);
			var comp	: SValue = fractional(multiply(getNode(scalar), bitSh));
			
			return subtract(comp, multiply(comp.xxyz, bitMsk));
		}
		
		/**
		 * Unack a [0 .. 1] scalar value from a float4 RGBA color value. 
		 * @param packedScalar
		 * @return 
		 * 
		 */
		protected final function unpack(packedScalar : Object) : SValue
		{
			var bitSh : SValue = float4(1. / (256. * 256. * 256.), 1. / (256. * 256.), 1. / 256., 1.);
			
			return dotProduct4(packedScalar, bitSh);
		}

		/**
		 * Retrieve the RGBA color of a pixel of a texture.
		 *
		 * @param styleId The style id of the texture to sample.
		 * @param uv The UV texture coordinates to sample.
		 * @param filtering The texture filtering to use. The value must be one of the following constants:
		 * <ul>
		 * <li>Sampler.FILTER_NEAREST</li>
		 * <li>FILTER_LINEAR</li>
		 * </ul>
		 * @param mipMapping The mip mapping to use. The value must be one of the following constants:
		 * <ul>
		 * <li>Sampler.MIPMAP_DISABLE</li>
		 * <li>Sampler.MIPMAP_NEAREST</li>
		 * <li>Sampler.MIPMAP_LINEAR</li>
		 * </ul>
		 * @param wrapping The texture wrapping to use. The value must be one of the following constants:
		 * <ul>
		 * <li>Sampler.WRAPPING_CLAMP</li>
		 * <li>Sampler.WRAPPING_REPEAT</li>
		 * </ul>
		 * @return
		 *
		 */
		protected final function sampleTexture(texture	: SValue,
											   uv 		: Object) : SValue
		{
			
			
			return new SValue(new Instruction(Instruction.TEX, getNode(uv), getNode(texture)));
		}

		/**
		 * Compute term-to-term scalar multiplication.
		 *
		 * @param arg1
		 * @param arg2
		 * @param args
		 * @return
		 *
		 */
		protected final function multiply(value1 : Object, value2 : Object, ...args) : SValue
		{
			var result : SValue = new SValue(new Instruction(Instruction.MUL, getNode(value1), getNode(value2)));
			
			for each (var arg : Object in args)
				result = multiply(result, arg);
			
			return result;
		}

		/**
		 * Compute term-to-term scalar division.
		 *
		 * @param arg1
		 * @param arg2
		 * @return
		 *
		 */
		protected final function divide(value1 : Object, value2 : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DIV, getNode(value1), getNode(value2)));
		}

		protected final function fractional(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.FRC, getNode(value)));
		}

		protected final function absolute(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.ABS, getNode(value)));
		}
		
		protected final function sign(value : Object) : SValue
		{
			return divide(value, absolute(value));
		}

		protected final function modulo(value : Object, base : Object) : SValue
		{
			return multiply(base, fractional(divide(value, base)));
		}

		protected final function reciprocal(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.RCP, getNode(value)));
		}

		/**
		 * Elevate the "base" value to the "exp" power.
		 *
		 * @param base
		 * @param exp
		 * @return
		 *
		 */
		protected final function power(base : Object, exp : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.POW, getNode(base), getNode(exp)));
		}

		protected final function add(value1 : Object, value2 : Object, ...args) : SValue
		{
			var result : SValue = new SValue(new Instruction(Instruction.ADD, getNode(value1), getNode(value2)));
			
			for each (var arg : Object in args)
				result = add(result, arg);
			
			return result;
		}

		protected final function subtract(value1 : Object, value2 : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SUB, getNode(value1), getNode(value2)));
		}

		protected final function dotProduct2(u : Object, v : Object) : SValue
		{
			var c : SValue = float3(1, 1, 0);
			return dotProduct3(multiply(u.xyy, c), v.xyy);
		}

		protected final function dotProduct3(u : Object, v : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DP3, getNode(u), getNode(v)));
		}

		protected final function dotProduct4(u : Object, v : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DP4, getNode(u), getNode(v)));
		}

		protected final function cross(u : Object, v : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.CRS, getNode(u), getNode(v)));
		}

		protected final function multiply4x4(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.M44, getNode(a), getNode(b)));
		}

		protected final function multiply3x3(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.M33, getNode(a), getNode(b)));
		}

		protected final function multiply3x4(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.M34, getNode(a), getNode(b)));
		}

		protected final function cos(angle : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.COS, getNode(angle)));
		}

		protected final function sin(angle : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SIN, getNode(angle)));
		}
		
		protected final function floor(value : Object) : SValue
		{
			var v : INode = getNode(value);
			return new SValue(new Instruction(Instruction.SUB, v, new Instruction(Instruction.FRC, v)));
		}
		
		protected final function tan(angle : Object) : SValue
		{
			return divide(sin(angle), cos(angle));
		}
		
		protected final function acos(angle : Object, numIterations : uint = 6) : SValue
		{
			var roughtGuess	: SValue = multiply(Math.PI / 2, subtract(1, angle));
			
			for (var stepId : uint = 0; stepId < numIterations; ++stepId)
				roughtGuess = add(
					roughtGuess, 
					divide(
						subtract(cos(roughtGuess), angle),
						sin(roughtGuess)
					)
				);
			
			return roughtGuess;
		}
		
		protected final function asin(angle : Object, numIterations : uint = 6) : SValue
		{
			return subtract(Math.PI / 2, acos(angle, numIterations));
		}
		
		protected final function atan(angle : Object, numIterations : uint = 6) : SValue
		{
			return asin(multiply(angle, rsqrt(add(1, multiply(angle, angle)))), numIterations);
		}
		
		protected final function atan2(y : Object, x : Object, numIterations : uint = 6) : SValue
		{
			var xy		: SValue = float2(x, y);
			var xyNrm	: SValue = sqrt(dotProduct2(xy, xy));
			
			return multiply(2, atan(divide(subtract(xyNrm, x), y), numIterations));
		}
		
		protected final function normalize(vector : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.NRM, getNode(vector)));
		}

		protected final function negate(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.NEG, getNode(value)));
		}

		protected final function saturate(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SAT, getNode(value)));
		}
		
		protected final function min(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.MIN, getNode(a), getNode(b)));
		}

		protected final function max(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.MAX, getNode(a), getNode(b)));
		}

		protected final function greaterEqual(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SGE, getNode(a), getNode(b)));
		}

		protected final function lessThan(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SLT, getNode(a), getNode(b)));
		}

		protected final function equal(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SEQ, getNode(a), getNode(b)));
		}
		
		protected final function notEqual(a : Object, b : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SNE, getNode(a), getNode(b)));
		}
		
		protected final function reflect(vector : Object, normal : Object) : SValue
		{
			return subtract(vector, multiply(2, dotProduct3(vector, normal), normal));
		}

		protected final function extract(value : Object, component : uint) : SValue
		{
			return new SValue(new Extract(getNode(value), component));
		}

		protected final function mix(a : Object, b : Object, factor : Object) : SValue
		{
			return add(a, multiply(factor, subtract(b, a)));
		}

		protected final function length(vector : Object) : SValue
		{
			var v : INode = getNode(vector);

			if (v.size == 2)
			{
				return new SValue(sqrt(dotProduct2(v, v)));
			}
			else if (v.size == 3)
			{
				return new SValue(sqrt(dotProduct3(v, v)));
			}
			else if (v.size == 4)
			{
				return new SValue(sqrt(dotProduct4(v, v)));
			}

			throw new Error("Unable to get the length of a value with size > 4.");
		}

		protected final function sqrt(scalar : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SQT, getNode(scalar)));
		}

		protected final function rsqrt(scalar : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.RSQ, getNode(scalar)));
		}

		protected final function getVertexAttribute(vertexComponent : VertexComponent,
													componentId		: uint = 0) : SValue
		{
			return new SValue(new Attribute(vertexComponent, componentId));
		}
		
		protected final function kill(value : Object) : void
		{
			main.minko_shader::_kills.push(getNode(value));
		}

		protected final function getParameter(bindingName	: String,
											  size			: uint) : SValue
		{
			return new SValue(new BindableConstant(bindingName, size));
		}
		
		protected final function getTexture(textureResource : TextureResource,
											filter			: uint = SamplerFilter.LINEAR,
											mipmap			: uint = SamplerMipmap.DISABLE,
											wrapping		: uint = SamplerWrapping.REPEAT,
											dimension		: uint = SamplerDimension.FLAT) : SValue
		{
			return new SValue(new Sampler(textureResource, filter, mipmap, wrapping, dimension));
		}
		
		protected final function getTextureParameter(bindingName	: String,
													 filter			: uint = SamplerFilter.LINEAR,
													 mipmap			: uint = SamplerMipmap.DISABLE,
													 wrapping		: uint = SamplerWrapping.REPEAT,
													 dimension		: uint = SamplerDimension.FLAT) : SValue
		{
			return new SValue(new BindableSampler(bindingName, filter, mipmap, wrapping, dimension));
		}
		
		protected final function getFieldFromArray(index	: Object,
												   constant : Object,
												   isMatrix	: Boolean) : SValue
		{
			var c	: INode	= getNode(constant);
			var i	: INode	= getNode(index);
			
			if (!(c is BindableConstant || c is Constant))
				throw new Error("Unable to use index on non-constant values.");
			
			return new SValue(new VariadicExtract(i, c, isMatrix));
		}
		
		private function getNode(value : Object) : INode
		{
			if (value is INode)
				return value as INode;
			
			if (value is SValue)
				return (value as SValue)._node;
			
			if (value is uint || value is int || value is Number)
				return new Constant(new <Number>[Number(value)]);
			
			throw new Error('This type cannot be casted to a shader value.');
		}
	}
}
