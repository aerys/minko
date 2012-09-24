package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	
	/**
	 * The BillboardsGeometry stores a list of 2D billboards meant to be used
	 * in particle effects or 2D sprites rendering.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class BillboardsGeometry extends Geometry
	{
		public static const BILLBOARD_FORMAT	: VertexFormat	= new VertexFormat(
			VertexComponent.XY,
			VertexComponent.ID
		);
		
		public function BillboardsGeometry(numQuads : uint)
		{
			super();
			
			initialize(numQuads);
		}
		
		private function initialize(numBillboards : uint) : void
		{
			var vertices	: Vector.<Number>	= new <Number>[];
			var indices		: Vector.<uint>		= new <uint>[];
			
			for (var billboardId : int = 0; billboardId < numBillboards; ++billboardId)
			{
				vertices.push(
					-0.5, 0.5, billboardId,
					0.5, 0.5, billboardId,
					0.5, -0.5, billboardId,
					-0.5, -0.5, billboardId
				);
				
				indices.push(
					billboardId * 4, billboardId * 4 + 2, billboardId * 4 + 1,
					billboardId * 4, billboardId * 4 + 3, billboardId * 4 + 2
				);
			}
			
			setVertexStream(VertexStream.fromVector(StreamUsage.STATIC, BILLBOARD_FORMAT, vertices), 0);
			indexStream = IndexStream.fromVector(StreamUsage.STATIC, indices);
		}
	}
}