package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.visitor.writer.WriteAgal;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteByteCode;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteDot;
	import aerys.minko.render.shader.node.INode;
	
	import flash.utils.ByteArray;

	public class ByteCodeCompiler extends AbstractCompiler
	{
		protected var _vertexShader		: ByteArray;
		protected var _fragmentShader	: ByteArray;
		
		public function ByteCodeCompiler()
		{
			super();
		}
		
		public function get fragmentShader():ByteArray
		{
			return _fragmentShader;
		}

		public function get vertexShader():ByteArray
		{
			return _vertexShader;
		}

		override protected function reset():void
		{
			super.reset();
			
			_vertexShader	= new ByteArray();
			_fragmentShader	= new ByteArray();
		}
		
		override public function compile(clipspacePos	: INode,
										 color			: INode) : void
		{
			reset();
			
//			var writeDot : WriteDot = new WriteDot();
//			writeDot.processShader(clipspacePos, color);
//			trace('-------- preprocessed shader ----------');
//			trace(writeDot.dot);
			
			prepare(clipspacePos, color);
			
			var writeByteCode : WriteByteCode = new WriteByteCode(
				_attrAllocator,
				_fsTmpAllocator, _varyingAllocator, _fsConstAllocator, 
				_vsTmpAllocator, _vsConstAllocator);
			
//			writeDot.processShader(clipspacePos, color);
//			trace('-------- processed shader ----------');
//			trace(writeDot.dot);
//			trace('=====================================');
			
			_vertexShader = writeByteCode.processVertexShader(_vertexOps);
			_fragmentShader = writeByteCode.processFragmentShader(_colorNode);
		}
	}
}
