package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.iterator.VertexIterator;
	import aerys.minko.type.stream.iterator.VertexReference;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.geom.Rectangle;
	
	public class AtlasUVModifier extends AbstractMeshModifier
	{
		use namespace minko_stream;
		
		public function AtlasUVModifier(target : IMesh, atlasSize : uint, rectangle : Rectangle)
		{
			super(target);
			
			initialize(atlasSize, rectangle);
		}
		
		private function initialize(atlasSize : uint, rectangle : Rectangle) : void
		{
			var offsetU		: Number			= rectangle.x / atlasSize;
			var offsetV		: Number			= rectangle.y / atlasSize;
			var scaleU		: Number			= rectangle.width / atlasSize;
			var scaleV		: Number			= rectangle.height / atlasSize;
			var vertices 	: VertexIterator	= new VertexIterator(vertexStream);
			var newUVs		: Vector.<Number>	= new Vector.<Number>();
			var i			: int				= 0;
			
			for each (var vertex : VertexReference in vertices)
			{
				var u : Number = vertex.u;
				var v : Number = vertex.v;
				
				// adjust coordinates to allow texture repeat (should work most of the time)
				newUVs[int(i * 2)] = offsetU + (u == 1. ? 1. : u - Math.floor(u)) * scaleU;
				newUVs[int(i * 2 + 1)] = offsetV + (v == 1. ? 1. : v - Math.floor(v)) * scaleV;
				
				++i;
			}
			
			var stream : VertexStream = new VertexStream(newUVs, new VertexFormat(VertexComponent.UV));
			
			_vertexStreamList.pushVertexStream(stream, true);
		}
	}
}