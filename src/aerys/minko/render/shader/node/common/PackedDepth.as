package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.packing.PackScalarToColor;
	import aerys.minko.scene.data.CameraData;

	public class PackedDepth extends Dummy implements IFragmentNode
	{
		public function PackedDepth()
		{
			var result : INode;

			var maxValueParts	: INode = new WorldParameter(4, CameraData, CameraData.Z_FAR_PARTS);
			var maxValue		: INode = new WorldParameter(1, CameraData, CameraData.Z_FAR);

			result = new Depth();
			result = new PackScalarToColor(result, maxValueParts, maxValue);

			super(result);
		}
	}
}
