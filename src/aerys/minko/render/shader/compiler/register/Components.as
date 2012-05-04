package aerys.minko.render.shader.compiler.register
{
	/**
	 * Collection of functions to manipulate register components.
	 * 
	 * @private
	 * @author Romain Gilliotte
	 */	
	public final class Components
	{
		private static const X : uint = 0;
		private static const Y : uint = 1;
		private static const Z : uint = 2;
		private static const W : uint = 3;
		private static const _ : uint = 4;
		
		public static const ____ : uint = _ | (_ << 8) | (_ << 16) | (_ << 24);
		
		/**
		 * String equivalent of each byte in a component part
		 */		
		private static const STRINGS : Vector.<String> = new <String>['x', 'y', 'z', 'w', '_'];
		
		/**
		 * Conversion table from swizzle to string (for AGAL output).
		 */
		private static const SWIZZLE_STRINGS : Vector.<String> = new <String>[
			'xxxx', 'yxxx', 'zxxx', 'wxxx', 'xyxx', 'yyxx', 'zyxx', 'wyxx',		//   0 - 7
			'xzxx', 'yzxx', 'zzxx', 'wzxx', 'xwxx', 'ywxx', 'zwxx', 'wwxx',		//   8 - 15
			'xxyx', 'yxyx', 'zxyx', 'wxyx', 'xyyx', 'yyyx', 'zyyx', 'wyyx',		//  16 - 23
			'xzyx', 'yzyx', 'zzyx', 'wzyx', 'xwyx', 'ywyx', 'zwyx', 'wwyx',		//  24 - 31
			'xxzx', 'yxzx', 'zxzx', 'wxzx', 'xyzx', 'yyzx', 'zyzx', 'wyzx',		//  32 - 39
			'xzzx', 'yzzx', 'zzzx', 'wzzx', 'xwzx', 'ywzx', 'zwzx', 'wwzx',		//  40 - 47
			'xxwx', 'yxwx', 'zxwx', 'wxwx', 'xywx', 'yywx', 'zywx', 'wywx',		//  48 - 55
			'xzwx', 'yzwx', 'zzwx', 'wzwx', 'xwwx', 'ywwx', 'zwwx', 'wwwx',		//  56 - 63
			'xxxy', 'yxxy', 'zxxy', 'wxxy', 'xyxy', 'yyxy', 'zyxy', 'wyxy',		//  64 - 71
			'xzxy', 'yzxy', 'zzxy', 'wzxy', 'xwxy', 'ywxy', 'zwxy', 'wwxy',		//  72 - 79
			'xxyy', 'yxyy', 'zxyy', 'wxyy', 'xyyy', 'yyyy', 'zyyy', 'wyyy',		//  80 - 87
			'xzyy', 'yzyy', 'zzyy', 'wzyy', 'xwyy', 'ywyy', 'zwyy', 'wwyy',		//  88 - 95
			'xxzy', 'yxzy', 'zxzy', 'wxzy', 'xyzy', 'yyzy', 'zyzy', 'wyzy',		//  96 - 103
			'xzzy', 'yzzy', 'zzzy', 'wzzy', 'xwzy', 'ywzy', 'zwzy', 'wwzy',		// 104 - 111
			'xxwy', 'yxwy', 'zxwy', 'wxwy', 'xywy', 'yywy', 'zywy', 'wywy',		// 112 - 119
			'xzwy', 'yzwy', 'zzwy', 'wzwy', 'xwwy', 'ywwy', 'zwwy', 'wwwy',		// 120 - 127
			'xxxz', 'yxxz', 'zxxz', 'wxxz', 'xyxz', 'yyxz', 'zyxz', 'wyxz',		// 128 - 135
			'xzxz', 'yzxz', 'zzxz', 'wzxz', 'xwxz', 'ywxz', 'zwxz', 'wwxz',		// 136 - 143
			'xxyz', 'yxyz', 'zxyz', 'wxyz', 'xyyz', 'yyyz', 'zyyz', 'wyyz',		// 144 - 151
			'xzyz', 'yzyz', 'zzyz', 'wzyz', 'xwyz', 'ywyz', 'zwyz', 'wwyz',		// 152 - 159
			'xxzz', 'yxzz', 'zxzz', 'wxzz', 'xyzz', 'yyzz', 'zyzz', 'wyzz',		// 160 - 167
			'xzzz', 'yzzz', 'zzzz', 'wzzz', 'xwzz', 'ywzz', 'zwzz', 'wwzz',		// 168 - 175
			'xxwz', 'yxwz', 'zxwz', 'wxwz', 'xywz', 'yywz', 'zywz', 'wywz',		// 176 - 183
			'xzwz', 'yzwz', 'zzwz', 'wzwz', 'xwwz', 'ywwz', 'zwwz', 'wwwz',		// 184 - 191
			'xxxw', 'yxxw', 'zxxw', 'wxxw', 'xyxw', 'yyxw', 'zyxw', 'wyxw',		// 192 - 199
			'xzxw', 'yzxw', 'zzxw', 'wzxw', 'xwxw', 'ywxw', 'zwxw', 'wwxw',		// 200 - 207
			'xxyw', 'yxyw', 'zxyw', 'wxyw', 'xyyw', 'yyyw', 'zyyw', 'wyyw',		// 208 - 215
			'xzyw', 'yzyw', 'zzyw', 'wzyw', 'xwyw', 'ywyw', 'zwyw', 'wwyw',		// 216 - 223
			'xxzw', 'yxzw', 'zxzw', 'wxzw', 'xyzw', 'yyzw', 'zyzw', 'wyzw',		// 224 - 231
			'xzzw', 'yzzw', 'zzzw', 'wzzw', 'xwzw', 'ywzw', 'zwzw', 'wwzw',		// 232 - 239
			'xxww', 'yxww', 'zxww', 'wxww', 'xyww', 'yyww', 'zyww', 'wyww',		// 240 - 247
			'xzww', 'yzww', 'zzww', 'wzww', 'xwww', 'ywww', 'zwww', 'wwww'		// 248 - 255
		];
		
		/**
		 * Conversion table from mask to string (for AGAL output).
		 */		
		private static const MASK_STRINGS : Vector.<String> = new <String>[
			 '',  'x',  'y',  'xy',  'z',  'xz',  'yz', 'xyz',					// 0 - 7
			'w', 'xw', 'yw', 'xyw', 'zw', 'xzw', 'yzw', 'xyzw'					// 8 - 15
		];
		
		public static function createContinuous(writeOffset	: uint,
												readOffset	: uint,
												writeSize	: uint,
												readSize	: uint) : uint
		{
			if (readOffset + readSize > 4)
				throw new Error('Invalid components: you are reading too far');
				
			if (writeOffset + writeSize > 4)
				throw new Error('Invalid components: you are writing too far');
			
			var component	: uint;
			var i			: uint;
			
			for (i = 0; i < writeOffset; ++i)
				component |= _ << (8 * i);
			
			for (; i < writeOffset + writeSize; ++i)
			{
				var readComponent : uint = (i - writeOffset + readOffset);
				if (readComponent > readOffset + readSize - 1)
					readComponent = readOffset + readSize - 1;
				
				component |= readComponent << (8 * i);
			}
			
			for (; i < 4; ++i)
				component |= _ << (8 * i);
			
			return component;
		}
		
		public static function createFromParts(part0 : uint,
											   part1 : uint = _,
											   part2 : uint = _,
											   part3 : uint = _) : uint
		{
			if (part0 > _ || part1 > _ || part2 > _ || part3 > _)
				throw new ArgumentError('Invalid arguments');
			
			return part0 | (part1 << 8) | (part2 << 16) | (part3 << 24);
		}
										
		public static function applyCombination(input		: uint,
												modifier	: uint) : uint
		{
			var result	: uint = 0;
			var index	: uint;
			
			index = getReadAtIndex(0, modifier);
			result |= index != _ ? getReadAtIndex(index, input) : _;
			
			index = getReadAtIndex(1, modifier);
			result |= (index != _ ? getReadAtIndex(index, input) : _) << 8;
			
			index = getReadAtIndex(2, modifier);
			result |= (index != _ ? getReadAtIndex(index, input) : _) << 16;
			
			index = getReadAtIndex(3, modifier);
			result |= (index != _ ? getReadAtIndex(index, input) : _) << 24;
			
			return result;
		}
		
		public static function applyOverwriting(input1 : uint, 
												input2 : uint) : uint
		{
			var result : uint = 0;
			
			var inputX1 : uint = getReadAtIndex(0, input1);
			var inputY1 : uint = getReadAtIndex(1, input1);
			var inputZ1 : uint = getReadAtIndex(2, input1);
			var inputW1 : uint = getReadAtIndex(3, input1);
			
			var inputX2 : uint = getReadAtIndex(0, input2);
			var inputY2 : uint = getReadAtIndex(1, input2);
			var inputZ2 : uint = getReadAtIndex(2, input2);
			var inputW2 : uint = getReadAtIndex(3, input2);
			
			result |= (inputX1 != _ ? inputX1 : inputX2);
			result |= (inputY1 != _ ? inputY1 : inputY2) << 8;
			result |= (inputZ1 != _ ? inputZ1 : inputZ2) << 16;
			result |= (inputW1 != _ ? inputW1 : inputW2) << 24;
			
			return result;
		}
		
		public static function applyWriteOffset(component	: uint, 
												offset		: int) : uint
		{
			var i : uint;
			if (offset > 0)
				for (i = 0; i < offset; ++i)
				{
					component <<= 8;
					component |= _;
				}
			else
			{
				offset = -offset;
				for (i = 0; i < offset; ++i)
				{
					component >>>= 8;
					component |= _ << 24;
				}
			}
			return component;
		}
		
		public static function applyReadOffset(component	: uint, 
											   offset		: int) : uint
		{
			var newComponent : uint = 0;
			
			for (var i : uint = 0; i < 4; ++i)
			{
				var currentRead : uint = (component >>> (8 * i)) & 0xff;
				
				if (currentRead != _)
					currentRead += offset;
				
				newComponent |= currentRead << (8 * i)
			}
			
			return newComponent;
		}
		
		public static function applyMask(component : uint, mask : uint) : uint
		{
			var result : uint = 0;
			
			for (var i : uint = 0; i < 4; ++i)
			{
				if (getReadAtIndex(i, mask) != _)
					result |= _ << (8 * i);
				else
					result |= getReadAtIndex(i, component) << (8 * i);
			}
			
			return result;
		}
		
		public static function generateReadSwizzle(component : uint) : uint
		{
			var i			: uint;
			var lastChar	: uint = _;
			
			// retrieve first char
			for (i = 0; i < 4; ++i)
				if (getReadAtIndex(i, component) != _)
				{
					lastChar = getReadAtIndex(i, component);
					break;
				}
			
			if (lastChar == _)
				throw new Error('Components is empty.');
			
			var result : uint = 0;
			for (i = 0; i < 4; ++i)
			{
				var char : uint = getReadAtIndex(i, component);
				if (char != _)
					lastChar = char;
				
				result |= lastChar << (2 * i);
			}
			
			return result;
		}
		
		public static function generateWriteMask(component : uint) : uint
		{
			var result : uint = 0;
			
			if (getReadAtIndex(0, component) != _)
				result |= 0x1;
			if (getReadAtIndex(1, component) != _)
				result |= 0x2;
			if (getReadAtIndex(2, component) != _)
				result |= 0x4;
			if (getReadAtIndex(3, component) != _)
				result |= 0x8;
			
			return result;
		}
		
		public static function isEmpty(component : uint) : Boolean
		{
			return component == (_ | (_ << 8) | (_ << 16) | (_ << 24));
		}
		
		/**
		 * Is the current component of the form /^[xyzw]+[_]*$/ 
		 */		
		public static function hasHoles(components:uint):Boolean
		{
			var hasHoles	: Boolean = false;
			var i			: uint;
			
			for (i = 0; i < 4; ++i)
				if (getReadAtIndex(i, components) == _)
					return true;
				else
					break;
			
			for (; i < 4; ++i)
				if (getReadAtIndex(i, components) == _)
					break;
			
			for (; i < 4; ++i)
				if (getReadAtIndex(i, components) != _)
					return true;
			
			return false;
		}
		
		public static function getReadAtIndex(offset	: uint,
											  component	: uint) : uint
		{
			if (offset > W)
				throw new ArgumentError('Invalid offset');
			
			var result : uint = (component >>> (8 * offset)) & 0xff;
			
			if (result > _)
				throw new ArgumentError('Invalid Component');
			
			return result;
		}
		
		public static function getMinReadOffset(component : uint) : uint
		{
			var minRead : uint = 4;
			
			for (var i : uint = 0; i < 4; ++i)
			{
				var currentRead : uint = getReadAtIndex(i, component);
				if (currentRead < minRead)
					minRead = currentRead;
			}
			
			return minRead;
		}
		
		public static function getMaxReadOffset(component : uint) : int
		{
			var maxRead : int = -1;
			
			for (var i : uint = 0; i < 4; ++i)
			{
				var currentRead : uint = getReadAtIndex(i, component);
				
				// cast to int: workaround iOS int VS uint bug
				if (currentRead > int(maxRead) && currentRead != _)
					maxRead = currentRead;
			}
			
			return maxRead;
		}
		
		public static function getMinWriteOffset(component : uint) : uint
		{
			for (var i : uint = 0; i < 4; ++i)
				if (getReadAtIndex(i, component) != _)
					break;
			return i;
		}
		
		public static function getMaxWriteOffset(component : uint) : uint
		{
			for (var i : int = 3; i >= 0; --i)
				if (getReadAtIndex(i, component) != _)
					break;
			
			return i;
		}
		
		public static function stringToComponent(string : String) : uint
		{
			var result : uint = 0;
			
			for (var i : uint = 0; i < 4; ++i)
				switch (string.charAt(i))
				{
					case 'x': case 'X': case 'r': case 'R':
						result |= X << (8 * i);
						break;
					
					case 'y': case 'Y': case 'g': case 'G':
						result |= Y << (8 * i);
						break;
					
					case 'z': case 'Z': case 'b': case 'B':
						result |= Z << (8 * i);
						break;
					
					case 'w': case 'W': case 'a': case 'A':
						result |= W << (8 * i);
						break;
					
					case '_': case '':
						result |= _ << (8 * i);
						break;
					
					default:
						throw new Error('Invalid string.');
				}
			
			return result;
		}
		
		public static function componentToString(component : uint) : String
		{
			var result : String = '';
			
			for (var i : uint = 0; i < 4; ++i)
				result += STRINGS[getReadAtIndex(i, component)];
			
			return result;
		}
		
		public static function swizzleToString(swizzle : uint) : String
		{
			return SWIZZLE_STRINGS[swizzle];
		}
		
		public static function maskToString(mask : uint) : String
		{
			return MASK_STRINGS[mask];
		}
	}
}
