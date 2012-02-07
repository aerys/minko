package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	public class Evaluator
	{
		public static const EVALUTION_FUNCTIONS	: Dictionary = new Dictionary();
		{
			EVALUTION_FUNCTIONS[Instruction.MOV]	= evaluateMov;
			EVALUTION_FUNCTIONS[Instruction.ADD]	= evaluateAdd;
			EVALUTION_FUNCTIONS[Instruction.SUB]	= evaluateSub;
			EVALUTION_FUNCTIONS[Instruction.MUL]	= evaluateMul;
			EVALUTION_FUNCTIONS[Instruction.DIV]	= evaluateDiv;
			EVALUTION_FUNCTIONS[Instruction.RCP]	= evaluateRcp;
			EVALUTION_FUNCTIONS[Instruction.MIN]	= evaluateMin;
			EVALUTION_FUNCTIONS[Instruction.MAX]	= evaluateMax;
			EVALUTION_FUNCTIONS[Instruction.FRC]	= evaluateFrc;
			EVALUTION_FUNCTIONS[Instruction.SQT]	= evaluateSqt;
			EVALUTION_FUNCTIONS[Instruction.RSQ]	= evaluateRsq;
			EVALUTION_FUNCTIONS[Instruction.POW]	= evaluatePow;
			EVALUTION_FUNCTIONS[Instruction.LOG]	= evaluateLog;
			EVALUTION_FUNCTIONS[Instruction.EXP]	= evaluateExp;
			EVALUTION_FUNCTIONS[Instruction.NRM]	= evaluateNrm;
			EVALUTION_FUNCTIONS[Instruction.SIN]	= evaluateSin;
			EVALUTION_FUNCTIONS[Instruction.COS]	= evaluateCos;
			EVALUTION_FUNCTIONS[Instruction.CRS]	= evaluateCrs;
			EVALUTION_FUNCTIONS[Instruction.DP3]	= evaluateDp3;
			EVALUTION_FUNCTIONS[Instruction.DP4]	= evaluateDp4;
			EVALUTION_FUNCTIONS[Instruction.ABS]	= evaluateAbs;
			EVALUTION_FUNCTIONS[Instruction.NEG]	= evaluateNeg;
			EVALUTION_FUNCTIONS[Instruction.SAT]	= evaluateSat;
			EVALUTION_FUNCTIONS[Instruction.M33]	= evaluateM33;
			EVALUTION_FUNCTIONS[Instruction.M44]	= evaluateM44;
			EVALUTION_FUNCTIONS[Instruction.M34]	= evaluateM34;
			EVALUTION_FUNCTIONS[Instruction.SGE]	= evaluateSge;
			EVALUTION_FUNCTIONS[Instruction.SLT]	= evaluateSlt;
			EVALUTION_FUNCTIONS[Instruction.SEQ]	= evaluateSeq;
			EVALUTION_FUNCTIONS[Instruction.SNE]	= evaluateSne;
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
			var newData : Vector.<Number>	= new Vector.<Number>(length, true);
			
			for (var i : uint = 0; i < length; ++i)
				newData[i] = data[Components.getReadAtIndex(i, components)];
			
			return newData;
		}
		
		public static function evaluateComponentWithHoles(components	: uint,
														  data			: Vector.<Number>) : Vector.<Number>
		{
			var newData : Vector.<Number>	= new Vector.<Number>(4, true);
			
			for (var i : uint = 0; i < 4; ++i)
			{
				var index : uint = Components.getReadAtIndex(i, components);
				newData[i] = index != Components._ ? data[index] : NaN;
			}
			
			return newData;
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = 
					arg1[i < arg1Length ? i : arg1Length - 1] /
					arg2[i < arg2Length ? i : arg2Length - 1];
			
			return result;
		}
		
		public static function evaluateRcp(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.max(
					arg1[i < arg1Length ? i : arg1Length - 1],
					arg2[i < arg2Length ? i : arg2Length - 1]);
			
			return result;
		}
		
		public static function evaluateFrc(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg[i] > 0 ? arg[i] - Math.floor(arg[i]) : arg[i] + Math.ceil(arg[i]);
			
			return result;
		}
		
		public static function evaluateSqt(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg[i] > 0 ? Math.sqrt(arg[i]) : -Math.sqrt(-arg[i]);
			
			return result;
		}
		
		public static function evaluateRsq(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.pow(
					arg1[i < arg1Length ? i : arg1Length - 1],
					arg2[i < arg2Length ? i : arg2Length - 1]);
			
			return result;
		}
		
		public static function evaluateLog(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.log(arg[i]);
			
			return result;
		}
		
		public static function evaluateExp(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.exp(arg[i]);
			
			return result;
		}
		
		public static function evaluateNrm(arg : Vector.<Number>) : Vector.<Number>
		{
			var result	: Vector.<Number>	= new Vector.<Number>(3);
			var length	: uint				= arg.length;
			var dist	: Number			= 0;
			
			for (var i : uint = 0; i < 3; ++i)
			{
				var v : Number = arg[i < length ? i : length - 1];
				dist += v * v;
			}
			
			dist = Math.sqrt(dist);
			
			for (i = 0; i < 3; ++i)
				result[i] = arg[i] / dist;
			
			return result;
		}
		
		public static function evaluateSin(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.sin(arg[i]);
			
			return result;
		}
		
		public static function evaluateCos(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.cos(arg[i]);
			
			return result;
		}
		
		public static function evaluateCrs(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result	: Vector.<Number>	= new Vector.<Number>(3);
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			
			result[0] = arg1[1 < l1 ? 1 : l1 - 1] * arg2[2 < l2 ? 2 : l2 - 1] - arg1[2 < l1 ? 2 : l1 - 1] * arg2[1 < l2 ? 1 : l2 - 1];
			result[1] = arg1[2 < l1 ? 2 : l1 - 1] * arg2[0] - arg1[0] * arg2[2 < l2 ? 2 : l2 - 1];
			result[2] = arg1[0] * arg2[1 < l2 ? 1 : l2 - 1] - arg1[1 < l1 ? 1 : l1 - 1] * arg2[0];
			
			return result;
		}
		
		public static function evaluateDp3(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result	: Vector.<Number>	= new Vector.<Number>(1);
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			
			result[0] = 
				arg1[0] * arg2[0] 
				+ arg1[1 < l1 ? 1 : l1 - 1] * arg2[1 < l2 ? 1 : l2 - 1] 
				+ arg1[2 < l1 ? 2 : l1 - 1] * arg2[2 < l2 ? 2 : l2 - 1];
			
			return result;
		}
		
		public static function evaluateDp4(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var result	: Vector.<Number>	= new Vector.<Number>(1);
			var l1		: uint				= arg1.length;
			var l2		: uint				= arg2.length;
			
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
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.abs(arg[i]);
			
			return result;
		}
		
		public static function evaluateNeg(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = -arg[i];
			
			return result;
		}
		
		public static function evaluateSat(arg : Vector.<Number>) : Vector.<Number>
		{
			var length	: uint				= arg.length;
			var result	: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = Math.max(0, Math.min(1, arg[i]));
			
			return result;
		}
		
		public static function evaluateM33(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			throw new Error('implement me');
		}
		
		public static function evaluateM44(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			throw new Error('implement me');
		}
		
		public static function evaluateM34(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			throw new Error('implement me');
		}
		
		public static function evaluateSge(arg1 : Vector.<Number>,
										   arg2 : Vector.<Number>) : Vector.<Number>
		{
			var arg1Length	: uint				= arg1.length;
			var arg2Length	: uint				= arg2.length;
			var length		: uint				= Math.max(arg1Length, arg2Length);
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
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
			var result		: Vector.<Number>	= new Vector.<Number>(length);
			
			for (var i : uint = 0; i < length; ++i)
				result[i] = arg1[i < arg1Length ? i : arg1Length - 1] 
					!= arg2[i < arg2Length ? i : arg2Length - 1] ? 1 : 0;
			
			return result;
		}
		
	}
}