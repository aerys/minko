package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.type.Factory;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Evaluator
	{
		private static const VEC_FACTORY : Factory = Factory.getFactory(Class(Vector.<Number>));
		
		public static const EVALUATION_FUNCTIONS	: Dictionary = new Dictionary();
		{
			EVALUATION_FUNCTIONS[Instruction.MOV]	= evaluateMov;
			EVALUATION_FUNCTIONS[Instruction.ADD]	= evaluateAdd;
			EVALUATION_FUNCTIONS[Instruction.SUB]	= evaluateSub;
			EVALUATION_FUNCTIONS[Instruction.MUL]	= evaluateMul;
			EVALUATION_FUNCTIONS[Instruction.DIV]	= evaluateDiv;
			EVALUATION_FUNCTIONS[Instruction.RCP]	= evaluateRcp;
			EVALUATION_FUNCTIONS[Instruction.MIN]	= evaluateMin;
			EVALUATION_FUNCTIONS[Instruction.MAX]	= evaluateMax;
			EVALUATION_FUNCTIONS[Instruction.FRC]	= evaluateFrc;
			EVALUATION_FUNCTIONS[Instruction.SQT]	= evaluateSqt;
			EVALUATION_FUNCTIONS[Instruction.RSQ]	= evaluateRsq;
			EVALUATION_FUNCTIONS[Instruction.POW]	= evaluatePow;
			EVALUATION_FUNCTIONS[Instruction.LOG]	= evaluateLog;
			EVALUATION_FUNCTIONS[Instruction.EXP]	= evaluateExp;
			EVALUATION_FUNCTIONS[Instruction.NRM]	= evaluateNrm;
			EVALUATION_FUNCTIONS[Instruction.SIN]	= evaluateSin;
			EVALUATION_FUNCTIONS[Instruction.COS]	= evaluateCos;
			EVALUATION_FUNCTIONS[Instruction.CRS]	= evaluateCrs;
			EVALUATION_FUNCTIONS[Instruction.DP3]	= evaluateDp3;
			EVALUATION_FUNCTIONS[Instruction.DP4]	= evaluateDp4;
			EVALUATION_FUNCTIONS[Instruction.ABS]	= evaluateAbs;
			EVALUATION_FUNCTIONS[Instruction.NEG]	= evaluateNeg;
			EVALUATION_FUNCTIONS[Instruction.SAT]	= evaluateSat;
			EVALUATION_FUNCTIONS[Instruction.M33]	= evaluateM33;
			EVALUATION_FUNCTIONS[Instruction.M44]	= evaluateM44;
			EVALUATION_FUNCTIONS[Instruction.M34]	= evaluateM34;
			EVALUATION_FUNCTIONS[Instruction.SGE]	= evaluateSge;
			EVALUATION_FUNCTIONS[Instruction.SLT]	= evaluateSlt;
			EVALUATION_FUNCTIONS[Instruction.SEQ]	= evaluateSeq;
			EVALUATION_FUNCTIONS[Instruction.SNE]	= evaluateSne;
			
			EVALUATION_FUNCTIONS[Instruction.MUL_MAT33] = evaluateMulMat33;
			EVALUATION_FUNCTIONS[Instruction.MUL_MAT44] = evaluateMulMat44;
		}
		
		public static function evaluateVariadicExtract(index	: Number,
													   values	: Vector.<Number>,
													   isMatrix	: Boolean) : Vector.<Number>
		{
			var fieldSize : uint = isMatrix ? 16 : 4;
			return values.slice(index * fieldSize, (index + 1) * fieldSize); 
		}
		
		public static function evaluateComponents(components	: uint, 
												  data			: Vector.<Number>) : Vector.<Number>
		{
			if (Components.hasHoles(components))
				throw new Error('Components with holes are only supported for overwriters');
			
			var length	: uint				= Components.getMaxWriteOffset(components) + 1;
			var newData : Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			
			newData.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				newData[i] = data[Components.getReadAtIndex(i, components)];
			
			return newData;
		}
		
		public static function evaluateComponentWithHoles(components	: uint,
														  data			: Vector.<Number>) : Vector.<Number>
		{
			var newData : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			newData.length = 4;
			
			for (var i : uint = 0; i < 4; ++i)
			{
				var index : uint = Components.getReadAtIndex(i, components);
				newData[i] = index != 4 ? data[index] : NaN;
			}
			
			return newData;
		}
		
		public static function evaluateMulMat33(arg1 : Vector.<Number>, arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 16;
			
			result[0]	= arg1[0]  * arg2[0]  + arg1[1]  * arg2[4] + arg1[2] * arg2[8];
			result[1]	= arg1[0]  * arg2[1]  + arg1[1]  * arg2[5] + arg1[2] * arg2[9];
			result[2]	= arg1[0]  * arg2[2]  + arg1[1]  * arg2[6] + arg1[2] * arg2[10];
			result[3]	= 0;
			
			result[4]	= arg1[4]  * arg2[0]  + arg1[5]  * arg2[4] + arg1[6] * arg2[8];
			result[5]	= arg1[4]  * arg2[1]  + arg1[5]  * arg2[5] + arg1[6] * arg2[9];
			result[6]	= arg1[4]  * arg2[2]  + arg1[5]  * arg2[6] + arg1[6] * arg2[10];
			result[7]	= 0;
			
			result[8]	= arg1[8]  * arg2[0]  + arg1[9]  * arg2[4] + arg1[10] * arg2[8];
			result[9]	= arg1[8]  * arg2[1]  + arg1[9]  * arg2[5] + arg1[10] * arg2[9];
			result[10]	= arg1[8]  * arg2[2]  + arg1[9]  * arg2[6] + arg1[10] * arg2[10];
			result[11]	= 0;
			
			result[12]	= arg1[12] * arg2[0]  + arg1[13] * arg2[4] + arg1[14] * arg2[8];
			result[13]	= arg1[12] * arg2[1]  + arg1[13] * arg2[5] + arg1[14] * arg2[9];
			result[14]	= arg1[12] * arg2[2]  + arg1[13] * arg2[6] + arg1[14] * arg2[10];
			result[15]	= 1;
			
			return result;
		}
		
		public static function evaluateMulMat44(arg1 : Vector.<Number>, arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1_00 : Number = arg1[0];
			var arg1_01 : Number = arg1[1];
			var arg1_02 : Number = arg1[2];
			var arg1_03 : Number = arg1[3];
			var arg1_04 : Number = arg1[4];
			var arg1_05 : Number = arg1[5];
			var arg1_06 : Number = arg1[6];
			var arg1_07 : Number = arg1[7];
			var arg1_08 : Number = arg1[8];
			var arg1_09 : Number = arg1[9];
			var arg1_10 : Number = arg1[10];
			var arg1_11 : Number = arg1[11];
			var arg1_12 : Number = arg1[12];
			var arg1_13 : Number = arg1[13];
			var arg1_14 : Number = arg1[14];
			var arg1_15 : Number = arg1[15];
			
			var arg2_00 : Number = arg2[0];
			var arg2_01 : Number = arg2[1];
			var arg2_02 : Number = arg2[2];
			var arg2_03 : Number = arg2[3];
			var arg2_04 : Number = arg2[4];
			var arg2_05 : Number = arg2[5];
			var arg2_06 : Number = arg2[6];
			var arg2_07 : Number = arg2[7];
			var arg2_08 : Number = arg2[8];
			var arg2_09 : Number = arg2[9];
			var arg2_10 : Number = arg2[10];
			var arg2_11 : Number = arg2[11];
			var arg2_12 : Number = arg2[12];
			var arg2_13 : Number = arg2[13];
			var arg2_14 : Number = arg2[14];
			var arg2_15 : Number = arg2[15];
			
			var result : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			
			result[0]	= arg1_00 * arg2_00 + arg1_01 * arg2_04 + arg1_02 * arg2_08 + arg1_03 * arg2_12;
			result[1]	= arg1_00 * arg2_01 + arg1_01 * arg2_05 + arg1_02 * arg2_09 + arg1_03 * arg2_13;
			result[2]	= arg1_00 * arg2_02 + arg1_01 * arg2_06 + arg1_02 * arg2_10 + arg1_03 * arg2_14;
			result[3]	= arg1_00 * arg2_03 + arg1_01 * arg2_07 + arg1_02 * arg2_11 + arg1_03 * arg2_15;
			result[4]	= arg1_04 * arg2_00 + arg1_05 * arg2_04 + arg1_06 * arg2_08 + arg1_07 * arg2_12;
			result[5]	= arg1_04 * arg2_01 + arg1_05 * arg2_05 + arg1_06 * arg2_09 + arg1_07 * arg2_13;
			result[6]	= arg1_04 * arg2_02 + arg1_05 * arg2_06 + arg1_06 * arg2_10 + arg1_07 * arg2_14;
			result[7]	= arg1_04 * arg2_03 + arg1_05 * arg2_07 + arg1_06 * arg2_11 + arg1_07 * arg2_15;
			result[8]	= arg1_08 * arg2_00 + arg1_09 * arg2_04 + arg1_10 * arg2_08 + arg1_11 * arg2_12;
			result[9]	= arg1_08 * arg2_01 + arg1_09 * arg2_05 + arg1_10 * arg2_09 + arg1_11 * arg2_13;
			result[10]	= arg1_08 * arg2_02 + arg1_09 * arg2_06 + arg1_10 * arg2_10 + arg1_11 * arg2_14;
			result[11]	= arg1_08 * arg2_03 + arg1_09 * arg2_07 + arg1_10 * arg2_11 + arg1_11 * arg2_15;
			result[12]	= arg1_12 * arg2_00 + arg1_13 * arg2_04 + arg1_14 * arg2_08 + arg1_15 * arg2_12;
			result[13]	= arg1_12 * arg2_01 + arg1_13 * arg2_05 + arg1_14 * arg2_09 + arg1_15 * arg2_13;
			result[14]	= arg1_12 * arg2_02 + arg1_13 * arg2_06 + arg1_14 * arg2_10 + arg1_15 * arg2_14;
			result[15]	= arg1_12 * arg2_03 + arg1_13 * arg2_07 + arg1_14 * arg2_11 + arg1_15 * arg2_15;
			
			return result;
		}
		
		public static function evaluateMov(arg : Vector.<Number>) : Vector.<Number>
		{
			return arg;
		}
		
		public static function evaluateAdd(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 
					arg1[i < arg1Length ? i : arg1Length - 1] +
					arg2[i < arg2Length ? i : arg2Length - 1];		
			
			return result;
		}
		
		public static function evaluateSub(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 
					arg1[i < arg1Length ? i : arg1Length - 1] -
					arg2[i < arg2Length ? i : arg2Length - 1];		
			
			return result;
		}
		
		public static function evaluateMul(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 
					arg1[i < arg1Length ? i : arg1Length - 1] *
					arg2[i < arg2Length ? i : arg2Length - 1];		
			
			return result;
		}
		
		public static function evaluateDiv(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 
					arg1[i < arg1Length ? i : arg1Length - 1] /
					arg2[i < arg2Length ? i : arg2Length - 1];
			
			return result;
		}
		
		public static function evaluateRcp(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 1 / arg[i];
			
			return result;
		}
		
		public static function evaluateMin(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.min(
					arg1[i < arg1Length ? i : arg1Length - 1],
					arg2[i < arg2Length ? i : arg2Length - 1]);
			
			return result;
		}
		
		public static function evaluateMax(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.max(
					arg1[i < arg1Length ? i : arg1Length - 1],
					arg2[i < arg2Length ? i : arg2Length - 1]);
			
			return result;
		}
		
		public static function evaluateFrc(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg[i] > 0 ? arg[i] - Math.floor(arg[i]) : arg[i] + Math.ceil(arg[i]);
			
			return result;
		}
		
		public static function evaluateSqt(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg[i] > 0 ? Math.sqrt(arg[i]) : -Math.sqrt(-arg[i]);
			
			return result;
		}
		
		public static function evaluateRsq(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg[i] > 0 ? 1 / Math.sqrt(arg[i]) : -1 / Math.sqrt(-arg[i]);
			
			return result;
		}
		
		public static function evaluatePow(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.pow(
					arg1[i < arg1Length ? i : arg1Length - 1],
					arg2[i < arg2Length ? i : arg2Length - 1]);
			
			return result;
		}
		
		public static function evaluateLog(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.log(arg[i]);
			
			return result;
		}
		
		public static function evaluateExp(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.exp(arg[i]);
			
			return result;
		}
		
		public static function evaluateNrm(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var dist	: Number			= 0;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 3;
			
			for (var i : uint = 0; i < 3; ++i)
			{
				var v : Number = arg[i < length ? i : length - 1];
				dist += v * v;
			}
			
			dist = Math.sqrt(dist);
			
			for (i = 0; i < 3; ++i)
				result[i] = arg[i < length ? i : length - 1] / dist;
			
			return result;
		}
		
		public static function evaluateSin(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.sin(arg[i]);
			
			return result;
		}
		
		public static function evaluateCos(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.cos(arg[i]);
			
			return result;
		}
		
		public static function evaluateCrs(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 3;
			
			result[0] = arg1[1 < l1 ? 1 : l1 - 1] * arg2[2 < l2 ? 2 : l2 - 1] - arg1[2 < l1 ? 2 : l1 - 1] * arg2[1 < l2 ? 1 : l2 - 1];
			result[1] = arg1[2 < l1 ? 2 : l1 - 1] * arg2[0] - arg1[0] * arg2[2 < l2 ? 2 : l2 - 1];
			result[2] = arg1[0] * arg2[1 < l2 ? 1 : l2 - 1] - arg1[1 < l1 ? 1 : l1 - 1] * arg2[0];
			
			return result;
		}
		
		public static function evaluateDp3(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 1;
			
			result[0] = 
				arg1[0] * arg2[0] 
				+ arg1[1 < l1 ? 1 : l1 - 1] * arg2[1 < l2 ? 1 : l2 - 1] 
				+ arg1[2 < l1 ? 2 : l1 - 1] * arg2[2 < l2 ? 2 : l2 - 1];
			
			return result;
		}
		
		public static function evaluateDp4(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 1;
			
			result[0] = 
				arg1[0] * arg2[0] 
				+ arg1[1 < l1 ? 1 : l1 - 1] * arg2[1 < l2 ? 1 : l2 - 1] 
				+ arg1[2 < l1 ? 2 : l1 - 1] * arg2[2 < l2 ? 2 : l2 - 1]
				+ arg1[3 < l1 ? 3 : l1 - 1] * arg2[3 < l2 ? 3 : l2 - 1];
			
			return result;
		}
		
		public static function evaluateAbs(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.abs(arg[i]);
			
			return result;
		}
		
		public static function evaluateNeg(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = -arg[i];
			
			return result;
		}
		
		public static function evaluateSat(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.max(0, Math.min(1, arg[i]));
			
			return result;
		}
		
		public static function evaluateM33(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 3;
			
			result[0] = arg1[0] * arg2[0] + arg1[1] * arg2[1] + arg1[2] * arg2[2];
			result[1] = arg1[0] * arg2[4] + arg1[1] * arg2[5] + arg1[2] * arg2[6];
			result[2] = arg1[0] * arg2[8] + arg1[1] * arg2[9] + arg1[2] * arg2[10];
			
			return result;
		}
		
		public static function evaluateM44(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 4;
			
			result[0] = arg1[0] * arg2[0] + arg1[1] * arg2[1] + arg1[2] * arg2[2] + arg1[3] * arg2[3];
			result[1] = arg1[0] * arg2[4] + arg1[1] * arg2[5] + arg1[2] * arg2[6] + arg1[3] * arg2[7];
			result[2] = arg1[0] * arg2[8] + arg1[1] * arg2[9] + arg1[2] * arg2[10] + arg1[3] * arg2[11];
			result[3] = arg1[0] * arg2[12] + arg1[1] * arg2[13] + arg1[2] * arg2[14] + arg1[3] * arg2[15];
			
			return result;
		}
		
		public static function evaluateM34(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result : Vector.<Number> = Vector.<Number>(VEC_FACTORY.create(true));
			result.length = 3;
			
			result[0] = arg1[0] * arg2[0] + arg1[1] * arg2[4] + arg1[2] * arg2[8] + arg1[3] * arg2[12];
			result[1] = arg1[0] * arg2[1] + arg1[1] * arg2[5] + arg1[2] * arg2[9] + arg1[3] * arg2[13];
			result[2] = arg1[0] * arg2[2] + arg1[1] * arg2[6] + arg1[2] * arg2[10] + arg1[3] * arg2[14];
			
			return result;
		}
		
		public static function evaluateSge(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg1[i < arg1Length ? i : arg1Length - 1] 
					>= arg2[i < arg2Length ? i : arg2Length - 1] ? 1 : 0;
			
			return result;
		}
		
		public static function evaluateSlt(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg1[i < arg1Length ? i : arg1Length - 1] 
					< arg2[i < arg2Length ? i : arg2Length - 1] ? 1 : 0;
			
			return result;
		}
		
		public static function evaluateSeq(arg1 : Vector.<Number>,
									 	   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg1[i < arg1Length ? i : arg1Length - 1] 
					== arg2[i < arg2Length ? i : arg2Length - 1] ? 1 : 0;
			
			return result;
		}
		
		public static function evaluateSne(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= Vector.<Number>(VEC_FACTORY.create(true));
			result.length = length;
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg1[i < arg1Length ? i : arg1Length - 1] 
					!= arg2[i < arg2Length ? i : arg2Length - 1] ? 1 : 0;
			
			return result;
		}
		
	}
}