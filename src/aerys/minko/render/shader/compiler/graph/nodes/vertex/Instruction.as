package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.InstructionFlag;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 */
	public class Instruction extends AbstractNode
	{
		public static const MOV : uint = 0x00;
		public static const ADD : uint = 0x01;
		public static const SUB : uint = 0x02;
		public static const MUL : uint = 0x03;
		public static const DIV : uint = 0x04;
		public static const RCP : uint = 0x05;
		public static const MIN : uint = 0x06;
		public static const MAX : uint = 0x07;
		public static const FRC : uint = 0x08;
		public static const SQT : uint = 0x09;
		public static const RSQ : uint = 0x0a;
		public static const POW : uint = 0x0b;
		public static const LOG : uint = 0x0c;
		public static const EXP : uint = 0x0d;
		public static const NRM : uint = 0x0e;
		public static const SIN : uint = 0x0f;
		public static const COS : uint = 0x10;
		public static const CRS : uint = 0x11;
		public static const DP3 : uint = 0x12;
		public static const DP4 : uint = 0x13;
		public static const ABS : uint = 0x14;
		public static const NEG : uint = 0x15;
		public static const SAT : uint = 0x16;
		public static const M33 : uint = 0x17;
		public static const M44 : uint = 0x18;
		public static const M34 : uint = 0x19;
		public static const KIL	: uint = 0x27;
		public static const TEX : uint = 0x28;
		public static const SGE : uint = 0x29;
		public static const SLT : uint = 0x2a;
		public static const SEQ : uint = 0x2c;
		public static const SNE : uint = 0x2d;
		
		public static const MUL_MAT33 : uint = 0x100;
		public static const MUL_MAT44 : uint = 0x101;
		
		public static const NAME : Dictionary = new Dictionary();
		{
			NAME[MOV] = 'mov';		NAME[ADD] = 'add';		NAME[SUB] = 'sub';
			NAME[MUL] = 'mul';		NAME[DIV] = 'div';		NAME[RCP] = 'rcp';
			NAME[MIN] = 'min';		NAME[MAX] = 'max';		NAME[FRC] = 'frc';
			NAME[SQT] = 'sqt';		NAME[RSQ] = 'rsq';		NAME[POW] = 'pow';
			NAME[LOG] = 'log';		NAME[EXP] = 'exp';		NAME[NRM] = 'nrm';
			NAME[SIN] = 'sin';		NAME[COS] = 'cos';		NAME[CRS] = 'crs';
			NAME[DP3] = 'dp3';		NAME[DP4] = 'dp4';		NAME[ABS] = 'abs';
			NAME[NEG] = 'neg';		NAME[SAT] = 'sat';		NAME[M33] = 'm33';
			NAME[M44] = 'm44';		NAME[M34] = 'm34';		NAME[TEX] = 'tex';
			NAME[KIL] = 'kil';		NAME[SGE] = 'sge';		NAME[SLT] = 'slt';
			NAME[SEQ] = 'seq';		NAME[SNE] = 'sne';
			
			NAME[MUL_MAT33] = 'mulMat33';
			NAME[MUL_MAT44] = 'mulMat44';
		}
		
		private static const FLAGS : Dictionary = new Dictionary();
		{
			FLAGS[MOV] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE | InstructionFlag.LINEAR;
			FLAGS[ADD] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE | InstructionFlag.LINEAR;
			FLAGS[SUB] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.LINEAR;
			FLAGS[MUL] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE | InstructionFlag.LINEAR;
			FLAGS[DIV] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.LINEAR;
			FLAGS[RCP] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE | InstructionFlag.LINEAR;
			FLAGS[MIN] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE;
			FLAGS[MAX] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE;
			FLAGS[FRC] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[SQT] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[RSQ] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[POW] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE;
			FLAGS[LOG] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[EXP] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[NRM] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.SINGLE;
			FLAGS[SIN] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[COS] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[CRS] = InstructionFlag.AVAILABLE_ALL;
			FLAGS[DP3] = InstructionFlag.AVAILABLE_ALL;
			FLAGS[DP4] = InstructionFlag.AVAILABLE_ALL;
			FLAGS[ABS] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[NEG] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE | InstructionFlag.LINEAR;
			FLAGS[SAT] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.SINGLE;
			FLAGS[M33] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.SPECIAL_MATRIX | InstructionFlag.LINEAR;
			FLAGS[M44] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.SPECIAL_MATRIX | InstructionFlag.LINEAR;
			FLAGS[M34] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.SPECIAL_MATRIX | InstructionFlag.LINEAR;
			FLAGS[KIL] = InstructionFlag.AVAILABLE_FS | InstructionFlag.SINGLE;
			FLAGS[TEX] = InstructionFlag.AVAILABLE_FS;
			FLAGS[SGE] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE;
			FLAGS[SLT] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE;
			FLAGS[SEQ] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE;
			FLAGS[SNE] = InstructionFlag.AVAILABLE_ALL | InstructionFlag.COMPONENT_WISE | InstructionFlag.COMMUTATIVE;
			
			FLAGS[MUL_MAT33] = InstructionFlag.AVAILABLE_CPU | InstructionFlag.ASSOCIATIVE;
			FLAGS[MUL_MAT44] = InstructionFlag.AVAILABLE_CPU | InstructionFlag.ASSOCIATIVE;
		}
		
		private var _id : uint;
		
		public function get id()			: uint		{ return _id;		}
		public function get name()			: String	{ return NAME[_id];	}
		
		public function get argument1()		: AbstractNode	{ return getArgumentAt(0);	}
		public function get argument2() 	: AbstractNode 	{ return getArgumentAt(1);	}
		public function get component1()	: uint			{ return getComponentAt(0);	}
		public function get component2()	: uint			{ return getComponentAt(1);	}
		
		public function set argument1(v : AbstractNode) : void { setArgumentAt(0, v);	}
		public function set argument2(v : AbstractNode) : void { setArgumentAt(1, v);	}
		public function set component1(v : uint) 		: void { setComponentAt(0, v);	}
		public function set component2(v : uint) 		: void { setComponentAt(1, v);	}
		
		public function get isAssociative() : Boolean
		{
			return (FLAGS[_id] & InstructionFlag.ASSOCIATIVE) != 0;
		}
		
		public function get isComponentWise() : Boolean
		{
			return (FLAGS[_id] & InstructionFlag.COMPONENT_WISE) != 0;
		}
		
		public function get isSingle() : Boolean
		{
			return (FLAGS[_id] & InstructionFlag.SINGLE) != 0;
		}
		
		public function get isLinear() : Boolean
		{
			return (FLAGS[_id] & InstructionFlag.LINEAR) != 0;
		}
		
		public function isCommutative() : Boolean
		{
			return (FLAGS[_id] & InstructionFlag.COMMUTATIVE) != 0;
		}

		public function Instruction(id			: uint,
									argument1	: AbstractNode,
									argument2	: AbstractNode = null)
		{
			_id = id;
			
			var component1 : uint = id == MUL_MAT33 || id == MUL_MAT44 ? 
				Components.createContinuous(0, 0, 4, 4) :
				Components.createContinuous(0, 0, argument1.size, argument1.size)
			
			var arguments	: Vector.<AbstractNode>	= new <AbstractNode>[argument1];
			var components	: Vector.<uint>			= new <uint>[component1];
			
			if (!isSingle)
			{
				var component2 : uint;
				
				if ((FLAGS[id] & InstructionFlag.SPECIAL_MATRIX) != 0)
					component2	= Components.createContinuous(0, 0, 4, 4);
				else if (id != TEX && id != MUL_MAT33 && id != MUL_MAT44)
					component2	= Components.createContinuous(0, 0, argument2.size, argument2.size);
				
				arguments.push(argument2);
				components.push(component2);
			}
			
			arguments.fixed = components.fixed = true;
			
			super(arguments, components);
		}
		
		override protected function computeHash() : uint
		{
			// commutative operations invert the arguments when computing the hash if arg1.crc > arg2.crc
			// that way, mul(a, b).hash == mul(b, a).hash
			
			var hashLeft  : String = argument1.hash.toString(16) + '_' + component1.toString(16);
			
			if ((FLAGS[_id] & InstructionFlag.SINGLE) != 0)
				return CRC32.computeForString(hashLeft);
			
			else
			{
				var hashRight : String = argument2.hash.toString(16) + '_' + component2.toString(16);
				
				if ((FLAGS[_id] & InstructionFlag.COMMUTATIVE) != 0 && argument1.hash > argument2.hash)
					return CRC32.computeForString(hashRight + hashLeft);
				else
					return CRC32.computeForString(hashLeft + hashRight);
			}
		}
		
		override protected function computeSize() : uint
		{
			switch (_id)
			{
				case DP3: case DP4:	return 1;
				case M33: case M34: case NRM: case CRS: return 3;
				case TEX: case M44: return 4;
				
				default:
					var nodeSize : uint;
					nodeSize = Components.getMaxWriteOffset(component1) 
								- Components.getMinWriteOffset(component1) + 1;
					
					if (nodeSize > 4)
						throw new Error();
								
					if (!isSingle)
						nodeSize = Math.max(nodeSize, 
							Components.getMaxWriteOffset(component2) 
							- Components.getMinWriteOffset(component2) + 1
						);
					
					if (nodeSize < 1)
						throw new Error();
					
					if (nodeSize > 4)
						throw new Error();
					
					return nodeSize;
			}
		}
		
		override public function toString() : String
		{
			return name;
		}
		
		override public function clone() : AbstractNode
		{
			var clone : Instruction;
			if (isSingle)
			{
				clone = new Instruction(_id, argument1);
				clone.component1 = component1;
			}
			else
			{
				clone = new Instruction(_id, argument1, argument2);
				clone.component1 = component1;
				clone.component2 = component2;
			}
			
			return clone;
		}

	}
}
