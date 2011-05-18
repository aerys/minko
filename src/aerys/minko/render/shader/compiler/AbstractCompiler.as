package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.allocator.AttributeAllocator;
	import aerys.minko.render.shader.compiler.allocator.ConstantAllocator;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.compiler.allocator.VaryingAllocator;
	import aerys.minko.render.shader.compiler.register.RegistryLimit;
	import aerys.minko.render.shader.compiler.visitor.allocator.FragmentAllocator;
	import aerys.minko.render.shader.compiler.visitor.allocator.VertexAllocator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.ConstantDuplicator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.DummyRemover;
	import aerys.minko.render.shader.compiler.visitor.preprocess.Merger;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteDot;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.RootWrapper;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class AbstractCompiler
	{
		// Shaders 
		
		protected var _vertexOps		: Vector.<INode>;
		protected var _colorNode		: INode;
		
		// Allocators
		
		protected var _attrAllocator	: AttributeAllocator;
		protected var _varyingAllocator	: VaryingAllocator;
		protected var _vsTmpAllocator	: Allocator;
		protected var _vsConstAllocator	: ConstantAllocator;
		protected var _fsTmpAllocator	: Allocator;
		protected var _fsConstAllocator	: ConstantAllocator;
		
		// Additional data that will be needed for shader usage
		
		protected var _vertexInput		: Vector.<VertexComponent>;
		protected var _vsConstData		: Vector.<Number>;
		protected var _fsConstData		: Vector.<Number>;
		protected var _vsParams			: Vector.<ParameterAllocation>;
		protected var _fsParams			: Vector.<ParameterAllocation>;
		protected var _samplers			: Vector.<String>;
		
		public function get vertexInput() : Vector.<VertexComponent>
		{
			return _vertexInput;
		}
		
		public function get vsConstData() : Vector.<Number>
		{
			return _vsConstData;
		}
		
		public function get fsConstData() : Vector.<Number>
		{
			return _fsConstData;
		}
		
		public function get vsParams() : Vector.<ParameterAllocation>
		{
			return _vsParams;
		}
		
		public function get fsParams() : Vector.<ParameterAllocation>
		{
			return _fsParams;
		}
		
		public function get samplers() : Vector.<String>
		{
			return _samplers;
		}
		
		public function AbstractCompiler()
		{
		}
		
		public function compile(clipspacePos	: INode,
								color			: INode) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function reset() : void 
		{
			_vertexInput = null;
			
			_vertexOps	= new Vector.<INode>();
			_vertexInput = new Vector.<VertexComponent>();
			_colorNode	= null;
			
			_attrAllocator		= new AttributeAllocator(RegistryLimit.VS_MAX_ATTRIBUTE);
			_varyingAllocator	= new VaryingAllocator(RegistryLimit.MAX_VARYING);
			_vsTmpAllocator		= new Allocator(RegistryLimit.VS_MAX_TEMPORARY);
			_vsConstAllocator	= new ConstantAllocator(RegistryLimit.VS_MAX_CONSTANT);
			_fsTmpAllocator		= new Allocator(RegistryLimit.FG_MAX_TEMPORARY);
			_fsConstAllocator	= new ConstantAllocator(RegistryLimit.FG_MAX_CONSTANT);
		}
		
		protected function prepare(clipspacePos : INode, color : INode):void
		{
			// A combine node cannot be root for vertex and fragment shader, nor can an interpolate node for color
			
//			if (clipspacePos is Combine)
				clipspacePos = new RootWrapper(clipspacePos);
			
//			if (color is Combine || color is Interpolate)
				color = new RootWrapper(color);
				
			
				var writeDot : WriteDot = new WriteDot();
				writeDot.processShader(clipspacePos, color);
				trace('-------- blabla shader ----------');
				trace(writeDot.dot);
				
				
			// preprocess
			
			var dummyRemover : DummyRemover = new DummyRemover();
			dummyRemover.processShader(clipspacePos, color);
			clipspacePos	= dummyRemover.clipspacePos;
			color			= dummyRemover.color;
			
			new Merger().processShader(clipspacePos, color);
			new ConstantDuplicator().processShader(clipspacePos, color);
			
			
																writeDot = new WriteDot();
																writeDot.processShader(clipspacePos, color);
																trace('-------- preprocessed shader ----------');
																trace(writeDot.dot);
			
			// Split vertex and fragment shader, report registry usage to allocators
			var vertexAllocator	: VertexAllocator = 
				new VertexAllocator(_attrAllocator, _vsTmpAllocator, _vsConstAllocator);
			
			var fragmentAllocator	: FragmentAllocator = 
				new FragmentAllocator(_fsTmpAllocator, _varyingAllocator, _fsConstAllocator);
			
			fragmentAllocator.processFragmentShader(color);
			
			_colorNode	= color;
			_samplers	= fragmentAllocator.samplers;
			_vertexOps	= fragmentAllocator.vertexShaderOutputs;
			_vertexOps.unshift(clipspacePos);
			
			vertexAllocator.processVertexShader(_vertexOps);
			
			// Allocate all registries on both shaders (va, vt, vc, v, ft, fs, fc)
			_attrAllocator.computeRegisterState(true);
			_varyingAllocator.computeRegisterState(true);
			_vsTmpAllocator.computeRegisterState();
			_vsConstAllocator.computeRegisterState();
			_fsTmpAllocator.computeRegisterState();
			_fsConstAllocator.computeRegisterState();
			
			// Compute additional data
			var a : Array = _attrAllocator.getAllocations();
			for (var i:int = 0; i < a.length; ++i)
				_vertexInput.push(Attribute(a[i]).vertexComponent);
			
			_vsConstData	= _vsConstAllocator.computeConstantAllocation();
			_fsConstData	= _fsConstAllocator.computeConstantAllocation();
			_vsParams		= _vsConstAllocator.computeParameterAllocation();
			_fsParams		= _fsConstAllocator.computeParameterAllocation();
		}
	}
}
