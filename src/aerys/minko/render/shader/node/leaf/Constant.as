package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	
	public class Constant extends AbstractConstant
	{
		// stockages des constantes en question
		protected var _constants:Vector.<Number>;
		
		override public function get size():uint 
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
		
		public function Constant(...constants)
		{
			super('Constant' + constants.length);
			
			_constants = Vector.<Number>(constants);
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
