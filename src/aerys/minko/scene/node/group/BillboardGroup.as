package aerys.minko.scene.node.group
{
	import aerys.minko.type.math.Transform3D;
	import aerys.minko.type.math.Vector4;

	public class BillboardGroup extends Group
	{
		private static const UP				: Vector4	= new Vector4(0., -1., 0.);
		
		private static var _id				: uint		= 0;
		
		private var _local			: Transform3D	= new Transform3D();
		private var _localCamera	: Vector4		= new Vector4();
		
		public function BillboardGroup(...children)
		{
			super(children);
		}
	}
}