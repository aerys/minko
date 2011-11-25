package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.iterator.VertexIterator;
	import aerys.minko.type.stream.iterator.VertexReference;

	public class MirrorMeshModifier extends AbstractMeshModifier
	{
		public function MirrorMeshModifier(target : IMesh, direction : Vector4)
		{
			super(target);

			var components : Vector.<VertexComponent> = new <VertexComponent>
			[
				VertexComponent.XYZ, VertexComponent.NORMAL, VertexComponent.TANGENT
			];

			for each (var component : VertexComponent in components)
			{
				var stream : VertexStream = vertexStream.getSubStreamByComponent(component);
				if (stream)
				{
					mirror(stream, component, direction);
				}
			}
		}
		
		private function mirror(stream : VertexStream, component : VertexComponent, direction : Vector4) : void
		{
			var vector : Vector4 = new Vector4;
			var vertices : VertexIterator = new VertexIterator(stream);
			for each (var vertex : VertexReference in vertices)
			{
				vector.set
				(
					vertex[component.fields[0]],
					vertex[component.fields[1]],
					vertex[component.fields[2]]
				);
				vector.mirror (direction);
				vertex[component.fields[0]] = vector.x;
				vertex[component.fields[1]] = vector.y;
				vertex[component.fields[2]] = vector.z;
			}
		}
	}
}