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
	import aerys.minko.render.shader.node.operation.builtin.Maximum;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Negate;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Power;
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
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Plane;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.geom.Point;
	import flash.utils.Dictionary;
	
	public class ParametricShader
	{
		use namespace minko;
		
		private var _shadersMap		: Object		= new Object();
		
		private var _styleStack		: StyleStack	= null;
		private var _local			: LocalData		= null;
		private var _world			: Dictionary	= null;
		
		protected final function get vertexClipspacePosition() : SValue
		{
			return multiply4x4(vertexPosition, localToScreenMatrix);
		}
		
		protected final function get vertexWorldPosition() : SValue
		{
			return multiply4x4(vertexPosition, localToWorldMatrix);
		}
		
		protected final function get vertexPosition() : SValue
		{
			return new SValue(new Attribute(VertexComponent.XYZ));
		}
		
		protected final function get vertexColor() : SValue
		{
			return new SValue(new Attribute(VertexComponent.RGB));
		}
		
		protected final function get vertexUV() : SValue
		{
			return new SValue(new Attribute(VertexComponent.UV));
		}
		
		protected final function get vertexNormal() : SValue
		{
			return new SValue(new Attribute(VertexComponent.NORMAL));
		}
		
		protected final function get vertexTangent() : SValue
		{
			return new SValue(new Attribute(VertexComponent.TANGENT));
		}
		
		protected final function get cameraLocalDirection() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.LOCAL_DIRECTION));
		}
		
		protected final function get cameraPosition() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.POSITION));
		}
		
		protected final function get cameraLocalPosition() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.LOCAL_POSITION));
		}
		
		protected final function get cameraDirection() : SValue
		{
			return new SValue(new WorldParameter(3, CameraData, CameraData.DIRECTION));
		}
		
		protected final function get localToScreenMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.LOCAL_TO_SCREEN));
		}
		
		protected final function get localToWorldMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.WORLD));
		}
		
		protected final function get localToViewMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.LOCAL_TO_VIEW));
		}
		
		protected final function get worldToLocalMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.WORLD_INVERSE));
		}
		
		protected final function get worldToViewMatrix() : SValue
		{
			return new SValue(new TransformParameter(16, LocalData.VIEW));
		}
		
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
			var shader 	: DynamicShader = _shadersMap[hash];
			
			_styleStack = style;
			_local = local;
			_world = world;
			
			if (!shader)
				_shadersMap[hash] = shader = DynamicShader.create(getOutputPosition()._node,
																  getOutputColor()._node);
			
			shader.fillRenderState(state, style, local, world);
			
			return true;
		}
		
		protected function getDataHash(style	: StyleStack, 
									   local	: LocalData, 
									   world	: Dictionary) : String
		{
			return "";
		}
		
		protected function getOutputPosition() : SValue
		{
			throw new Error();
		}
		
		protected function getOutputColor() : SValue
		{
			throw new Error();
		}
		
		protected final function getStyleConstant(styleId : int, defaultValue : Object = null) : Object
		{
			return _styleStack.get(styleId, defaultValue);
		}
		
		protected final function styleIsSet(styleId : int) : Boolean
		{
			return _styleStack.isSet(styleId);
		}
		
		protected final function interpolate(value : SValue) : SValue
		{
			return new SValue(new Interpolate(getNode(value)));
		}
		
		protected final function combine(value1	: Object,
										 value2	: Object,
										 ...values) : SValue
		{
			var result : Combine = new Combine(getNode(value1), getNode(value2));
			var numValues : int = values.length;
			
			for (var i : int = 0; i < numValues; ++i)
				result = new Combine(result, getNode(values[i]));
			
			return new SValue(result);
		}
		
		protected final function vector3(x : Object, y : Object, z : Object) : SValue
		{
			return combine(x, y, z);
		}
		
		protected final function vector4(x : Object, y : Object, z : Object, w : Object) : SValue
		{
			return combine(x, y, z, w);
		}
		
		protected final function sampleTexture(styleId 		: int,
											   uv 			: Object,
											   filtering	: uint	= Sampler.FILTER_LINEAR,
											   mipMapping	: uint	= Sampler.MIPMAP_LINEAR,
											   wrapping		: uint	= Sampler.WRAPPING_REPEAT) : SValue
		{
			return new SValue(new Texture(getNode(uv), new Sampler(styleId, filtering, mipMapping, wrapping)));
		}
		
		protected final function multiply(arg1 : Object, arg2 : Object, ...args) : SValue
		{
			var p 		: Product 	= new Product(getNode(arg1), getNode(arg2));
			var numArgs : int 		= args.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				p.addTerm(getNode(args[i]))
			
			return new SValue(p);
		}
		
		protected final function divide(arg1 : Object, arg2 : Object) : SValue
		{
			return new SValue(new Divide(getNode(arg1), getNode(arg2)));
		}
		
		protected final function pow(base : Object, exp : Object) : SValue
		{
			return new SValue(new Power(getNode(base), getNode(exp)));
		}
		
		protected final function add(value1 : Object, value2 : Object) : SValue
		{
			return new SValue(new Add(getNode(value1), getNode(value2)));
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
		
		protected final function max(a : Object, b : Object, ...values) : SValue
		{
			var max : Maximum = new Maximum(getNode(a), getNode(b));
			var numValues : int = values.length;
			
			for (var i : int = 0; i < numValues; ++i)
				max = new Maximum(max, getNode(values[i]));
			
			return new SValue(max);
		}
		
		protected final function planarReflection(vector : Object, normal : Object) : SValue
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
		
		protected final function getFolorColor(start	: Object,
											   distance	: Object,
											   color	: Object) : SValue
		{
			return new SValue(new Fog(getNode(start), getNode(distance), getNode(color)));
		}
		
		protected final function getVertexAttribute(vertexComponent : VertexComponent) : SValue
		{
			return new SValue(new Attribute(vertexComponent));
		}
		
		private function getNode(value : Object) : INode
		{
			if (value is INode)
				return value as INode;
			
			if (value is SValue)
				return (value as SValue)._node;
			
			var c	: Constant	= new Constant();
			
			if (value is int || value is uint || value is Number)
			{
				c.constants[0] = value as Number;
			}
			if (value is Point)
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
			
			if (!c)
				throw new Error("Constants can only be int, uint, Number, Point, Vector4 or Matrix4x4 values.");
			
			return c;
		}
	}
}