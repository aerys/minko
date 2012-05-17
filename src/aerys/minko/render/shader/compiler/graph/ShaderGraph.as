package aerys.minko.render.shader.compiler.graph
{
	import aerys.minko.Minko;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.Signature;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	import aerys.minko.render.shader.compiler.graph.visitors.*;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.sequence.AgalInstruction;
	import aerys.minko.type.log.DebugLevel;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.Endian;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class ShaderGraph
	{
		private static const SPLITTER				: SplitterVisitor						= new SplitterVisitor();
		private static const REMOVE_EXTRACT			: RemoveExtractsVisitor					= new RemoveExtractsVisitor();
		private static const MERGER					: MergeVisitor							= new MergeVisitor();
		private static const OVERWRITER_CLEANER		: OverwriterCleanerVisitor				= new OverwriterCleanerVisitor();
		private static const RESOLVE_CONSTANT		: ResolveConstantComputationVisitor		= new ResolveConstantComputationVisitor();
		private static const CONSTANT_PACKER		: ConstantPackerVisitor					= new ConstantPackerVisitor();
		private static const CONSTANT_GROUPER		: ConstantGrouperVisitor				= new ConstantGrouperVisitor();
		private static const RESOLVE_PARAMETRIZED	: ResolveParametrizedComputationVisitor	= new ResolveParametrizedComputationVisitor();
		private static const REMOVE_USELESS			: RemoveUselessComputation				= new RemoveUselessComputation();
		private static const COPY_INSERTER			: CopyInserterVisitor					= new CopyInserterVisitor();
		private static const ALLOCATOR				: AllocationVisitor						= new AllocationVisitor();
		private static const INTERPOLATE_FINDER		: InterpolateFinder						= new InterpolateFinder();
		private static const WRITE_DOT				: WriteDot								= new WriteDot();
		private static const MATRIX_TRANSFORMATION	: MatrixTransformationGrouper			= new MatrixTransformationGrouper();
		
		private var _position				: AbstractNode;
		private var _positionComponents		: uint;
		private var _interpolates			: Vector.<AbstractNode>;
		private var _color					: AbstractNode;
		private var _colorComponents		: uint;
		private var _kills					: Vector.<AbstractNode>;
		private var _killComponents			: Vector.<uint>;
		private var _computableConstants	: Object;
		
		private var _isCompiled				: Boolean;
		
		private var _vertexSequence			: Vector.<AgalInstruction>;
		private var _fragmentSequence		: Vector.<AgalInstruction>;
		private var _bindings				: Object;
		
		private var _vertexComponents		: Vector.<VertexComponent>;
		private var _vertexIndices			: Vector.<uint>;
		private var _vsConstants			: Vector.<Number>;
		private var _fsConstants			: Vector.<Number>;
		private var _textures				: Vector.<ITextureResource>;
		
		public function get position() : AbstractNode
		{
			return _position;
		}
		public function set position(v : AbstractNode)	: void
		{
			_position = v;
		}
		
		public function get interpolates() : Vector.<AbstractNode>
		{
			return _interpolates;
		}
		
		public function get color() : AbstractNode
		{
			return _color;
		}
		public function set color(v : AbstractNode)	: void
		{
			_color = v;
		}
		
		public function get kills() : Vector.<AbstractNode>
		{
			return _kills;
		}
		
		public function get positionComponents() : uint
		{
			return _positionComponents;
		}
		public function set positionComponents(v : uint) : void
		{
			_positionComponents = v;
		}
		
		public function get colorComponents() : uint
		{
			return _colorComponents;
		}
		public function set colorComponents(v : uint) : void
		{
			_colorComponents = v;
		}
		
		public function get killComponents() : Vector.<uint>
		{
			return _killComponents;
		}
		
		public function get computableConstants() : Object
		{
			return _computableConstants;
		}
		
		public function ShaderGraph(position	: AbstractNode,
									color		: AbstractNode,
									kills		: Vector.<AbstractNode>)
		{
			_isCompiled				= false;
			
			_position				= position;
			_positionComponents		= Components.createContinuous(0, 0, 4, position.size);
			_interpolates			= new Vector.<AbstractNode>();
			_color					= color;
			_colorComponents		= Components.createContinuous(0, 0, 4, color.size);
			_kills					= kills;
			_killComponents			= new Vector.<uint>();
			_computableConstants	= new Object();
			
			var numKills : uint = kills.length;
			for (var killId : uint = 0; killId < numKills; ++killId)
				_killComponents[killId] = Components.createContinuous(0, 0, 1, 1);
		}
		
		public function generateProgram(name : String, signature : Signature) : Program3DResource
		{
			if (!_isCompiled)
				compile();
			
			if (Minko.debugLevel & DebugLevel.SHADER_AGAL)
				Minko.log(DebugLevel.SHADER_AGAL, generateAGAL(name));
			
			var vsProgram	: ByteArray = computeBinaryProgram(_vertexSequence, true);
			var fsProgram	: ByteArray = computeBinaryProgram(_fragmentSequence, false);
			
			var program : Program3DResource = new Program3DResource(
				name,
				signature,
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
		
		public function generateAGAL(name : String) : String
		{
			var instruction	: AgalInstruction;
			var shader		: String = name + "\n";
			
			if (!_isCompiled)
				compile();
			
			shader += "- vertex shader\n";
			for each (instruction in _vertexSequence)
				shader += instruction.getAgal(true);
			
			shader += "- fragment shader\n";
			for each (instruction in _fragmentSequence)
				shader += instruction.getAgal(false);
			
			return shader;
		}
		
		private function compile() : void
		{
			// execute consecutive visitors to optimize the shader graph.
			// Warning: the order matters, do not swap lines.
			// log shader in dotty format
			MERGER					.process(this);	// merge duplicate nodes
			REMOVE_EXTRACT			.process(this);	// remove all extract nodes
			OVERWRITER_CLEANER		.process(this);	// remove nested overwriters
			RESOLVE_CONSTANT		.process(this);	// resolve constant computation
			CONSTANT_PACKER			.process(this);	// pack constants [0,0,0,1] => [0,1].xxxy
			REMOVE_USELESS			.process(this);	// remove some useless operations (add 0, mul 0, mul 1...)
			RESOLVE_PARAMETRIZED	.process(this);	// replace computations that depend on parameters by evalexp parameters
//			MATRIX_TRANSFORMATION	.process(this);	// replace ((vector * matrix1) * matrix2) by vector * (matrix1 * matrix2) to save registers on GPU
			COPY_INSERTER			.process(this);	// ensure there are no operations between constants
			SPLITTER				.process(this);	// clone nodes that are shared between vertex and fragment shader
			CONSTANT_GROUPER		.process(this);	// group constants [0,1] & [0,2] => [0, 1, 2]
			
			if (Minko.debugLevel & DebugLevel.SHADER_DOTTY)
			{
				WRITE_DOT.process(this);
				Minko.log(DebugLevel.SHADER_DOTTY, WRITE_DOT.result);
				WRITE_DOT.clear();
			}
			
			// generate final program
			INTERPOLATE_FINDER		.process(this);	// find interpolate nodes. We may skip that in the future.
			ALLOCATOR				.process(this);	// allocate memory and generate final code.
			
			// retrieve program
			_vertexSequence		= ALLOCATOR.vertexSequence;
			_fragmentSequence	= ALLOCATOR.fragmentSequence;
			_bindings			= ALLOCATOR.parameterBindings;
			_vertexComponents	= ALLOCATOR.vertexComponents;
			_vertexIndices		= ALLOCATOR.vertexIndices;
			_vsConstants		= ALLOCATOR.vertexConstants;
			_fsConstants		= ALLOCATOR.fragmentConstants;
			_textures			= ALLOCATOR.textures;
			
			ALLOCATOR.clear();
			
			_isCompiled = true;
		}
		
		private function computeBinaryProgram(sequence			: Vector.<AgalInstruction>,
											  isVertexShader	: Boolean) : ByteArray
		{
			var program : ByteArray = new ByteArray();
			
			program.endian = Endian.LITTLE_ENDIAN;
			program.writeByte(0xa0);					// tag version
			program.writeUnsignedInt(1);				// AGAL version, big endian, bit pattern will be 0x01000000
			program.writeByte(0xa1);					// tag program id
			program.writeByte(isVertexShader ? 0 : 1);	// vertex or fragment			
			
			for each (var instruction : AgalInstruction in sequence)
				instruction.getBytecode(program);
			
			return program;
		}
	}
}
