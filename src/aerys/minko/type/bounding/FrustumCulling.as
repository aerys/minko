package aerys.minko.type.bounding
{
	/**
	 * The FrustumClipping enumeration exposes the required flags to
	 * parametrize the frustum culling process.
	 *
	 * @see Frustum
	 *
	 * @author Jean-Marc Le Roux
	 */
	public final class FrustumCulling
	{
		/**
		 * Disable frustum culling.
		 */
		public static const DISABLED		: uint	= 0;

		/**
		 * Enabled frustum culling on the left plane.
		 */
		public static const LEFT_SPHERE		: uint	= 0x00000001;
		public static const LEFT_BOX		: uint	= 0x00000002;
		public static const LEFT			: uint	= LEFT_BOX | LEFT_SPHERE;
		/**
		 * Enabled frustum culling on the top plane.
		 */
		public static const TOP_SPHERE		: uint	= 0x00000010;
		public static const TOP_BOX			: uint	= 0x00000020;
		public static const TOP				: uint	= TOP_BOX | TOP_SPHERE;
		/**
		 * Enabled frustum culling on the right plane.
		 */
		public static const RIGHT_SPHERE	: uint	= 0x00000100;
		public static const RIGHT_BOX		: uint	= 0x00000200;
		public static const RIGHT			: uint	= RIGHT_BOX | RIGHT_SPHERE;
		/**
		 * Enabled frustum culling on the bottom plane.
		 */
		public static const BOTTOM_SPHERE	: uint	= 0x00001000;
		public static const BOTTOM_BOX		: uint	= 0x00002000;
		public static const BOTTOM			: uint	= BOTTOM_BOX | BOTTOM_SPHERE;
		/**
		 * Enabled frustum culling on the near plane.
		 */
		public static const NEAR_SPHERE		: uint	= 0x00010000;
		public static const NEAR_BOX		: uint	= 0x00020000;
		public static const NEAR			: uint	= NEAR_BOX | NEAR_SPHERE;
		/**
		 * Enabled frustum culling on the far plane.
		 */
		public static const FAR_SPHERE		: uint	= 0x00100000;
		public static const FAR_BOX			: uint	= 0x00200000;
		public static const FAR				: uint	= NEAR_BOX | NEAR_SPHERE;

		/**
		 * Enabled frustum culling.
		 */
		public static const BOX				: uint	= NEAR_BOX | RIGHT_BOX | FAR_BOX | LEFT_BOX
													  | TOP_BOX | BOTTOM_BOX;
		public static const SPHERE			: uint	= NEAR_SPHERE | RIGHT_SPHERE | FAR_SPHERE
													  | LEFT_SPHERE | TOP_SPHERE | BOTTOM_SPHERE;
		public static const ENABLED			: uint	= BOX | SPHERE;
	}
}