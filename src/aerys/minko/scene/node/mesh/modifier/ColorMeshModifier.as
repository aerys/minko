package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class ColorMeshModifier extends AbstractMeshModifier
	{
		private static const RGB_FORMAT		: VertexFormat	= new VertexFormat(VertexComponent.RGB);
		private static const RGBA_FORMAT	: VertexFormat	= new VertexFormat(VertexComponent.RGBA);
		
		private var _colors	: Vector.<uint>	= null;
		
		public function ColorMeshModifier(target 	: IMesh,
										  colors 	: Vector.<uint>,
										  withAlpha	: Boolean	= false)
		{
			_colors = colors.concat();
			
			super(target, getColorStream(withAlpha));
		}
		
		private function getColorStream(withAlpha : Boolean) : VertexStream
		{
			var numVertices	: int				= target.vertexStream.length;
			var colors 		: Vector.<Number> 	= new Vector.<Number>(numVertices * 3);
			var ii			: int				= 0;
			var size		: int				= withAlpha ? 4 : 3;
			
			for (var i : int = 0; i < numVertices; ++i)
			{
				var color : uint = _colors[int(i % _colors.length)];
				
				ii = i * size;
				
				colors[ii] = ((color >> 16) & 0xff) / 255.;
				colors[int(ii + 1)] = ((color >> 8) & 0xff) / 255.;
				colors[int(ii + 2)] = (color & 0xff) / 255.;
				if (withAlpha)
					colors[int(ii + 3)] = (color >>> 24) / 255.;
			}
			
			return new VertexStream(colors, withAlpha ? RGBA_FORMAT : RGB_FORMAT);
		}
	}
}