package aerys.minko.render.shader.compiler.register
{
	/**
	 * Swizzling constants (generated).
	 */
	public final class RegisterSwizzling
	{
		private static const X	: uint = 0x0;
		private static const Y	: uint = 0x1;
		private static const Z	: uint = 0x2;
		private static const W	: uint = 0x3;
		
		public static const XXXX : uint = X | (X << 2) | (X << 4) | (X << 6);
		public static const XXXY : uint = X | (X << 2) | (X << 4) | (Y << 6);
		public static const XXXZ : uint = X | (X << 2) | (X << 4) | (Z << 6);
		public static const XXXW : uint = X | (X << 2) | (X << 4) | (W << 6);
		public static const XXYX : uint = X | (X << 2) | (Y << 4) | (X << 6);
		public static const XXYY : uint = X | (X << 2) | (Y << 4) | (Y << 6);
		public static const XXYZ : uint = X | (X << 2) | (Y << 4) | (Z << 6);
		public static const XXYW : uint = X | (X << 2) | (Y << 4) | (W << 6);
		public static const XXZX : uint = X | (X << 2) | (Z << 4) | (X << 6);
		public static const XXZY : uint = X | (X << 2) | (Z << 4) | (Y << 6);
		public static const XXZZ : uint = X | (X << 2) | (Z << 4) | (Z << 6);
		public static const XXZW : uint = X | (X << 2) | (Z << 4) | (W << 6);
		public static const XXWX : uint = X | (X << 2) | (W << 4) | (X << 6);
		public static const XXWY : uint = X | (X << 2) | (W << 4) | (Y << 6);
		public static const XXWZ : uint = X | (X << 2) | (W << 4) | (Z << 6);
		public static const XXWW : uint = X | (X << 2) | (W << 4) | (W << 6);
		public static const XYXX : uint = X | (Y << 2) | (X << 4) | (X << 6);
		public static const XYXY : uint = X | (Y << 2) | (X << 4) | (Y << 6);
		public static const XYXZ : uint = X | (Y << 2) | (X << 4) | (Z << 6);
		public static const XYXW : uint = X | (Y << 2) | (X << 4) | (W << 6);
		public static const XYYX : uint = X | (Y << 2) | (Y << 4) | (X << 6);
		public static const XYYY : uint = X | (Y << 2) | (Y << 4) | (Y << 6);
		public static const XYYZ : uint = X | (Y << 2) | (Y << 4) | (Z << 6);
		public static const XYYW : uint = X | (Y << 2) | (Y << 4) | (W << 6);
		public static const XYZX : uint = X | (Y << 2) | (Z << 4) | (X << 6);
		public static const XYZY : uint = X | (Y << 2) | (Z << 4) | (Y << 6);
		public static const XYZZ : uint = X | (Y << 2) | (Z << 4) | (Z << 6);
		public static const XYZW : uint = X | (Y << 2) | (Z << 4) | (W << 6);
		public static const XYWX : uint = X | (Y << 2) | (W << 4) | (X << 6);
		public static const XYWY : uint = X | (Y << 2) | (W << 4) | (Y << 6);
		public static const XYWZ : uint = X | (Y << 2) | (W << 4) | (Z << 6);
		public static const XYWW : uint = X | (Y << 2) | (W << 4) | (W << 6);
		public static const XZXX : uint = X | (Z << 2) | (X << 4) | (X << 6);
		public static const XZXY : uint = X | (Z << 2) | (X << 4) | (Y << 6);
		public static const XZXZ : uint = X | (Z << 2) | (X << 4) | (Z << 6);
		public static const XZXW : uint = X | (Z << 2) | (X << 4) | (W << 6);
		public static const XZYX : uint = X | (Z << 2) | (Y << 4) | (X << 6);
		public static const XZYY : uint = X | (Z << 2) | (Y << 4) | (Y << 6);
		public static const XZYZ : uint = X | (Z << 2) | (Y << 4) | (Z << 6);
		public static const XZYW : uint = X | (Z << 2) | (Y << 4) | (W << 6);
		public static const XZZX : uint = X | (Z << 2) | (Z << 4) | (X << 6);
		public static const XZZY : uint = X | (Z << 2) | (Z << 4) | (Y << 6);
		public static const XZZZ : uint = X | (Z << 2) | (Z << 4) | (Z << 6);
		public static const XZZW : uint = X | (Z << 2) | (Z << 4) | (W << 6);
		public static const XZWX : uint = X | (Z << 2) | (W << 4) | (X << 6);
		public static const XZWY : uint = X | (Z << 2) | (W << 4) | (Y << 6);
		public static const XZWZ : uint = X | (Z << 2) | (W << 4) | (Z << 6);
		public static const XZWW : uint = X | (Z << 2) | (W << 4) | (W << 6);
		public static const XWXX : uint = X | (W << 2) | (X << 4) | (X << 6);
		public static const XWXY : uint = X | (W << 2) | (X << 4) | (Y << 6);
		public static const XWXZ : uint = X | (W << 2) | (X << 4) | (Z << 6);
		public static const XWXW : uint = X | (W << 2) | (X << 4) | (W << 6);
		public static const XWYX : uint = X | (W << 2) | (Y << 4) | (X << 6);
		public static const XWYY : uint = X | (W << 2) | (Y << 4) | (Y << 6);
		public static const XWYZ : uint = X | (W << 2) | (Y << 4) | (Z << 6);
		public static const XWYW : uint = X | (W << 2) | (Y << 4) | (W << 6);
		public static const XWZX : uint = X | (W << 2) | (Z << 4) | (X << 6);
		public static const XWZY : uint = X | (W << 2) | (Z << 4) | (Y << 6);
		public static const XWZZ : uint = X | (W << 2) | (Z << 4) | (Z << 6);
		public static const XWZW : uint = X | (W << 2) | (Z << 4) | (W << 6);
		public static const XWWX : uint = X | (W << 2) | (W << 4) | (X << 6);
		public static const XWWY : uint = X | (W << 2) | (W << 4) | (Y << 6);
		public static const XWWZ : uint = X | (W << 2) | (W << 4) | (Z << 6);
		public static const XWWW : uint = X | (W << 2) | (W << 4) | (W << 6);
		public static const YXXX : uint = Y | (X << 2) | (X << 4) | (X << 6);
		public static const YXXY : uint = Y | (X << 2) | (X << 4) | (Y << 6);
		public static const YXXZ : uint = Y | (X << 2) | (X << 4) | (Z << 6);
		public static const YXXW : uint = Y | (X << 2) | (X << 4) | (W << 6);
		public static const YXYX : uint = Y | (X << 2) | (Y << 4) | (X << 6);
		public static const YXYY : uint = Y | (X << 2) | (Y << 4) | (Y << 6);
		public static const YXYZ : uint = Y | (X << 2) | (Y << 4) | (Z << 6);
		public static const YXYW : uint = Y | (X << 2) | (Y << 4) | (W << 6);
		public static const YXZX : uint = Y | (X << 2) | (Z << 4) | (X << 6);
		public static const YXZY : uint = Y | (X << 2) | (Z << 4) | (Y << 6);
		public static const YXZZ : uint = Y | (X << 2) | (Z << 4) | (Z << 6);
		public static const YXZW : uint = Y | (X << 2) | (Z << 4) | (W << 6);
		public static const YXWX : uint = Y | (X << 2) | (W << 4) | (X << 6);
		public static const YXWY : uint = Y | (X << 2) | (W << 4) | (Y << 6);
		public static const YXWZ : uint = Y | (X << 2) | (W << 4) | (Z << 6);
		public static const YXWW : uint = Y | (X << 2) | (W << 4) | (W << 6);
		public static const YYXX : uint = Y | (Y << 2) | (X << 4) | (X << 6);
		public static const YYXY : uint = Y | (Y << 2) | (X << 4) | (Y << 6);
		public static const YYXZ : uint = Y | (Y << 2) | (X << 4) | (Z << 6);
		public static const YYXW : uint = Y | (Y << 2) | (X << 4) | (W << 6);
		public static const YYYX : uint = Y | (Y << 2) | (Y << 4) | (X << 6);
		public static const YYYY : uint = Y | (Y << 2) | (Y << 4) | (Y << 6);
		public static const YYYZ : uint = Y | (Y << 2) | (Y << 4) | (Z << 6);
		public static const YYYW : uint = Y | (Y << 2) | (Y << 4) | (W << 6);
		public static const YYZX : uint = Y | (Y << 2) | (Z << 4) | (X << 6);
		public static const YYZY : uint = Y | (Y << 2) | (Z << 4) | (Y << 6);
		public static const YYZZ : uint = Y | (Y << 2) | (Z << 4) | (Z << 6);
		public static const YYZW : uint = Y | (Y << 2) | (Z << 4) | (W << 6);
		public static const YYWX : uint = Y | (Y << 2) | (W << 4) | (X << 6);
		public static const YYWY : uint = Y | (Y << 2) | (W << 4) | (Y << 6);
		public static const YYWZ : uint = Y | (Y << 2) | (W << 4) | (Z << 6);
		public static const YYWW : uint = Y | (Y << 2) | (W << 4) | (W << 6);
		public static const YZXX : uint = Y | (Z << 2) | (X << 4) | (X << 6);
		public static const YZXY : uint = Y | (Z << 2) | (X << 4) | (Y << 6);
		public static const YZXZ : uint = Y | (Z << 2) | (X << 4) | (Z << 6);
		public static const YZXW : uint = Y | (Z << 2) | (X << 4) | (W << 6);
		public static const YZYX : uint = Y | (Z << 2) | (Y << 4) | (X << 6);
		public static const YZYY : uint = Y | (Z << 2) | (Y << 4) | (Y << 6);
		public static const YZYZ : uint = Y | (Z << 2) | (Y << 4) | (Z << 6);
		public static const YZYW : uint = Y | (Z << 2) | (Y << 4) | (W << 6);
		public static const YZZX : uint = Y | (Z << 2) | (Z << 4) | (X << 6);
		public static const YZZY : uint = Y | (Z << 2) | (Z << 4) | (Y << 6);
		public static const YZZZ : uint = Y | (Z << 2) | (Z << 4) | (Z << 6);
		public static const YZZW : uint = Y | (Z << 2) | (Z << 4) | (W << 6);
		public static const YZWX : uint = Y | (Z << 2) | (W << 4) | (X << 6);
		public static const YZWY : uint = Y | (Z << 2) | (W << 4) | (Y << 6);
		public static const YZWZ : uint = Y | (Z << 2) | (W << 4) | (Z << 6);
		public static const YZWW : uint = Y | (Z << 2) | (W << 4) | (W << 6);
		public static const YWXX : uint = Y | (W << 2) | (X << 4) | (X << 6);
		public static const YWXY : uint = Y | (W << 2) | (X << 4) | (Y << 6);
		public static const YWXZ : uint = Y | (W << 2) | (X << 4) | (Z << 6);
		public static const YWXW : uint = Y | (W << 2) | (X << 4) | (W << 6);
		public static const YWYX : uint = Y | (W << 2) | (Y << 4) | (X << 6);
		public static const YWYY : uint = Y | (W << 2) | (Y << 4) | (Y << 6);
		public static const YWYZ : uint = Y | (W << 2) | (Y << 4) | (Z << 6);
		public static const YWYW : uint = Y | (W << 2) | (Y << 4) | (W << 6);
		public static const YWZX : uint = Y | (W << 2) | (Z << 4) | (X << 6);
		public static const YWZY : uint = Y | (W << 2) | (Z << 4) | (Y << 6);
		public static const YWZZ : uint = Y | (W << 2) | (Z << 4) | (Z << 6);
		public static const YWZW : uint = Y | (W << 2) | (Z << 4) | (W << 6);
		public static const YWWX : uint = Y | (W << 2) | (W << 4) | (X << 6);
		public static const YWWY : uint = Y | (W << 2) | (W << 4) | (Y << 6);
		public static const YWWZ : uint = Y | (W << 2) | (W << 4) | (Z << 6);
		public static const YWWW : uint = Y | (W << 2) | (W << 4) | (W << 6);
		public static const ZXXX : uint = Z | (X << 2) | (X << 4) | (X << 6);
		public static const ZXXY : uint = Z | (X << 2) | (X << 4) | (Y << 6);
		public static const ZXXZ : uint = Z | (X << 2) | (X << 4) | (Z << 6);
		public static const ZXXW : uint = Z | (X << 2) | (X << 4) | (W << 6);
		public static const ZXYX : uint = Z | (X << 2) | (Y << 4) | (X << 6);
		public static const ZXYY : uint = Z | (X << 2) | (Y << 4) | (Y << 6);
		public static const ZXYZ : uint = Z | (X << 2) | (Y << 4) | (Z << 6);
		public static const ZXYW : uint = Z | (X << 2) | (Y << 4) | (W << 6);
		public static const ZXZX : uint = Z | (X << 2) | (Z << 4) | (X << 6);
		public static const ZXZY : uint = Z | (X << 2) | (Z << 4) | (Y << 6);
		public static const ZXZZ : uint = Z | (X << 2) | (Z << 4) | (Z << 6);
		public static const ZXZW : uint = Z | (X << 2) | (Z << 4) | (W << 6);
		public static const ZXWX : uint = Z | (X << 2) | (W << 4) | (X << 6);
		public static const ZXWY : uint = Z | (X << 2) | (W << 4) | (Y << 6);
		public static const ZXWZ : uint = Z | (X << 2) | (W << 4) | (Z << 6);
		public static const ZXWW : uint = Z | (X << 2) | (W << 4) | (W << 6);
		public static const ZYXX : uint = Z | (Y << 2) | (X << 4) | (X << 6);
		public static const ZYXY : uint = Z | (Y << 2) | (X << 4) | (Y << 6);
		public static const ZYXZ : uint = Z | (Y << 2) | (X << 4) | (Z << 6);
		public static const ZYXW : uint = Z | (Y << 2) | (X << 4) | (W << 6);
		public static const ZYYX : uint = Z | (Y << 2) | (Y << 4) | (X << 6);
		public static const ZYYY : uint = Z | (Y << 2) | (Y << 4) | (Y << 6);
		public static const ZYYZ : uint = Z | (Y << 2) | (Y << 4) | (Z << 6);
		public static const ZYYW : uint = Z | (Y << 2) | (Y << 4) | (W << 6);
		public static const ZYZX : uint = Z | (Y << 2) | (Z << 4) | (X << 6);
		public static const ZYZY : uint = Z | (Y << 2) | (Z << 4) | (Y << 6);
		public static const ZYZZ : uint = Z | (Y << 2) | (Z << 4) | (Z << 6);
		public static const ZYZW : uint = Z | (Y << 2) | (Z << 4) | (W << 6);
		public static const ZYWX : uint = Z | (Y << 2) | (W << 4) | (X << 6);
		public static const ZYWY : uint = Z | (Y << 2) | (W << 4) | (Y << 6);
		public static const ZYWZ : uint = Z | (Y << 2) | (W << 4) | (Z << 6);
		public static const ZYWW : uint = Z | (Y << 2) | (W << 4) | (W << 6);
		public static const ZZXX : uint = Z | (Z << 2) | (X << 4) | (X << 6);
		public static const ZZXY : uint = Z | (Z << 2) | (X << 4) | (Y << 6);
		public static const ZZXZ : uint = Z | (Z << 2) | (X << 4) | (Z << 6);
		public static const ZZXW : uint = Z | (Z << 2) | (X << 4) | (W << 6);
		public static const ZZYX : uint = Z | (Z << 2) | (Y << 4) | (X << 6);
		public static const ZZYY : uint = Z | (Z << 2) | (Y << 4) | (Y << 6);
		public static const ZZYZ : uint = Z | (Z << 2) | (Y << 4) | (Z << 6);
		public static const ZZYW : uint = Z | (Z << 2) | (Y << 4) | (W << 6);
		public static const ZZZX : uint = Z | (Z << 2) | (Z << 4) | (X << 6);
		public static const ZZZY : uint = Z | (Z << 2) | (Z << 4) | (Y << 6);
		public static const ZZZZ : uint = Z | (Z << 2) | (Z << 4) | (Z << 6);
		public static const ZZZW : uint = Z | (Z << 2) | (Z << 4) | (W << 6);
		public static const ZZWX : uint = Z | (Z << 2) | (W << 4) | (X << 6);
		public static const ZZWY : uint = Z | (Z << 2) | (W << 4) | (Y << 6);
		public static const ZZWZ : uint = Z | (Z << 2) | (W << 4) | (Z << 6);
		public static const ZZWW : uint = Z | (Z << 2) | (W << 4) | (W << 6);
		public static const ZWXX : uint = Z | (W << 2) | (X << 4) | (X << 6);
		public static const ZWXY : uint = Z | (W << 2) | (X << 4) | (Y << 6);
		public static const ZWXZ : uint = Z | (W << 2) | (X << 4) | (Z << 6);
		public static const ZWXW : uint = Z | (W << 2) | (X << 4) | (W << 6);
		public static const ZWYX : uint = Z | (W << 2) | (Y << 4) | (X << 6);
		public static const ZWYY : uint = Z | (W << 2) | (Y << 4) | (Y << 6);
		public static const ZWYZ : uint = Z | (W << 2) | (Y << 4) | (Z << 6);
		public static const ZWYW : uint = Z | (W << 2) | (Y << 4) | (W << 6);
		public static const ZWZX : uint = Z | (W << 2) | (Z << 4) | (X << 6);
		public static const ZWZY : uint = Z | (W << 2) | (Z << 4) | (Y << 6);
		public static const ZWZZ : uint = Z | (W << 2) | (Z << 4) | (Z << 6);
		public static const ZWZW : uint = Z | (W << 2) | (Z << 4) | (W << 6);
		public static const ZWWX : uint = Z | (W << 2) | (W << 4) | (X << 6);
		public static const ZWWY : uint = Z | (W << 2) | (W << 4) | (Y << 6);
		public static const ZWWZ : uint = Z | (W << 2) | (W << 4) | (Z << 6);
		public static const ZWWW : uint = Z | (W << 2) | (W << 4) | (W << 6);
		public static const WXXX : uint = W | (X << 2) | (X << 4) | (X << 6);
		public static const WXXY : uint = W | (X << 2) | (X << 4) | (Y << 6);
		public static const WXXZ : uint = W | (X << 2) | (X << 4) | (Z << 6);
		public static const WXXW : uint = W | (X << 2) | (X << 4) | (W << 6);
		public static const WXYX : uint = W | (X << 2) | (Y << 4) | (X << 6);
		public static const WXYY : uint = W | (X << 2) | (Y << 4) | (Y << 6);
		public static const WXYZ : uint = W | (X << 2) | (Y << 4) | (Z << 6);
		public static const WXYW : uint = W | (X << 2) | (Y << 4) | (W << 6);
		public static const WXZX : uint = W | (X << 2) | (Z << 4) | (X << 6);
		public static const WXZY : uint = W | (X << 2) | (Z << 4) | (Y << 6);
		public static const WXZZ : uint = W | (X << 2) | (Z << 4) | (Z << 6);
		public static const WXZW : uint = W | (X << 2) | (Z << 4) | (W << 6);
		public static const WXWX : uint = W | (X << 2) | (W << 4) | (X << 6);
		public static const WXWY : uint = W | (X << 2) | (W << 4) | (Y << 6);
		public static const WXWZ : uint = W | (X << 2) | (W << 4) | (Z << 6);
		public static const WXWW : uint = W | (X << 2) | (W << 4) | (W << 6);
		public static const WYXX : uint = W | (Y << 2) | (X << 4) | (X << 6);
		public static const WYXY : uint = W | (Y << 2) | (X << 4) | (Y << 6);
		public static const WYXZ : uint = W | (Y << 2) | (X << 4) | (Z << 6);
		public static const WYXW : uint = W | (Y << 2) | (X << 4) | (W << 6);
		public static const WYYX : uint = W | (Y << 2) | (Y << 4) | (X << 6);
		public static const WYYY : uint = W | (Y << 2) | (Y << 4) | (Y << 6);
		public static const WYYZ : uint = W | (Y << 2) | (Y << 4) | (Z << 6);
		public static const WYYW : uint = W | (Y << 2) | (Y << 4) | (W << 6);
		public static const WYZX : uint = W | (Y << 2) | (Z << 4) | (X << 6);
		public static const WYZY : uint = W | (Y << 2) | (Z << 4) | (Y << 6);
		public static const WYZZ : uint = W | (Y << 2) | (Z << 4) | (Z << 6);
		public static const WYZW : uint = W | (Y << 2) | (Z << 4) | (W << 6);
		public static const WYWX : uint = W | (Y << 2) | (W << 4) | (X << 6);
		public static const WYWY : uint = W | (Y << 2) | (W << 4) | (Y << 6);
		public static const WYWZ : uint = W | (Y << 2) | (W << 4) | (Z << 6);
		public static const WYWW : uint = W | (Y << 2) | (W << 4) | (W << 6);
		public static const WZXX : uint = W | (Z << 2) | (X << 4) | (X << 6);
		public static const WZXY : uint = W | (Z << 2) | (X << 4) | (Y << 6);
		public static const WZXZ : uint = W | (Z << 2) | (X << 4) | (Z << 6);
		public static const WZXW : uint = W | (Z << 2) | (X << 4) | (W << 6);
		public static const WZYX : uint = W | (Z << 2) | (Y << 4) | (X << 6);
		public static const WZYY : uint = W | (Z << 2) | (Y << 4) | (Y << 6);
		public static const WZYZ : uint = W | (Z << 2) | (Y << 4) | (Z << 6);
		public static const WZYW : uint = W | (Z << 2) | (Y << 4) | (W << 6);
		public static const WZZX : uint = W | (Z << 2) | (Z << 4) | (X << 6);
		public static const WZZY : uint = W | (Z << 2) | (Z << 4) | (Y << 6);
		public static const WZZZ : uint = W | (Z << 2) | (Z << 4) | (Z << 6);
		public static const WZZW : uint = W | (Z << 2) | (Z << 4) | (W << 6);
		public static const WZWX : uint = W | (Z << 2) | (W << 4) | (X << 6);
		public static const WZWY : uint = W | (Z << 2) | (W << 4) | (Y << 6);
		public static const WZWZ : uint = W | (Z << 2) | (W << 4) | (Z << 6);
		public static const WZWW : uint = W | (Z << 2) | (W << 4) | (W << 6);
		public static const WWXX : uint = W | (W << 2) | (X << 4) | (X << 6);
		public static const WWXY : uint = W | (W << 2) | (X << 4) | (Y << 6);
		public static const WWXZ : uint = W | (W << 2) | (X << 4) | (Z << 6);
		public static const WWXW : uint = W | (W << 2) | (X << 4) | (W << 6);
		public static const WWYX : uint = W | (W << 2) | (Y << 4) | (X << 6);
		public static const WWYY : uint = W | (W << 2) | (Y << 4) | (Y << 6);
		public static const WWYZ : uint = W | (W << 2) | (Y << 4) | (Z << 6);
		public static const WWYW : uint = W | (W << 2) | (Y << 4) | (W << 6);
		public static const WWZX : uint = W | (W << 2) | (Z << 4) | (X << 6);
		public static const WWZY : uint = W | (W << 2) | (Z << 4) | (Y << 6);
		public static const WWZZ : uint = W | (W << 2) | (Z << 4) | (Z << 6);
		public static const WWZW : uint = W | (W << 2) | (Z << 4) | (W << 6);
		public static const WWWX : uint = W | (W << 2) | (W << 4) | (X << 6);
		public static const WWWY : uint = W | (W << 2) | (W << 4) | (Y << 6);
		public static const WWWZ : uint = W | (W << 2) | (W << 4) | (Z << 6);
		public static const WWWW : uint = W | (W << 2) | (W << 4) | (W << 6);
		
		public static const STRINGS:Vector.<String> = Vector.<String>([
			'xxxx', 'yxxx', 'zxxx', 'wxxx', 'xyxx', 'yyxx', 'zyxx', 'wyxx', 
			'xzxx', 'yzxx', 'zzxx', 'wzxx', 'xwxx', 'ywxx', 'zwxx', 'wwxx', 
			'xxyx', 'yxyx', 'zxyx', 'wxyx', 'xyyx', 'yyyx', 'zyyx', 'wyyx', 
			'xzyx', 'yzyx', 'zzyx', 'wzyx', 'xwyx', 'ywyx', 'zwyx', 'wwyx', 
			'xxzx', 'yxzx', 'zxzx', 'wxzx', 'xyzx', 'yyzx', 'zyzx', 'wyzx', 
			'xzzx', 'yzzx', 'zzzx', 'wzzx', 'xwzx', 'ywzx', 'zwzx', 'wwzx', 
			'xxwx', 'yxwx', 'zxwx', 'wxwx', 'xywx', 'yywx', 'zywx', 'wywx', 
			'xzwx', 'yzwx', 'zzwx', 'wzwx', 'xwwx', 'ywwx', 'zwwx', 'wwwx', 
			'xxxy', 'yxxy', 'zxxy', 'wxxy', 'xyxy', 'yyxy', 'zyxy', 'wyxy', 
			'xzxy', 'yzxy', 'zzxy', 'wzxy', 'xwxy', 'ywxy', 'zwxy', 'wwxy', 
			'xxyy', 'yxyy', 'zxyy', 'wxyy', 'xyyy', 'yyyy', 'zyyy', 'wyyy', 
			'xzyy', 'yzyy', 'zzyy', 'wzyy', 'xwyy', 'ywyy', 'zwyy', 'wwyy', 
			'xxzy', 'yxzy', 'zxzy', 'wxzy', 'xyzy', 'yyzy', 'zyzy', 'wyzy', 
			'xzzy', 'yzzy', 'zzzy', 'wzzy', 'xwzy', 'ywzy', 'zwzy', 'wwzy', 
			'xxwy', 'yxwy', 'zxwy', 'wxwy', 'xywy', 'yywy', 'zywy', 'wywy', 
			'xzwy', 'yzwy', 'zzwy', 'wzwy', 'xwwy', 'ywwy', 'zwwy', 'wwwy', 
			'xxxz', 'yxxz', 'zxxz', 'wxxz', 'xyxz', 'yyxz', 'zyxz', 'wyxz', 
			'xzxz', 'yzxz', 'zzxz', 'wzxz', 'xwxz', 'ywxz', 'zwxz', 'wwxz', 
			'xxyz', 'yxyz', 'zxyz', 'wxyz', 'xyyz', 'yyyz', 'zyyz', 'wyyz', 
			'xzyz', 'yzyz', 'zzyz', 'wzyz', 'xwyz', 'ywyz', 'zwyz', 'wwyz', 
			'xxzz', 'yxzz', 'zxzz', 'wxzz', 'xyzz', 'yyzz', 'zyzz', 'wyzz', 
			'xzzz', 'yzzz', 'zzzz', 'wzzz', 'xwzz', 'ywzz', 'zwzz', 'wwzz', 
			'xxwz', 'yxwz', 'zxwz', 'wxwz', 'xywz', 'yywz', 'zywz', 'wywz', 
			'xzwz', 'yzwz', 'zzwz', 'wzwz', 'xwwz', 'ywwz', 'zwwz', 'wwwz', 
			'xxxw', 'yxxw', 'zxxw', 'wxxw', 'xyxw', 'yyxw', 'zyxw', 'wyxw', 
			'xzxw', 'yzxw', 'zzxw', 'wzxw', 'xwxw', 'ywxw', 'zwxw', 'wwxw', 
			'xxyw', 'yxyw', 'zxyw', 'wxyw', 'xyyw', 'yyyw', 'zyyw', 'wyyw', 
			'xzyw', 'yzyw', 'zzyw', 'wzyw', 'xwyw', 'ywyw', 'zwyw', 'wwyw', 
			'xxzw', 'yxzw', 'zxzw', 'wxzw', 'xyzw', 'yyzw', 'zyzw', 'wyzw', 
			'xzzw', 'yzzw', 'zzzw', 'wzzw', 'xwzw', 'ywzw', 'zwzw', 'wwzw', 
			'xxww', 'yxww', 'zxww', 'wxww', 'xyww', 'yyww', 'zyww', 'wyww', 
			'xzww', 'yzww', 'zzww', 'wzww', 'xwww', 'ywww', 'zwww', 'wwww'
		]);
		
		
		private static const TMP_UNPACKED_INPUT_SWIZZLE		: Vector.<uint> = new Vector.<uint>(4, true);
		private static const TMP_UNPACKED_CUSTOM_SWIZZLE	: Vector.<uint> = new Vector.<uint>(4, true);
		
		/**
		 * @fixme should be implemented only with bitmasks to be way faster
		 */
		public static function combine(inputSwizzle : uint, modifier : uint) : uint
		{
			// unpack both of them
			TMP_UNPACKED_INPUT_SWIZZLE[0]	= 0x3 & (inputSwizzle >>> 6);
			TMP_UNPACKED_INPUT_SWIZZLE[1]	= 0x3 & (inputSwizzle >>> 4);
			TMP_UNPACKED_INPUT_SWIZZLE[2]	= 0x3 & (inputSwizzle >>> 2);
			TMP_UNPACKED_INPUT_SWIZZLE[3]	= 0x3 & inputSwizzle;
			
			TMP_UNPACKED_CUSTOM_SWIZZLE[0]	= 0x3 & (modifier >>> 6);
			TMP_UNPACKED_CUSTOM_SWIZZLE[1]	= 0x3 & (modifier >>> 4);
			TMP_UNPACKED_CUSTOM_SWIZZLE[2]	= 0x3 & (modifier >>> 2);
			TMP_UNPACKED_CUSTOM_SWIZZLE[3]	= 0x3 & modifier;
			
			// compose and pack again
			var finalSw : uint =
				(TMP_UNPACKED_CUSTOM_SWIZZLE[3 - TMP_UNPACKED_INPUT_SWIZZLE[0]] << 6) |
				(TMP_UNPACKED_CUSTOM_SWIZZLE[3 - TMP_UNPACKED_INPUT_SWIZZLE[1]] << 4) |
				(TMP_UNPACKED_CUSTOM_SWIZZLE[3 - TMP_UNPACKED_INPUT_SWIZZLE[2]] << 2) |
				(TMP_UNPACKED_CUSTOM_SWIZZLE[3 - TMP_UNPACKED_INPUT_SWIZZLE[3]]);
			
			return finalSw;
		}
		
		public static function createContinuous(size : uint) : uint
		{
			if (size == 1) return RegisterSwizzling.XXXX;
			else if (size == 2) return RegisterSwizzling.XYYY;
			else if (size == 3) return RegisterSwizzling.XYZZ;
			else if (size == 4)	return RegisterSwizzling.XYZW;
			else throw new Error('Invalid size : ' + size.toString());
		}
		
		public static function readOffset(offset	: int, 
										  swizzle	: uint) : uint
		{
			return swizzle + offset * (0x1 | (0x1 << 2) | (0x1 << 4) | (0x1 << 6));
		}
		
		public static function writeOffset(writeOffsetValue	: int,
										   swizzle			: uint) : uint
		{
			var bitsToInject : uint, i : uint;
			if (writeOffsetValue > 0)
			{
				// left shift
				bitsToInject = swizzle & 0x3;
				
				for (i = 0; i < writeOffsetValue; ++i)
					swizzle = (swizzle << 2) | bitsToInject;
			}
			else if (writeOffsetValue < 0)
			{
				// right shift
				writeOffsetValue = - writeOffsetValue;
				bitsToInject = (swizzle >>> 6) << 6;
				
				for (i = 0; i < writeOffsetValue; ++i)
					swizzle = (swizzle >>> 2) | bitsToInject;
			}
			
			return swizzle;
		}
		
		public static function createReadyToUse(readOffsetValue		: uint, 
												writeOffsetValue	: uint, 
												size				: uint) : uint
		{
			var swizzle : uint;
			
			swizzle = createContinuous(size);
			swizzle = readOffset(readOffsetValue, swizzle);
			swizzle = writeOffset(writeOffsetValue, swizzle);
			
			return swizzle;
		}
		
	}
}