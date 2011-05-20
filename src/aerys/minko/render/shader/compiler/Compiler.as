package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.DynamicShader;
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.allocator.AttributeAllocator;
	import aerys.minko.render.shader.compiler.allocator.ConstantAllocator;
	import aerys.minko.render.shader.compiler.allocator.ParameterAllocation;
	import aerys.minko.render.shader.compiler.allocator.VaryingAllocator;
	import aerys.minko.render.shader.compiler.register.RegistryLimit;
	import aerys.minko.render.shader.compiler.visitor.allocator.FragmentAllocator;
	import aerys.minko.render.shader.compiler.visitor.allocator.VertexAllocator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.ConstantDuplicator;
	import aerys.minko.render.shader.compiler.visitor.preprocess.DebugVisitor;
	import aerys.minko.render.shader.compiler.visitor.preprocess.DummyRemover;
	import aerys.minko.render.shader.compiler.visitor.preprocess.Merger;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteAgal;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteByteCode;
	import aerys.minko.render.shader.compiler.visitor.writer.WriteDot;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.RootWrapper;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.utils.ByteArray;
	
	public class Compiler
	{
		// Nodes
		protected var _clipspacePosNode	: INode;
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
		
		public function Compiler()
		{
		}
		
		public function load(clipspacePos	: INode, 
							 color			: INode) : void
		{
			reset();
			
			_clipspacePosNode	= clipspacePos;
			_colorNode			= color;
			
			wrapRootNodes();
			removeDummyNodes();
			
			preprocess();
			splitAndReportMemoryUsage();
			allocateRegistries();
			createAllocationTables();
		}
		
		protected function reset() : void
		{
			_vertexInput		= null;
			
			_vertexOps			= new Vector.<INode>();
			_vertexInput		= new Vector.<VertexComponent>();
			_colorNode			= null;
			
			_attrAllocator		= new AttributeAllocator(RegistryLimit.VS_MAX_ATTRIBUTE);
			_varyingAllocator	= new VaryingAllocator(RegistryLimit.MAX_VARYING);
			_vsTmpAllocator		= new Allocator(RegistryLimit.VS_MAX_TEMPORARY);
			_vsConstAllocator	= new ConstantAllocator(RegistryLimit.VS_MAX_CONSTANT);
			_fsTmpAllocator		= new Allocator(RegistryLimit.FG_MAX_TEMPORARY);
			_fsConstAllocator	= new ConstantAllocator(RegistryLimit.FG_MAX_CONSTANT);
		}
		
		protected function wrapRootNodes() : void
		{
			_clipspacePosNode	= new RootWrapper(_clipspacePosNode);
			_colorNode			= new RootWrapper(_colorNode);
		}
		
		protected function removeDummyNodes() : void
		{
			var dummyRemover : DummyRemover = new DummyRemover();
			dummyRemover.processShader(_clipspacePosNode, _colorNode);
			_clipspacePosNode	= dummyRemover.clipspacePos;
			_colorNode			= dummyRemover.color;
		}
		
		protected function preprocess() : void
		{
			new Merger().processShader(_clipspacePosNode, _colorNode);
			new ConstantDuplicator().processShader(_clipspacePosNode, _colorNode);
		}
		
		protected function splitAndReportMemoryUsage() : void
		{
			// Split vertex and fragment shader, report registry usage to allocators
			var vertexAllocator	: VertexAllocator = 
				new VertexAllocator(_attrAllocator, _vsTmpAllocator, _vsConstAllocator);
			
			var fragmentAllocator	: FragmentAllocator = 
				new FragmentAllocator(_fsTmpAllocator, _varyingAllocator, _fsConstAllocator);
			
			fragmentAllocator.processFragmentShader(_colorNode);
			
			_samplers	= fragmentAllocator.samplers;
			_vertexOps	= fragmentAllocator.vertexShaderOutputs;
			_vertexOps.unshift(_clipspacePosNode);
			
			vertexAllocator.processVertexShader(_vertexOps);
		}
		
		protected function allocateRegistries() : void
		{
			// Allocate all registries on both shaders (va, vt, vc, v, ft, fs, fc)
			_attrAllocator		.computeRegisterState(true);
			_varyingAllocator	.computeRegisterState(true);
			_vsTmpAllocator		.computeRegisterState(false);
			_vsConstAllocator	.computeRegisterState(false);
			_fsTmpAllocator		.computeRegisterState(false);
			_fsConstAllocator	.computeRegisterState(false);
		}
		
		protected function createAllocationTables() : void
		{
			var a : Array = _attrAllocator.getAllocations();
			for (var i:int = 0; i < a.length; ++i)
				_vertexInput.push(Attribute(a[i]).vertexComponent);
			
			_vsConstData	= _vsConstAllocator.computeConstantAllocation();
			_fsConstData	= _fsConstAllocator.computeConstantAllocation();
			_vsParams		= _vsConstAllocator.computeParameterAllocation();
			_fsParams		= _fsConstAllocator.computeParameterAllocation();
		}
		
		public function compileShader() : DynamicShader
		{
			var vertexShader	: ByteArray = compileVertexShader();
			var fragmentShader	: ByteArray	= compileFragmentShader();
			
			return new DynamicShader(
				vertexShader, fragmentShader, _vertexInput, 
				_vsConstData, _fsConstData, _vsParams, 
				_fsParams, _samplers
			);
		}
		
		protected function compileVertexShader() : ByteArray
		{
			return new WriteByteCode(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processVertexShader(_vertexOps);
		}
		
		protected function compileFragmentShader() : ByteArray
		{
			return new WriteByteCode(
				_attrAllocator, _fsTmpAllocator, _varyingAllocator,
				_fsConstAllocator, _vsTmpAllocator, _vsConstAllocator
			).processFragmentShader(_colorNode);
		}

	}
}
