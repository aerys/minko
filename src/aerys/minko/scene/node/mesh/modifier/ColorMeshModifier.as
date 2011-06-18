package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	import aerys.minko.scene.node.mesh.modifier.AbstractMeshModifier;
	
	public class ColorMeshModifier extends AbstractMeshModifier
	{
		private static const VERTEX_FORMAT	: VertexFormat	= new VertexFormat(VertexComponent.RGB);
		
		private var _colors	: Vector.<uint>	= null;
		
		public function ColorMeshModifier(target : IMesh, colors : Vector.<uint>)
		{
			super(target);
			
			_colors = colors.concat();
			
			_vertexStreamList = target.vertexStreamList.clone();
			
			initialize();
		}
		
		private function initialize() : void
		{
			var numVertices	: int				= _vertexStreamList.length;
			var colors 		: Vector.<Number> 	= new Vector.<Number>(numVertices * 3);
			
			for (var i : int = 0; i < numVertices; ++i)
			{
				var color : uint = _colors[int(i % _colors.length)];
				
				colors[int(i * 3)] = ((color >> 16) & 0xff) / 255.;
				colors[int(i * 3 + 1)] = ((color >> 8) & 0xff) / 255.;
				colors[int(i * 3 + 2)] = (color & 0xff) / 255.;
			}
			
			_vertexStreamList.pushVertexStream(new VertexStream(colors, VERTEX_FORMAT));
		}
	}
}