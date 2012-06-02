package aerys.minko.render.shader.part
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderDataBindings;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.geom.Rectangle;

	/**
	 * The base class to create ActionScript shader parts.
	 * 
	 * @author Jean-Marc Le Roux
	 * @author Romain Gilliotte
	 */
	public class ShaderPart
	{
		use namespace minko_shader;
		
		private var _main	: Shader	= null;
		
		protected final function get main() : Shader
		{
			return _main;
		}

		protected function get vertexId() : SFloat
		{
			return getVertexAttribute(VertexComponent.ID);
		}
		
		/**
		 * The position of the current vertex in local space.
		 * @return
		 *
		 */
		protected function get vertexXYZ() : SFloat
		{
			return getVertexAttribute(VertexComponent.XYZ);
		}
		
		protected function get vertexXY() : SFloat
		{
			return getVertexAttribute(VertexComponent.XY);
		}

		/**
		 * The RGB color of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexRGBColor() : SFloat
		{
			return getVertexAttribute(VertexComponent.RGB);
		}

		/**
		 * The RGBA color of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexRGBAColor() : SFloat
		{
			return getVertexAttribute(VertexComponent.RGBA);
		}

		/**
		 * The UV texture coordinates of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexUV() : SFloat
		{
			return getVertexAttribute(VertexComponent.UV);
		}

		/**
		 * The normal of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexNormal() : SFloat
		{
			return getVertexAttribute(VertexComponent.NORMAL);
		}

		/**
		 * The tangent of the current vertex.
		 * @return
		 *
		 */
		protected function get vertexTangent() : SFloat
		{
			return getVertexAttribute(VertexComponent.TANGENT);
		}
		
		protected function get localToWorldMatrix() : SFloat
		{
			return _main._meshBindings.getParameter("localToWorld", 16);
		}
		
		protected function get worldToLocalMatrix() : SFloat
		{
			return _main._meshBindings.getParameter("worldToLocal", 16);
		}
		
		protected function get worldToViewMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter("worldToView", 16);
		}
		
		protected function get viewToWorldMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter('viewToWorld', 16);
		}
		
		protected function get worldToScreenMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter("worldToScreen", 16);
		}
		
		protected function get screenToWorldMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter("screenToWorld", 16);
		}
		
		protected function get projectionMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter("projection", 16);
		}
		
		protected function get screenToViewMatrix() : SFloat
		{
			return _main._sceneBindings.getParameter("screenToView", 16);
		}
		
		protected function get cameraPosition() : SFloat
		{
			return multiply4x4(float4(0, 0, 0, 1), viewToWorldMatrix);
		}
		
		protected function get cameraDirection() : SFloat
		{
			return multiply4x4(float4(0, 0, 1, 1), viewToWorldMatrix);
		}
		
		protected function get cameraZNear() : SFloat
		{
			return _main._sceneBindings.getParameter("cameraZNear", 1);
		}
		
		protected function get cameraZFar() : SFloat
		{
			return _main._sceneBindings.getParameter("cameraZFar", 1);
		}
		
		protected function get viewportWidth() : SFloat
		{
			return _main._sceneBindings.getParameter("viewportWidth", 1);
		}
		
		protected function get viewportHeight() : SFloat
		{
			return _main._sceneBindings.getParameter("viewportHeight", 1);
		}
		
		protected function get time() : SFloat
		{
			return _main._sceneBindings.getParameter("time", 1);
		}
		
		public function ShaderPart(main : Shader) : void
		{
			_main = main;
		}
		
		protected function get meshBindings() : ShaderDataBindings
		{
			return _main._meshBindings;
		}
		
		protected function get sceneBindings() : ShaderDataBindings
		{
			return _main._sceneBindings;
		}
				
		/**
		 * Interpolate vertex shader values to make them usable inside the
		 * fragment shader.
		 *
		 * <p>Any SFloat object comming from the vertex shader but used inside
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
		protected final function interpolate(value : SFloat) : SFloat
		{
			return new SFloat(new Interpolate(getNode(value)));
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
		 * override protected function getOutputColor() : SFloat
		 * {
		 * 	// interpolate the vertex RGB color
		 * 	var color : SFloat = interpolate(vertexRGBColor);
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
		private function toFloat(size : int, values : Array) : SFloat
		{
			var currentOffset	: uint = 0;
			
			var args			: Vector.<AbstractNode>	= new Vector.<AbstractNode>();
			var components		: Vector.<uint>			= new Vector.<uint>();
			
			for each (var value : Object in values)
			{
				var node		: AbstractNode	= getNode(value);
				var nodeSize	: uint			= node.size;
				
				if (currentOffset + nodeSize > size)
					throwInvalidSizeError(size, currentOffset + nodeSize);
				
				args.push(node);
				components.push(Components.createContinuous(currentOffset, 0, nodeSize, nodeSize));
				currentOffset += nodeSize;
			}
			
			if (currentOffset != size)
				throwInvalidSizeError(size, currentOffset);
			
			return new SFloat(new Overwriter(args, components));
		}
		
		private function throwInvalidSizeError(expected	: uint,
											   received	: uint) : void
		{
			throw new Error(
				'Invalid size specified: expected ' + expected
				+ ' but got ' + received + '.'
			);
		}

		protected final function float(x : Object) : SFloat
		{
			var node : AbstractNode = getNode(x);
			if (node.size == 1)
				return new SFloat(node);
			else
				throw new Error('Invalid argument');
		}

		protected final function float2(x : Object, y : Object = null) : SFloat
		{
			if (x != null && y == null)
				return toFloat(2, [x]);
			else if (x != null && y != null)
				return toFloat(2, [x, y]);
			else
				throw new Error('Invalid arguments');
		}

		/**
		 * Create a new SFloat object of size 3 by combining up to 3 values.
		 *
		 * <p>This method is an alias of the "combine" method. You should prefer this
		 * method everytime you want to build an SFloat object of size 3 because this
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
										z : Object = null) : SFloat
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
		 * Create a new SFloat object of size 4 by combining up to 4 values.
		 *
		 * <p>This method is an alias of the "combine" method. You should prefer this
		 * method everytime you want to build an SFloat object of size 4 because
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
										w : Object = null) : SFloat
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
		 * 
		 * @param scalar
		 * @return 
		 * 
		 */
		protected final function pack(scalar : Object) : SFloat
		{
			if (getNode(scalar).size != 1)
				throw new ArgumentError('Argument of size 1 expected. ' + scalar + ' received');
			
			var bitSh	: SFloat = float4(256. * 256. * 256., 256. * 256, 256., 1.);
			var bitMsk	: SFloat = float4(0., 1. / 256., 1. / 256., 1. / 256.);
			var comp	: SFloat = fractional(multiply(scalar, bitSh));
			
			return subtract(comp, multiply(comp.xxyz, bitMsk));
		}
		
		/**
		 * Unpack a [0 .. 1] scalar value from a float4 RGBA color value.
		 * 
		 * @param packedScalar
		 * @return 
		 * 
		 */
		protected final function unpack(packedScalar : Object) : SFloat
		{
			var bitSh : SFloat = float4(1. / (256. * 256. * 256.), 1. / (256. * 256.), 1. / 256., 1.);
			
			return dotProduct4(packedScalar, bitSh);
		}
		
		protected final function rgba(color : uint) : SFloat
		{
			return float4(
				((color >>> 24) & 0xff) / 255.,
				((color >>> 16) & 0xff) / 255.,
				((color >>> 8) & 0xff) / 255.,
				(color & 0xff) / 255.
			);
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
		protected final function sampleTexture(texture	: SFloat,
											   uv 		: Object) : SFloat
		{
			
			
			return new SFloat(new Instruction(Instruction.TEX, getNode(uv), getNode(texture)));
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
		protected final function multiply(value1 : Object, value2 : Object, ...args) : SFloat
		{
			var result : SFloat = new SFloat(new Instruction(Instruction.MUL, getNode(value1), getNode(value2)));
			
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
		protected final function divide(value1 : Object, value2 : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.DIV, getNode(value1), getNode(value2)));
		}

		protected final function fractional(value : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.FRC, getNode(value)));
		}

		protected final function absolute(value : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.ABS, getNode(value)));
		}
		
		protected final function sign(value : Object) : SFloat
		{
			return divide(value, absolute(value));
		}

		protected final function modulo(value : Object, base : Object) : SFloat
		{
			return multiply(base, fractional(divide(value, base)));
		}

		protected final function reciprocal(value : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.RCP, getNode(value)));
		}

		/**
		 * Elevate the "base" value to the "exp" power.
		 *
		 * @param base
		 * @param exp
		 * @return
		 *
		 */
		protected final function power(base : Object, exp : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.POW, getNode(base), getNode(exp)));
		}

		protected final function add(a : Object, b : Object, ...args) : SFloat
		{
			var result : SFloat = new SFloat(new Instruction(Instruction.ADD, getNode(a), getNode(b)));
			
			for each (var arg : Object in args)
				result = add(result, arg);
			
			return result;
		}

		protected final function subtract(value1 : Object, value2 : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SUB, getNode(value1), getNode(value2)));
		}

		protected final function dotProduct2(u : Object, v : Object) : SFloat
		{
			var c : SFloat = float3(1, 1, 0);
			
			return dotProduct3(multiply(u.xyy, c), v.xyy);
		}

		protected final function dotProduct3(u : Object, v : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.DP3, getNode(u), getNode(v)));
		}

		protected final function dotProduct4(u : Object, v : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.DP4, getNode(u), getNode(v)));
		}

		protected final function crossProduct(u : Object, v : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.CRS, getNode(u), getNode(v)));
		}

		protected final function multiply4x4(vector : Object, matrix : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.M44, getNode(vector), getNode(matrix)));
		}

		protected final function multiply3x3(vector : Object, matrix : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.M33, getNode(vector), getNode(matrix)));
		}

		protected final function multiply3x4(vector : Object, matrix : Object) : SFloat
		{
			var vectorNode : AbstractNode = getNode(vector);
			
			if (vectorNode.size < 4)
				throw new Error("The argument 'vector' should have a size of 4.");
			
			return new SFloat(new Instruction(Instruction.M34, vectorNode, getNode(matrix)));
		}

		protected final function cos(angle : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.COS, getNode(angle)));
		}

		protected final function sin(angle : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SIN, getNode(angle)));
		}
		
		protected final function floor(value : Object) : SFloat
		{
			return subtract(value, fractional(value));
		}
		
		protected final function ceil(value : Object) : SFloat
		{
			var frac		: SFloat = fractional(value);
			var isNotWhole	: SFloat = greaterEqual(frac, 0);
			
			return add(isNotWhole, subtract(value, frac));
		}
		
		protected final function tan(angle : Object) : SFloat
		{
			return divide(sin(angle), cos(angle));
		}
		
		protected final function acos(angle : Object, numIterations : uint = 6) : SFloat
		{
			var roughtGuess	: SFloat = multiply(Math.PI / 2, subtract(1, angle));
			
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
		
		protected final function asin(angle : Object, numIterations : uint = 6) : SFloat
		{
			return subtract(Math.PI / 2, acos(angle, numIterations));
		}
		
		protected final function atan(angle : Object, numIterations : uint = 6) : SFloat
		{
			return asin(multiply(angle, rsqrt(add(1, multiply(angle, angle)))), numIterations);
		}
		
		protected final function atan2(y : Object, x : Object, numIterations : uint = 6) : SFloat
		{
			var xy		: SFloat = float2(x, y);
			var xyNrm	: SFloat = sqrt(dotProduct2(xy, xy));
			
			return multiply(2, atan(divide(subtract(xyNrm, x), y), numIterations));
		}
		
		protected final function normalize(vector : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.NRM, getNode(vector)));
		}

		protected final function negate(value : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.NEG, getNode(value)));
		}

		protected final function saturate(value : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SAT, getNode(value)));
		}
		
		protected final function min(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.MIN, getNode(a), getNode(b)));
		}

		protected final function max(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.MAX, getNode(a), getNode(b)));
		}

		protected final function greaterEqual(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SGE, getNode(a), getNode(b)));
		}

		protected final function lessThan(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SLT, getNode(a), getNode(b)));
		}

		protected final function equal(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SEQ, getNode(a), getNode(b)));
		}
		
		protected final function notEqual(a : Object, b : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SNE, getNode(a), getNode(b)));
		}
		
		protected final function reflect(vector : Object, normal : Object) : SFloat
		{
			return subtract(vector, multiply(2, dotProduct3(vector, normal), normal));
		}

		protected final function extract(value : Object, component : uint) : SFloat
		{
			return new SFloat(new Extract(getNode(value), component));
		}

		protected final function mix(a : Object, b : Object, factor : Object) : SFloat
		{
			return add(a, multiply(factor, subtract(b, a)));
		}
		
		protected final function transform2DCoordinates(value	: Object,
												 		source	: Rectangle, 
														target	: Rectangle) : SFloat
		{
			// could be done with a multiply2x2, but useless here
			var sourceOrigin	: SFloat = float2(-source.x, -source.y);
			var targetOrigin	: SFloat = float2(target.x, target.y);
			var scale			: SFloat = float2(target.width / source.width, target.height / source.height);
			
			return add(targetOrigin, multiply(scale, add(value, sourceOrigin)));
		}
		
		protected final function length(vector : Object) : SFloat
		{
			var v : AbstractNode = getNode(vector);

			if (v.size == 2)
			{
				return new SFloat(sqrt(dotProduct2(vector, vector)));
			}
			else if (v.size == 3)
			{
				return new SFloat(sqrt(dotProduct3(vector, vector)));
			}
			else if (v.size == 4)
			{
				return new SFloat(sqrt(dotProduct4(vector, vector)));
			}

			throw new Error("Unable to get the length of a value with size > 4.");
		}

		protected final function sqrt(scalar : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.SQT, getNode(scalar)));
		}

		protected final function rsqrt(scalar : Object) : SFloat
		{
			return new SFloat(new Instruction(Instruction.RSQ, getNode(scalar)));
		}
		
		protected final function getVertexAttribute(vertexComponent : VertexComponent,
													componentId		: uint = 0) : SFloat
		{
			return new SFloat(new Attribute(vertexComponent, componentId));
		}
		
		protected final function kill(value : Object) : void
		{
			main._kills.push(getNode(value));
		}
		
		protected final function localToWorld(value : Object) : SFloat
		{
			return multiply4x4(value, localToWorldMatrix);
		}
		
		protected final function localToView(value : Object) : SFloat
		{
			return multiply4x4(localToWorld(value), worldToViewMatrix);
		}
		
		protected final function worldToLocal(value : Object) : SFloat
		{
			return multiply4x4(value, worldToLocalMatrix);
		}
		
		protected final function worldToView(value : Object) : SFloat
		{
			return multiply4x4(value, worldToViewMatrix);
		}
		
		protected final function localToScreen(vertex : Object) : SFloat
		{
			return multiply4x4(
				localToWorld(vertex),
				worldToScreenMatrix
			);
		}
		
		protected final function deltaLocalToWorld(vertex : Object) : SFloat
		{
			return multiply3x3(vertex, localToWorldMatrix);
		}
		
		protected final function deltaWorldToLocal(vertex : Object) : SFloat
		{
			return multiply3x3(vertex, worldToLocalMatrix);
		}
		
		protected final function deltaWorldToView(vertex : Object) : SFloat
		{
			return multiply3x3(vertex, worldToViewMatrix);
		}
		
		protected final function deltaViewToWorld(vertex : Object) : SFloat
		{
			return multiply3x3(vertex, viewToWorldMatrix);
		}
		
		protected final function getTexture(textureResource : ITextureResource,
											filter			: uint = SamplerFiltering.LINEAR,
											mipmap			: uint = SamplerMipMapping.DISABLE,
											wrapping		: uint = SamplerWrapping.REPEAT,
											dimension		: uint = SamplerDimension.FLAT) : SFloat
		{
			return new SFloat(new Sampler(textureResource, filter, mipmap, wrapping, dimension));
		}
		
		protected final function getFieldFromArray(index	: Object,
												   constant : Object,
												   isMatrix	: Boolean	= false) : SFloat
		{
			var c	: AbstractNode	= getNode(constant);
			var i	: AbstractNode	= getNode(index);
			
			if (!(c is BindableConstant || c is Constant))
				throw new Error("Unable to use index on non-constant values.");
			
			return new SFloat(new VariadicExtract(i, c, isMatrix));
		}
		
		private function getNode(value : Object) : AbstractNode
		{
			if (value is AbstractNode)
				return value as AbstractNode;
			
			if (value is SFloat)
				return (value as SFloat)._node;
			
			if (value is uint || value is int || value is Number)
				return new Constant(new <Number>[Number(value)]);
			
			if (value is Matrix4x4)
				return new Constant(Matrix4x4(value).getRawData(null, 0, true));
			
			throw new Error('This type cannot be casted to a shader value.');
		}
	}
}
