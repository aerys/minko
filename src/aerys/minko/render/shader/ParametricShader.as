package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.geom.Point;
	import flash.net.getClassByAlias;
	import flash.utils.Dictionary;
	
	public class ParametricShader
	{
		use namespace minko;
		
		private var _shadersMap		: Object	= new Object();
		
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
		
		protected final function get localToScreenMatrix() : INode
		{
			return new TransformParameter(16, TransformData.LOCAL_TO_SCREEN);
		}
		
		public function fillRenderState(state	: RenderState, 
										style	: StyleStack, 
										local	: TransformData, 
										world	: Dictionary) : Boolean
		{
			var hash 	: String 		= getDataHash(style, local, world);
			var shader 	: DynamicShader = _shadersMap[hash];
			
			if (!shader)
				_shadersMap[hash] = shader = DynamicShader.create(getOutputPosition(style, local, world),
																  getOutputColor(style, local, world));
			
			shader.fillRenderState(state, style, local, world);
			
			return true;
		}
		
		protected function getDataHash(style	: StyleStack, 
									   local	: TransformData, 
									   world	: Dictionary) : String
		{
			return "";
		}
		
		protected function getOutputPosition(style	: StyleStack, 
											 local	: TransformData, 
											 world	: Dictionary) : INode
		{
			throw new Error();
		}
		
		protected function getOutputColor(style	: StyleStack, 
										  local	: TransformData, 
										  world	: Dictionary) : INode
		{
			throw new Error();
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
		
		protected final function sampleTexture(styleName : String, uv : Object) : Texture
		{
			return new Texture(getShaderNode(uv), new Sampler(styleName));
		}
		
		protected final function multiply(value1 : Object, value2 : Object) : INode
		{
			return new Multiply(getShaderNode(value1), getShaderNode(value2));
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
		
		protected final function extract(value : Object, Component : uint) : INode
		{
			return new Extract(getShaderNode(value), Component);
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