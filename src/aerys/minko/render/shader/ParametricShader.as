package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.fog.Fog;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.SquareRoot;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.manipulation.Blend;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.math.Product;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.type.math.Matrix4x4;
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
		
		protected final function get vertexClipspacePosition() : INode
		{
			return multiply4x4(vertexPosition, localToScreenMatrix);
		}
		
		protected final function get vertexPosition() : INode
		{
			return new Attribute(VertexComponent.XYZ);
		}
		
		protected final function get vertexColor() : INode
		{
			return new Attribute(VertexComponent.RGB);
		}
		
		protected final function get vertexUV() : INode
		{
			return new Attribute(VertexComponent.UV);
		}
		
		protected final function get cameraLocalDirection() : INode
		{
			return new WorldParameter(3, CameraData, CameraData.LOCAL_DIRECTION);
		}
		
		protected final function get cameraLocalPosition() : INode
		{
			return new WorldParameter(3, CameraData, CameraData.LOCAL_POSITION);
		}
		
		protected final function get localToScreenMatrix() : INode
		{
			return new TransformParameter(16, LocalData.LOCAL_TO_SCREEN);
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
				_shadersMap[hash] = shader = DynamicShader.create(getOutputPosition(),
																  getOutputColor());
			
			shader.fillRenderState(state, style, local, world);
			
			return true;
		}
		
		protected function getDataHash(style	: StyleStack, 
									   local	: LocalData, 
									   world	: Dictionary) : String
		{
			return "";
		}
		
		protected function getOutputPosition() : INode
		{
			throw new Error();
		}
		
		protected function getOutputColor() : INode
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
		
		protected final function interpolate(value : INode) : INode
		{
			return new Interpolate(value);
		}
		
		protected final function combine(value1	: Object,
										 value2	: Object) : INode
		{
			return new Combine(getShaderNode(value1), getShaderNode(value2));
		}
		
		protected final function sampleTexture(styleId : int, uv : Object) : Texture
		{
			return new Texture(getShaderNode(uv), new Sampler(styleId));
		}
		
		protected final function multiply(arg1 : Object, arg2 : Object, ...args) : INode
		{
			var p 		: Product 	= new Product(getShaderNode(arg1), getShaderNode(arg2));
			var numArgs : int 		= args.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				p.addTerm(getShaderNode(args[i]))
			
			return p;
		}
		
		protected final function divide(arg1 : Object, arg2 : Object) : INode
		{
			return new Divide(getShaderNode(arg1), getShaderNode(arg2));
		}
		
		protected final function power(base : Object, exp : Object) : INode
		{
			return new Power(getShaderNode(base), getShaderNode(exp));
		}
		
		protected final function add(value1 : Object, value2 : Object) : INode
		{
			return new Add(getShaderNode(value1), getShaderNode(value2));
		}
		
		protected final function substract(value1 : Object, value2 : Object) : INode
		{
			return new Substract(getShaderNode(value1), getShaderNode(value2));
		}
		
		protected final function dotProduct3(u : Object, v : Object) : INode
		{
			return new DotProduct3(getShaderNode(u), getShaderNode(v));
		}
		
		protected final function dotProduct4(u : Object, v : Object) : INode
		{
			return new DotProduct4(getShaderNode(u), getShaderNode(v));
		}
		
		protected final function multiply4x4(a : Object, b : Object) : INode
		{
			return new Multiply4x4(getShaderNode(a), getShaderNode(b));
		}
		
		protected final function getWorldParameter(size		: uint, 
												   key		: Class,
												   field	: String	= null, 
												   index	: int		= -1) : INode
		{
			return new WorldParameter(size, key, field, index);
		}
		
		protected final function getStyleParameter(size 	: uint,
												   key 		: int,
												   field 	: String 	= null,
												   index 	: int 		= -1) : INode
		{
			return new StyleParameter(size, key, field, index);
		}
		
		protected final function getConstant(value : Object) : INode
		{
			return getShaderNode(value);
		}
		
		protected final function extract(value : Object, Component : uint) : INode
		{
			return new Extract(getShaderNode(value), Component);
		}
		
		protected final function blend(color1 : Object, color2 : Object, blending : uint) : INode
		{
			return new Blend(getShaderNode(color1), getShaderNode(color2), blending);
		}
		
		protected final function vector3Length(vector3 : Object) : INode
		{
			var v : INode = getShaderNode(vector3);
			
			return sqrt(dotProduct3(v, v));
		}
		
		protected final function sqrt(scalar : Object) : INode
		{
			return new SquareRoot(getShaderNode(scalar));
		}
		
		protected final function getFolorColor(start	: Object,
											   distance	: Object,
											   color	: Object) : INode
		{
			return new Fog(getShaderNode(start), getShaderNode(distance), getShaderNode(color));
		}
		
		protected final function getVertexAttribute(vertexComponent : VertexComponent) : INode
		{
			return new Attribute(vertexComponent);
		}
		
		private function getShaderNode(value : Object) : INode
		{
			if (value is INode)
				return value as INode;
			
			var c	: Constant	= new Constant();
			
			if (value is int || value is Number)
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