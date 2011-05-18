package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.visitor.writer.WriteAgal;
	import aerys.minko.render.shader.node.INode;

	public class AgalCompiler extends AbstractCompiler
	{
		protected var _vertexShaderAgal		: String;
		protected var _fragmentShaderAgal	: String;
		
		public function AgalCompiler()
		{
			super();
		}
		
		public function get fragmentShaderAgal():String
		{
			return _fragmentShaderAgal;
		}

		public function get vertexShaderAgal():String
		{
			return _vertexShaderAgal;
		}
		
		override protected function reset():void
		{
			super.reset();
			
			_vertexShaderAgal	= '';
			_fragmentShaderAgal	= '';
		}
		
		override public function compile(clipspacePos	: INode,
										 color			: INode) : void
		{
			reset();
			prepare(clipspacePos, color);
			
			var writeAssembly : WriteAgal = new WriteAgal(
				_attrAllocator,
				_fsTmpAllocator, _varyingAllocator, _fsConstAllocator, 
				_vsTmpAllocator, _vsConstAllocator);
			
			_vertexShaderAgal	= writeAssembly.processVertexShader(_vertexOps);
			_fragmentShaderAgal	= writeAssembly.processFragmentShader(_colorNode);
		}
	}
}
