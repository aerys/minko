package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.InstructionFlag;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Instruction implements INode
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
		
		private var _id				: uint;
		
		private var _arg1			: INode;
		private var _arg2			: INode;
		
		private var _arg1Components	: uint;
		private var _arg2Components	: uint;
		
		private var _size			: uint;
		private var _sizeIsValid	: Boolean;
		
		private var _hash			: uint;
		private var _hashIsValid	: Boolean;
		
		public function get id() : uint
		{
			return _id;
		}
		
		public function get name() : String
		{
			return NAME[_id];
		}
		
		public function get arg1() : INode
		{
			return _arg1;
		}
		
		public function get arg2() : INode
		{
			return _arg2; 
		}
		
		public function get arg1Components() : uint
		{
			return _arg1Components;
		}
		
		public function get arg2Components() : uint
		{
			return _arg2Components;
		}
		
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

		public function get size() : uint
		{
			if (!_sizeIsValid)
			{
				switch (_id)
				{
					case DP3: case DP4:
						_size = 1;
						break;
					
					case M33: case M34: case NRM: case CRS:
						_size = 3;
						break;
					
					case TEX: case M44:
						_size = 4;
						break;
						
					default:
						// size of _XX_ is 2
						// size of X_X_ is 3
						// size of _XYZ is 3
						
						_size = Components.getMaxWriteOffset(_arg1Components) - Components.getMinWriteOffset(_arg1Components) + 1;
						if (!isSingle)
							_size = Math.max(_size, Components.getMaxWriteOffset(_arg2Components) - Components.getMinWriteOffset(_arg2Components) + 1);
						
						break;
				}
				
				_sizeIsValid = true;
			}
			
			if (_size > 4)
				throw new Error();
			
			return _size; 
		}
		
		public function get hash() : uint
		{
			if (!_hashIsValid)
			{
				// commutative operations invert the arguments when computing the hash if arg1.crc > arg2.crc
				// that way, mul(a, b).hash == mul(b, a).hash
				var hashLeft  : String = _arg1.hash.toString(16) + '_' + _arg1Components.toString(16);
				if ((FLAGS[_id] & InstructionFlag.SINGLE) != 0)
					_hash = CRC32.computeForString(hashLeft);
					
				else
				{
					var hashRight : String = _arg2.hash.toString(16) + '_' + _arg2Components.toString(16);
					
					if ((FLAGS[_id] & InstructionFlag.COMMUTATIVE) != 0 && _arg1.hash > _arg2.hash)
						_hash = CRC32.computeForString(hashRight + hashLeft);
					else
						_hash = CRC32.computeForString(hashLeft + hashRight);
				}
				
				_hashIsValid = true;
			}
			
			return _hash; 
		}
		
		public function set arg1(v : INode) : void
		{
			if (v.hash != _arg1.hash)
				_hashIsValid = false;
			
			_arg1 = v;
		}
		
		public function set arg2(v : INode) : void
		{
			if (isSingle)
				throw new Error('arg2 cannot be set, this is a single operation');
			
			if (v.hash != _arg2.hash)
				_hashIsValid = false;
			
			_arg2 = v;
		}
		
		public function set arg1Components(v : uint) : void
		{
			if (v == Components.____)
				throw new Error("Component cannot be empty.");
			
			/*if (_arg1.size < Components.getMaxReadOffset(v))
				throw new Error('Component is reading too far.');*/
			
			if (v != _arg1Components)
			{
				_arg1Components	= v;
				_hashIsValid	= false;
				_sizeIsValid	= false;
			}
		}
		
		public function set arg2Components(v : uint) : void
		{
			if (isSingle || _id === TEX)
				throw new Error('arg2Components cannot be set, this is a single operation');
			
			if (v == Components.____)
				throw new Error("Component cannot be empty.");
			
			// cast to int: workaround iOS int VS uint bug 
			if (int(_arg2.size) < Components.getMaxReadOffset(v))
				throw new Error('Component is reading too far.');
			
			if (v != _arg2Components)
			{
				_arg2Components	= v;
				_hashIsValid	= false;
				_sizeIsValid	= false;
			}
		}
		
		public function Instruction(id				: uint,
									arg1			: INode,
									arg2			: INode = null)
		{
			_sizeIsValid	= false;
			_hashIsValid	= false;
			_id				= id;
			_arg1			= arg1;
			
			_arg1Components	= id == MUL_MAT33 || id == MUL_MAT44 ? 
				Components.createContinuous(0, 0, 4, 4) :
				Components.createContinuous(0, 0, _arg1.size, _arg1.size)
			
			if (!isSingle)
			{
				_arg2 = arg2;
				
				if ((FLAGS[id] & InstructionFlag.SPECIAL_MATRIX) != 0)
					_arg2Components	= Components.createContinuous(0, 0, 4, 4);
				else if (id != TEX && id != MUL_MAT33 && id != MUL_MAT44)
					_arg2Components	= Components.createContinuous(0, 0, _arg2.size, _arg2.size);
			}
		}
		
		public function toString() : String
		{
			return name;
		}
	}
}
