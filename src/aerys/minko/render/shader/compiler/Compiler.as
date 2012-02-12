package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.visitors.AllocationVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.CopyInserterVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.InterpolateFinder;
	import aerys.minko.render.shader.compiler.graph.visitors.MatrixTransformationGrouper;
	import aerys.minko.render.shader.compiler.graph.visitors.MergeVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.OverwriterCleanerVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.RemoveExtractsVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.RemoveUselessComputation;
	import aerys.minko.render.shader.compiler.graph.visitors.ResolveConstantComputationVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.ResolveParametrizedComputationVisitor;
	import aerys.minko.render.shader.compiler.graph.visitors.WriteDot;
	import aerys.minko.render.shader.compiler.sequence.AgalInstruction;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class Compiler
	{
		public static const COMPUTE_CONSTANTS_IN_CPU					: uint = 1;
		public static const BUBBLE_UP_INTERPOLATES						: uint = 2;
		public static const RAISE_FS_CONSTANTS_LIMIT_BY_INTERPOLATING	: uint = 4;
		
		private static const REMOVE_EXTRACT			: RemoveExtractsVisitor					= new RemoveExtractsVisitor();
		private static const MERGER					: MergeVisitor							= new MergeVisitor();
		private static const OVERWRITER_CLEANER		: OverwriterCleanerVisitor				= new OverwriterCleanerVisitor();
		private static const RESOLVE_CONSTANT		: ResolveConstantComputationVisitor		= new ResolveConstantComputationVisitor();
		private static const RESOLVE_PARAMETRIZED	: ResolveParametrizedComputationVisitor	= new ResolveParametrizedComputationVisitor();
		private static const REMOVE_USELESS			: RemoveUselessComputation				= new RemoveUselessComputation();
		private static const COPY_INSERTER			: CopyInserterVisitor					= new CopyInserterVisitor();
		private static const ALLOCATOR				: AllocationVisitor						= new AllocationVisitor();
		private static const INTERPOLATE_FINDER		: InterpolateFinder						= new InterpolateFinder();
		private static const WRITE_DOT				: WriteDot								= new WriteDot();
		private static const MATRIX_TRANSFORMATION	: MatrixTransformationGrouper			= new MatrixTransformationGrouper();
		
		
		private static var _shaderGraph			: ShaderGraph;
		
		private static var _vertexSequence		: Vector.<AgalInstruction>;
		private static var _fragmentSequence	: Vector.<AgalInstruction>;
		private static var _bindings			: Object;
		
		private static var _vertexComponents	: Vector.<VertexComponent>;
		private static var _vertexIndices		: Vector.<uint>;
		private static var _vsConstants			: Vector.<Number>;
		private static var _fsConstants			: Vector.<Number>;
		private static var _textures			: Vector.<ITextureResource>;
		
		public static function load(shaderGraph	: ShaderGraph,
									flags		: uint) : void
		{
			
			// execute consecutive visitors to optimize the shader graph.
			REMOVE_EXTRACT			.process(shaderGraph);
			MERGER					.process(shaderGraph);
			
//			OVERWRITER_CLEANER		.process(shaderGraph);
//			RESOLVE_CONSTANT		.process(shaderGraph);
//			REMOVE_USELESS			.process(shaderGraph);
			//RESOLVE_PARAMETRIZED	.process(shaderGraph);
			COPY_INSERTER			.process(shaderGraph);
			
//			MATRIX_TRANSFORMATION	.process(shaderGraph);
			
			// generate final program
			INTERPOLATE_FINDER.process(shaderGraph);
			ALLOCATOR.process(shaderGraph);
			
			// retrieve program
			_vertexSequence		= ALLOCATOR.vertexSequence;
			_fragmentSequence	= ALLOCATOR.fragmentSequence;
			_bindings			= ALLOCATOR.parameterBindings;
			_vertexComponents	= ALLOCATOR.vertexComponents;
			_vertexIndices		= ALLOCATOR.vertexIndices;
			_vsConstants		= ALLOCATOR.vertexConstants;
			_fsConstants		= ALLOCATOR.fragmentConstants;
			_textures			= ALLOCATOR.textures;
		}
		
		public static function compileShader(name : String) : Program3DResource
		{
			trace(compileStringShader());
//			trace('vs', _vsConstants);
//			trace('fs', _fsConstants);
			
			var vsProgram	: ByteArray = computeBinaryProgram(_vertexSequence, true);
			var fsProgram	: ByteArray = computeBinaryProgram(_fragmentSequence, false);
			
			var program : Program3DResource = new Program3DResource(
				name,
				vsProgram,
				fsProgram,
				_vertexComponents,
				_vertexIndices,
				_vsConstants,
				_fsConstants,
				_textures,
				_bindings
			);
			
			return program;
		}
		
		public static function compileStringShader() : String
		{
			var finalShader : String = '';
			
			finalShader += "---------------- vertex shader ----------------\n";
			finalShader += computeStringProgram(_vertexSequence, true);
			finalShader += "\n";
			finalShader += "--------------- fragment shader ---------------\n";
			finalShader += computeStringProgram(_fragmentSequence, false);
			
			return finalShader;
		}
		
		private static function computeBinaryProgram(sequence		: Vector.<AgalInstruction>,
													 isVertexShader	: Boolean) : ByteArray
		{
			var program : ByteArray = new ByteArray();
			
			program.endian	= Endian.LITTLE_ENDIAN;
			program.writeByte(0xa0);					// tag version
			program.writeUnsignedInt(1);				// AGAL version, big endian, bit pattern will be 0x01000000
			program.writeByte(0xa1);					// tag program id
			program.writeByte(isVertexShader ? 0 : 1);	// vertex or fragment			
			
			for each (var instruction : AgalInstruction in sequence)
				instruction.getByteCode(program);
			
			return program;
		}
		
		private static function computeStringProgram(sequence		: Vector.<AgalInstruction>,
													 isVertexShader	: Boolean) : String
		{
			var shader : String = '';
			
			for each (var instruction : AgalInstruction in sequence)
				shader += instruction.getAgal(isVertexShader);
			
			return shader;
		}
		
	}
}