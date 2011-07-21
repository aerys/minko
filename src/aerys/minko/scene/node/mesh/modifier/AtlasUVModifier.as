package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.VertexIterator;
	import aerys.minko.type.vertex.VertexReference;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.geom.Rectangle;
	
	public class AtlasUVModifier extends AbstractMeshModifier
	{
		use namespace minko_stream;
		
		public function AtlasUVModifier(target : IMesh, atlasSize : uint, rectangle : Rectangle)
		{
			super(target);
			
			initialize(atlasSize, rectangle);
		}
		
		private function initialize(size : uint, rectangle : Rectangle) : void
		{
			var offsetU		: Number			= rectangle.x / size;
			var offsetY		: Number			= rectangle.y / size;
			var scaleX		: Number			= rectangle.width / size;
			var scaleY		: Number			= rectangle.height / size;
			var vertices 	: VertexIterator	= new VertexIterator(vertexStream);
			var newUVs		: Vector.<Number>	= new Vector.<Number>();
			var i			: int				= 0;
			
			for each (var vertex : VertexReference in vertices)
			{
				newUVs[int(i * 2)] = offsetU + vertex.u * scaleX;
				newUVs[int(i * 2 + 1)] = offsetY + vertex.v * scaleY;
				
				++i;
			}
			
			var stream : VertexStream = new VertexStream(newUVs, new VertexFormat(VertexComponent.UV));
			
			_vertexStreamList.pushVertexStream(stream, true);
		}
	}
}