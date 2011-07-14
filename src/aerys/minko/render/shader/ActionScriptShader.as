package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.fog.Fog;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Cosine;
	import aerys.minko.render.shader.node.operation.builtin.CrossProduct;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Fractional;
	import aerys.minko.render.shader.node.operation.builtin.Maximum;
	import aerys.minko.render.shader.node.operation.builtin.Minimum;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Negate;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.Reciprocal;
	import aerys.minko.render.shader.node.operation.builtin.ReciprocalRoot;
	import aerys.minko.render.shader.node.operation.builtin.Saturate;
	import aerys.minko.render.shader.node.operation.builtin.Sine;
	import aerys.minko.render.shader.node.operation.builtin.SquareRoot;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.manipulation.Blend;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.math.PlanarReflection;
	import aerys.minko.render.shader.node.operation.math.Product;
	import aerys.minko.render.shader.node.operation.math.Sum;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Plane;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.geom.Point;
	import flash.utils.Dictionary;
	
	/**
	 * <p>Shader objects define vertex and fragment shaders with
	 * ActionScript code.</p>
	 * 
	 * <p>ActionScript shaders make it possible to write dynamic, parametric
	 * and OOP oriented shaders taking benefits of all the features of
	 * ActionScript. They fully integrate with the rest of your ActionScript
	 * code and the application development process. Thus, they allow to
	 * greatly simplify shaders writing and the rendering process as a whole.
	 * </p>
	 * 
	 * <p>
	 * Because ActionScript shaders are just actual ActionScript code, they
	 * can take into account the scene configuration (the number of lights,
	 * the need for hardware skinning, etc...). Using conditionnal statements
	 * and loops, one can write an ActionScript shader that will be able to
	 * handle many usecases by generating a different shader bytecode anytime
	 * the rendering or the scene configuration changes.
	 * </p>
	 * 
	 * <p>
	 * To create your own shaders using ActionScript code, you should extend
	 * the Shader class and override the following methods:</p>
	 * <ul>
	 * <li>getOutputPosition: this method will define the vertex shader</li>
	 * <li>getOutputColor: this method will define the fragment shader</li>
	 * <li>getHash (optionnal): this method should return a hash value
	 * that will be used as a unique key to identify the shader</li>
	 * </ul>
	 * 
	 * <p>
	 * Those methods and other shader dedicated methods heavily rely on
	 * two data types:
	 * </p>
	 * <ul>
	 * <li>Object: arguments are typed has Object to ensure polymorphism. Any
	 * of the following type can be used: int, uint, Number, Point, Vector4,
	 * Matrix4x4 and SValue. Arguments with types different from SValue will
	 * always be interpreted has static fixed constants.</li>
	 * <li>SValue: return values are types as SValue objects. They represent
	 * hardware memory proxies.</li>
	 * </ul>
	 * 
	 * <p>
	 * ActionScript shaders workflow at runtime is as follow:
	 * </p>
	 * <ul>
	 * <li>The shader hash is retrieved using the "getHash" method</li>
	 * <li>If a shader bytecode has already been compiled for this hash value,
	 * it is set and used directly.</li>
	 * <li>Else, the getOutputPosition and getOutputColor methods are called.
	 * Those methods transparently build and return a "shader graph"
	 * encapsulated in an SValue object.</li>
	 * <li>This shader graph is then optimized, compiled into AGAL
	 * bytecode and saved using the computed hash as a key.</li>
	 * <li>The compiled AGAL bytecode is then uploaded and used to render the
	 * scene.</li>
	 * </ul>
	 * 
	 * <p>
	 * You should always remember both getOutputPosition and getOutputColor
	 * are <b>never</b> executed on the graphics hardware. Instead, they help
	 * to transparently define a "shader program graph" that will be dynamically
	 * compiled at runtime into AGAL bytecode. Thus, you should always keep in
	 * mind that:
	 * </p>
	 * <ul>
	 * <li>Both getOutputPosition and getOutputColor are called only once when
	 * there is no valid AGAL bytecode available (ie. the "invalidate" method
	 * has been called or their is no shader bytecode associated with the
	 * hash retrieve by calling the "getHash" method).</li>
	 * <li>Conditionnals, loops and method calls with a return type different
	 * from SValue are considered as static fixed CPU-side code.</li>
	 * <li>Any value with a type different from SValue will be transparently
	 * turned into a static fixed shader constant.</li>
	 * <li>If you want to use updatable values, you should use parameters.</li>
	 * </ul>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ActionScriptShader
	{
		use namespace minko;
		
		private var _hashToShader	: Object		= new Object();
		
		private var _invalid		: Boolean		= true;
		
		private var _styleStack		: StyleStack	= null;
		private var _local			: LocalData		= null;
		private var _world			: Dictionary	= null;
		
		/**
		 * The position of the current vertex in clipspace (normalized
		 * screenspace).
		 *  
		 * @return 
		 * 
		 */
		protected final function get vertexClipspacePosition() : SValue
		{
			return multiply4x4(vertexPosition, localToScreenMatrix);
		}
		
		/**
		 * The position of the current vertex in world space. 
		 * @return 
		 * 
		 */
		protected final function get vertexWorldPosition() : SValue
		{
			return multiply4x4(vertexPosition, localToWorldMatrix);
		}
		
		/**
		 * The position of the current vertex in local space. 
		 * @return 
		 * 
		 */
		protected final function get vertexPosition() : SValue
		{
			return new SValue(new Attribute(VertexComponent.XYZ));
		}
		
		/**
		 * The RGB color of the current vertex. 
		 * @return 
		 * 
		 */
		protected final function get vertexRGBColor() : SValue
		{
			return new SValue(new Attribute(VertexComponent.RGB));
		}

		/**
		 * The RGBA color of the current vertex. 
		 * @return 
		 * 
		 */
		protected final function get vertexRGBAColor() : SValue
		{
			return new SValue(new Attribute(VertexComponent.RGBA));
		}

		/**
		 * The UV texture coordinates of the current vertex. 
		 * @return 
		 * 
		 */
		protected final function get vertexUV() : SValue
		{
			return new SValue(new Attribute(VertexComponent.UV));
		}
		
		/**
		 * The normal of the current vertex. 
		 * @return 
		 * 
		 */
		protected final function get vertexNormal() : SValue
		{
			return new SValue(new Attribute(VertexComponent.NORMAL));
		}
		
		/**
		 * The tangent of the current vertex. 
		 * @return 
		 * 
		 */
		protected final function get vertexTangent() : SValue
		{
			return new SValue(new Attribute(VertexComponent.TANGENT));
		}
		
		/**
		 * The direction of the camera in local space. 
		 * @return 
		 * 
		 */
		protected final function get cameraLocalDirection() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.LOCAL_DIRECTION));
		}
		
		/**
		 * The position of the camera in world space. 
		 * @return 
		 * 
		 */
		protected final function get cameraPosition() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.POSITION));
		}
		
		/**
		 * The position of the camera in local space. 
		 * @return 
		 * 
		 */
		protected final function get cameraLocalPosition() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.LOCAL_POSITION));
		}
		
		/**
		 * The direction of the camera in world space. 
		 * @return 
		 * 
		 */
		protected final function get cameraDirection() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.DIRECTION));
		}
		
		/**
		 * The local-to-screen (= world * view * projection) transformation matrix. 
		 * @return 
		 * 
		 */
		protected final function get localToScreenMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.LOCAL_TO_SCREEN));
		}
		
		/**
		 * The local-to-world (= world) transformation matrix. 
		 * @return 
		 * 
		 */
		protected final function get localToWorldMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.WORLD));
		}
		
		/**
		 * The local-to-view (= world * view) transformation matrix. 
		 * @return 
		 * 
		 */
		protected final function get localToViewMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.LOCAL_TO_VIEW));
		}
		
		/**
		 * The world-to-local (= world^-1) transformation matrix. 
		 * @return 
		 * 
		 */
		protected final function get worldToLocalMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.WORLD_INVERSE));
		}
		
		/**
		 * The world-to-view (= world * view) transformation matrix. 
		 * @return 
		 * 
		 */
		protected final function get worldToViewMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.VIEW));
		}
		
		/**
		 * The view-to-clipspace (= projection) matrix. 
		 * @return 
		 * 
		 */
		protected final function get projectionMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.PROJECTION));
		}
		
		public function fillRenderState(state	: RendererState, 
										style	: StyleStack, 
										local	: LocalData, 
										world	: Dictionary) : Boolean
		{
			var hash 	: String 		= getDataHash(style, local, world);
			var shader 	: Shader = _hashToShader[hash];
			
			_styleStack = style;
			_local = local;
			_world = world;
			
			if (!shader || _invalid)
			{
				_invalid = false;
				_hashToShader[hash] = shader = Shader.create(getOutputPosition()._node,
															 getOutputColor()._node);
			}
			
			shader.fillRenderState(state, style, local, world);
			
			return true;
		}
		
		/**
		 * The getDataHash method returns a String computed from the style, local and world data.
		 * This value is used as a hash that defines all the values used as conditionnals in the
		 * vertex (getOutputPosition) or frament (getOutputColor) shaders.
		 *  
		 * @param style
		 * @param local
		 * @param world
		 * @return 
		 * 
		 */
		protected function getDataHash(style	: StyleStack, 
									   local	: LocalData, 
									   world	: Dictionary) : String
		{
			return "";
		}
		
		/**
		 * The getOutputPosition method implements a vertex shader using ActionScript code. 
		 * @return 
		 * 
		 */
		protected function getOutputPosition() : SValue
		{
			throw new Error();
		}
		
		/**
		 * The getOutputColor method implements a fragment shader using ActionScript code.
		 * @return 
		 * 
		 */
		protected function getOutputColor() : SValue
		{
			throw new Error();
		}
		
		/**
		 * Return a style value that will be passed as a shader constant.
		 * 
		 * @param styleId
		 * @param defaultValue
		 * @return 
		 * 
		 */
		protected final function getStyleConstant(styleId : int, defaultValue : Object = null) : Object
		{
			return _styleStack.get(styleId, defaultValue);
		}
		
		/**
		 * Return whether a specific style is set or not. 
		 * @param styleId
		 * @return 
		 * 
		 */
		protected final function styleIsSet(styleId : int) : Boolean
		{
			return _styleStack.isSet(styleId);
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
		protected final function combine(value1	: Object,
										 value2	: Object,
										 ...values) : SValue
		{
			var result 		: Combine 	= new Combine(getNode(value1), getNode(value2));
			var numValues 	: int 		= values.length;
			
			for (var i : int = 0; i < numValues; ++i)
				result = new Combine(result, getNode(values[i]));
			
			return new SValue(result);
		}
		
		private final function toFloat(size : uint, values : Array) : SValue
		{
			var inputSize 	: uint 	= 0;
			var numValues	: uint	= values.length;
			var result		: INode	= null;
			var i 			: int 	= 0;
			var value 		: INode	= null;
			var validValue	: INode	= null;
			
			for (i = 0; i < numValues && size > 0; ++i)
			{
				value = getNode(values[i]);
				
				if (!value)
					continue ;
				
				validValue = value;
				
				var sizeToComponents	: Array	= [Components.X, Components.XY, Components.XYZ];
				
				size -= value.size;
				if (size < 0)
					result = new Combine(result, new Extract(value, sizeToComponents[int(-size + 1)]));
				else
					result = result ? new Combine(result, value) : value;
			}
			
			if (size > 0)
			{
				var last 	: INode 	= null;
				var zero	: Constant	= new Constant(0.);
				
				if (validValue)
				{
					if (validValue.size == 1)
						last = new Extract(validValue, Components.X);
					else if (validValue.size == 2)
						last = new Extract(validValue, Components.Y);
					else if (validValue.size == 3)
						last = new Extract(validValue, Components.Z);
					else if (validValue.size == 4)
						last = new Extract(validValue, Components.W);
				}
				
				while (size)
				{
					if (last)
						result = new Combine(result, last);
					else
						result = result ? new Combine(result, zero) : zero;
					size--;
				}
			}
			
			return new SValue(result);
		}
		
		protected final function float2(x : Object, y : Object = null) : SValue
		{
			return toFloat(2, [x, y]);
		}
		
		/**
		 * Create a new SVAlue object of size 3 by combining 3 scalar values.
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
		protected final function float3(x : Object, y : Object = null, z : Object = null) : SValue
		{
			return toFloat(3, [x, y, z]);
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
		protected final function float4(x : Object, y : Object = null, z : Object = null, w : Object = null) : SValue
		{
			return toFloat(4, [x, y, z, w]);
		}
		
		/**
		 * Retrieve the RGBA color of a pixel of a texture.
		 *  
		 * @param styleId
		 * @param uv
		 * @param filtering
		 * @param mipMapping
		 * @param wrapping
		 * @return 
		 * 
		 */
		protected final function sampleTexture(styleId 		: int,
											   uv 			: Object,
											   filtering	: uint	= Sampler.FILTER_LINEAR,
											   mipMapping	: uint	= Sampler.MIPMAP_LINEAR,
											   wrapping		: uint	= Sampler.WRAPPING_REPEAT) : SValue
		{
			return new SValue(new Texture(getNode(uv), new Sampler(styleId, filtering, mipMapping, wrapping)));
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
			var p 		: Product 	= new Product(getNode(value1), getNode(value2));
			var numArgs : int 		= args.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				p.addTerm(getNode(args[i]))
			
			return new SValue(p);
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
			return new SValue(new Divide(getNode(value1), getNode(value2)));
		}
		
		protected final function fractional(value : Object) : SValue
		{
			return new SValue(new Fractional(getNode(value)));
		}
		
		protected final function modulo(value : Object, base : Object) : SValue
		{
			var baseNode : INode = getNode(base);
			
			return new SValue(new Multiply(
				baseNode,
				new Fractional(new Divide(getNode(value), baseNode)))
			);
		}
		
		protected final function reciprocal(value : Object) : SValue
		{
			return new SValue(new Reciprocal(getNode(value)));
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
			return new SValue(new Power(getNode(base), getNode(exp)));
		}
		
		protected final function add(value1 : Object, value2 : Object, ...values) : SValue
		{
			var sum		: Sum	= new Sum(getNode(value1), getNode(value2));
			var numArgs : int	= values.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				sum.addTerm(getNode(values[i]))
			
			return new SValue(sum);
		}
		
		protected final function subtract(value1 : Object, value2 : Object) : SValue
		{
			return new SValue(new Substract(getNode(value1), getNode(value2)));
		}
		
		protected final function dotProduct3(u : Object, v : Object) : SValue
		{
			return new SValue(new DotProduct3(getNode(u), getNode(v)));
		}
		
		protected final function dotProduct4(u : Object, v : Object) : SValue
		{
			return new SValue(new DotProduct4(getNode(u), getNode(v)));
		}
		
		protected final function cross(u : Object, v : Object) : SValue
		{
			return new SValue(new CrossProduct(getNode(u), getNode(v)));
		}
		
		protected final function multiply4x4(a : Object, b : Object) : SValue
		{
			return new SValue(new Multiply4x4(getNode(a), getNode(b)));
		}
		
		protected final function cos(angle : Object) : SValue
		{
			return new SValue(new Cosine(getNode(angle)));
		}
		
		protected final function sin(angle : Object) : SValue
		{
			return new SValue(new Sine(getNode(angle)));
		}
		
		protected final function normalize(vector : Object) : SValue
		{
			return new SValue(new Normalize(getNode(vector)));
		}
		
		protected final function negate(value : Object) : SValue
		{
			return new SValue(new Negate(getNode(value)));
		}
		
		protected final function saturate(value : Object) : SValue
		{
			return new SValue(new Saturate(getNode(value)));
		}
		
		protected final function max(a : Object, b : Object, ...values) : SValue
		{
			var max 		: Maximum 	= new Maximum(getNode(a), getNode(b));
			var numValues 	: int		= values.length;
			
			for (var i : int = 0; i < numValues; ++i)
				max = new Maximum(max, getNode(values[i]));
			
			return new SValue(max);
		}
		
		protected final function min(a : Object, b : Object, ...values) : SValue
		{
			var max 		: Minimum 	= new Minimum(getNode(a), getNode(b));
			var numValues 	: int 		= values.length;
			
			for (var i : int = 0; i < numValues; ++i)
				max = new Minimum(max, getNode(values[i]));
			
			return new SValue(max);
		}
		
		protected final function getReflectedVector(vector : Object, normal : Object) : SValue
		{
			return new SValue(new PlanarReflection(getNode(vector), getNode(normal)));
		}
		
		protected final function getWorldParameter(size		: uint, 
												   key		: Class,
												   field	: String	= null, 
												   index	: int		= -1) : SValue
		{
			return new SValue(new WorldParameter(size, key, field, index));
		}
		
		protected final function getLocalParameter(size		: uint, 
												   key		: Object) : SValue
		{
			return new SValue(new TransformParameter(size, key));
		}
		
		protected final function getStyleParameter(size 	: uint,
												   key 		: int,
												   field 	: String 	= null,
												   index 	: int 		= -1) : SValue
		{
			return new SValue(new StyleParameter(size, key, field, index));
		}
		
		protected final function getConstant(value : Object) : SValue
		{
			return new SValue(getNode(value));
		}
		
		protected final function extract(value : Object, component : uint) : SValue
		{
			return new SValue(new Extract(getNode(value), component));
		}
		
		protected final function blend(color1 : Object, color2 : Object, blending : uint) : SValue
		{
			return new SValue(new Blend(getNode(color1), getNode(color2), blending));
		}
		
		protected final function length(vector : Object) : SValue
		{
			var v : INode = getNode(vector);

			if (v.size == 2)
			{
				var x : INode = new Extract(v, Components.X);
				var y : INode = new Extract(v, Components.Y);
				
				return new SValue(sqrt(add(multiply(x, x), multiply(y, y))));
			}
			else if (v.size == 3)
				return new SValue(sqrt(dotProduct3(v, v)));
			else if (v.size == 4)
				return new SValue(sqrt(dotProduct4(v, v)));
			
			throw new Error("Unable to get the length of a value with size > 4.");
		}
		
		protected final function sqrt(scalar : Object) : SValue
		{
			return new SValue(new SquareRoot(getNode(scalar)));
		}
		
		protected final function rsqrt(scalar : Object) : SValue
		{
			return new SValue(new ReciprocalRoot(getNode(scalar)));
		}
		
		protected final function getFogColor(start		: Object,
											 distance	: Object,
											 color		: Object) : SValue
		{
			return new SValue(new Fog(getNode(start), getNode(distance), getNode(color)));
		}
		
		protected final function getVertexAttribute(vertexComponent : VertexComponent) : SValue
		{
			return new SValue(new Attribute(vertexComponent));
		}
		
		/**
		 * Invalidate both the vertex and fragment shader bytecode and force it
		 * to be recompiled next time the shader will be used to draw
		 * triangles.
		 * 
		 * <p>You can use the "invalidate" method to force the shader
		 * compilation anytime a value used in conditionnals or loop
		 * statements changes.</p>
		 * 
		 * <p>If you do not want to have to maniually invalidate the shader
		 * bytecode, you should implement the "getHash" method properly.</p>
		 * 
		 */
		public function invalidate() : void
		{
			_invalid = true;
		}
		
		private function getNode(value : Object) : INode
		{
			if (!value)
				return null;
			
			if (value is INode)
				return value as INode;
			
			if (value is SValue)
				return (value as SValue)._node;
			
			var c	: Constant	= new Constant();
			
			if (value is int || value is uint || value is Number)
			{
				c.constants[0] = value as Number;
			}
			else if (value is Point)
			{
				var point	: Point	= value as Point;
				
				c.constants[0] = point.x;
				c.constants[1] = point.y;
			}
			else if (value is Vector4)
			{
				var vector 	: Vector4 	= value as Vector4;
				
				c.constants[0] = vector.x;
				c.constants[1] = vector.y;
				c.constants[2] = vector.z;
				if (!isNaN(vector.w))
					c.constants[3] = vector.w;
			}
			else if (value is Matrix4x4)
			{
				(value as Matrix4x4).getRawData(c.constants);
			}
			else
				throw new Error("Constants can only be int, uint, Number, Point, Vector4 or Matrix4x4 values.");
			
			return c;
		}
	}
}