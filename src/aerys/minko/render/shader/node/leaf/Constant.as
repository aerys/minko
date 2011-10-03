package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Point;
	
	public class Constant extends AbstractConstant
	{
		// stockages des constantes en question
		protected var _constants	: Vector.<Number>	= new Vector.<Number>();
		
		override public function get size() : uint 
		{ 
			return _constants.length; 
		}
		
		public function get constants() : Vector.<Number> 
		{ 
			return _constants; 
		}
		
		public function set constants(value : Vector.<Number>) : void 
		{ 
			_constants = value; 
		}
		
		public function Constant(...values)
		{
			super('Constant' + values.length);
			
			serialize(values);
		}
		
		private function serialize(values : Object, offset : int = 0) : int
		{
			for each (var value : Object in values)
			{
				if (value is Number)
				{
					_constants[int(offset++)] = value as Number;
				}
				else if (value is Point)
				{
					var point	: Point	= value as Point;
					
					_constants[int(offset++)] = point.x;
					_constants[int(offset++)] = point.y;
					// force modulo 4 alignment because variadic extract
					// does not handle size !E {4, 16} (#20)
					_constants[int(offset++)] = 0.;
					_constants[int(offset++)] = 0.;
				}
				else if (value is Vector4)
				{
					var vector 	: Vector4 	= value as Vector4;
					
					_constants[int(offset++)] = vector.x;
					_constants[int(offset++)] = vector.y;
					_constants[int(offset++)] = vector.z;
					_constants[int(offset++)] = isNaN(vector.w) ? 0. : vector.w;
				}
				else if (value is Matrix4x4)
				{
					(value as Matrix4x4).getRawData(_constants, offset);
				}
				else if (value is Vector.<int> || value is Vector.<uint>
						 || value is Vector.<Number> || value is Vector.<Point>
						 || value is Vector.<Vector4> || value is Vector.<ConstVector4>
						 || value is Vector.<Matrix4x4> || value is Array)
				{
					offset = serialize(value, offset);
				}
				else
				{
					throw new Error("Constants can only be (vector or array of) int,"
									+ " uint, Number, Point, Vector4 or Matrix3D values.");
				}
			}
			
			return offset;
		}
		
		override public function isSame(otherNode : INode) : Boolean
		{
			var constOtherNode : Constant = otherNode as Constant;
			
			if (constOtherNode == null)
				return false;
			
			var length : uint = _constants.length;
			
			if (length != constOtherNode._constants.length)
				return false;
			
			for (var i : int = 0; i < length; ++i)
				if (_constants[i] != constOtherNode._constants[i])
					return false;
			
			return true;
		}
		
		override public function clone() : AbstractConstant
		{
			var newConstant:Constant = new Constant();
			
			newConstant._constants = _constants;
			newConstant._name = _name;
			
			return newConstant;
		}
		
		override public function toString() : String
		{
			return "Constant\\n(" + _constants.join() + ")";
		}
	}
}
