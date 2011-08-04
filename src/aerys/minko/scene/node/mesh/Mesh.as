package aerys.minko.scene.node.mesh
{
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	public class Mesh extends AbstractScene implements IMesh
	{
		use namespace minko;
		use namespace minko_stream;
		
		private static var _id : uint = 0;
		
		protected var _vertexStream	: IVertexStream	= null;
		protected var _indexStream	: IndexStream	= null;
		
		public function get version() : uint
		{
			return _vertexStream.version + _indexStream.version;
		}
		
		public function get vertexStream() : IVertexStream
		{
			return _vertexStream;
		}
		
		public function set vertexStream(value : IVertexStream) : void
		{
			_vertexStream = value;
		}

		public function get indexStream() : IndexStream
		{
			return _indexStream;
		}
		
		public function set indexStream(value : IndexStream) : void
		{
			_indexStream = value;
		}

		public function Mesh(vertexStream	: IVertexStream	= null,
							 indexStream	: IndexStream 	= null)
		{
			super();
			
			initialize(vertexStream, indexStream);
		}
		
		private function initialize(vertexStream	: IVertexStream,
									indexStream		: IndexStream) : void
		{
			_vertexStream = vertexStream;
			_indexStream = indexStream;
			
			if (!_indexStream && _vertexStream)
			{
				_indexStream = new IndexStream(null,
											   vertexStream.length,
											   vertexStream.dynamic);
			}
			
			actions[0] = MeshAction.meshAction;
		}
		
		/**
		 * Create a new copy of the Mesh object with the specified VertexFormat.
		 * 
		 * <p>
		 * If no VertexFormat is specified, the original VertexFormat of the Mesh
		 * will be used. Calling the "clone" method and specifying a VertexFormat
		 * is a good way to create a lighter Mesh by copying only the required
		 * vertex components data.
		 * </p>
		 *  
		 * @param vertexFormat
		 * @return 
		 * 
		 */
		public function clone(vertexFormat : VertexFormat = null) : IMesh
		{
			vertexFormat ||= _vertexStream.format.clone();
			
			return createMesh(Vector.<IMesh>([this]), vertexFormat);
		}
		
		/**
		 * The "merge" method will takes multiple Mesh objects and will
		 * merge all their data to create a single and only Mesh out of it.
		 * 
		 * <p>
		 * Because all the Mesh objects can have multiple VertexFormat, the
		 * result Mesh VertexFormat will be the biggest common set of
		 * VertexComponent of all the merged meshes.
		 * </p>
		 * 
		 * @param meshes
		 * @return 
		 * 
		 */
		public static function merge(meshes : Vector.<IMesh>) : IMesh
		{
			// insersect all meshes vertexformats
			var numMeshes		: uint			= meshes.length;
			var vertexFormat	: VertexFormat	= meshes[0].vertexStream.format.clone();
			
			for (var meshId : uint = 1; meshId < numMeshes; ++meshId)
				vertexFormat.intersectWith(meshes[meshId].vertexStream.format);
			
			return createMesh(meshes, vertexFormat);
		}
		
		private static function createMesh(meshes		: Vector.<IMesh>,
										   vertexFormat	: VertexFormat) : IMesh
		{
			
			var newIndexStreamData 			: Vector.<uint>				= new Vector.<uint>();
			var newVertexStreamData			: Vector.<Number>			= new Vector.<Number>();
			
			var components					: Vector.<VertexComponent>	= vertexFormat.componentList;
			var numComponents				: uint						= components.length;
			var componentOffsets			: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentSizes				: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDwordsPerVertex	: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDatas				: Vector.<Vector.<Number>>	= new Vector.<Vector.<Number>>(numComponents, true);
			
			var numMeshes 					: uint 						= meshes.length;
			var totalVertices				: int						= 0;
			var totalIndices				: int						= 0;
			
			for (var i : uint = 0; i < numMeshes; ++i)
			{
				var mesh			: IMesh			= meshes[i];
				var vertexStream	: IVertexStream	= mesh.vertexStream;
			
				// append index data
				var indexData	: Vector.<uint>		= mesh.indexStream._data;
				var indexCount	: uint				= indexData.length;
				var indexOffset	: uint				= newVertexStreamData.length / vertexFormat.dwordsPerVertex;
				
				for (var j : int = 0; j < indexCount; ++j, ++totalIndices)
					newIndexStreamData[totalIndices] = indexData[j] + indexOffset;
				
				// get offsets, sizes, and buffers for components in the current mesh.
				for (var k : int = 0; k < numComponents; ++k)
				{
					var vertexComponent	: VertexComponent	= components[k];
					var subVertexStream	: VertexStream		= vertexStream.getSubStreamByComponent(vertexComponent);
					var subvertexFormat	: VertexFormat		= subVertexStream.format;
					
					componentOffsets[k]			= subvertexFormat.getOffsetForComponent(vertexComponent);
					componentDwordsPerVertex[k]	= subvertexFormat.dwordsPerVertex;
					componentSizes[k]			= vertexComponent.dwords;
					componentDatas[k]			= subVertexStream._data;
				}
				
				// push vertex data into the new buffer.
				var numVertices : uint 	= mesh.vertexStream.length;
				
				for (var vertexId : uint = 0; vertexId < numVertices; ++vertexId)
				{
					for (var componentId : int = 0; componentId < numComponents; ++componentId)
					{
						var vertexData		: Vector.<Number>	= componentDatas[componentId];
						var componentSize	: uint				= componentSizes[componentId];
						var componentOffset	: uint				= componentOffsets[componentId]
																  + vertexId * componentDwordsPerVertex[componentId];
						var componentLimit	: uint				= componentSize + componentOffset;
						
						for (var n : int = componentOffset; n < componentLimit; ++n, ++totalVertices)
							newVertexStreamData[totalVertices] = vertexData[n];
					}
				}
			}
			
			// avoid copying data vectors
			var newIndexStream		: IndexStream	= new IndexStream();
			var newVertexStream		: VertexStream	= new VertexStream(null, vertexFormat);
			
			newIndexStream._data = newIndexStreamData;
			newIndexStream.invalidate();
			
			newVertexStream._data = newVertexStreamData;
			newVertexStream.invalidate();
			
			return new Mesh(newVertexStream, newIndexStream);
		}
		
	}
}