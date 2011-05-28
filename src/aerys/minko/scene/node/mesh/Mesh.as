package aerys.minko.scene.node.mesh
{
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.IActionTarget;
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class Mesh extends AbstractScene implements IMesh, IActionTarget
	{
		use namespace minko;
		use namespace minko_stream;
		
		private static var _id : uint = 0;
		
		protected var _vertexStreamList	: VertexStreamList	= null;
		protected var _indexStream		: IndexStream		= null;
		
		public function get version() : uint
		{
			return _vertexStreamList.version + _indexStream.version;
		}
		
		public function get vertexStreamList() : VertexStreamList
		{
			return _vertexStreamList;
		}
		
		public function set vertexStreamList(value : VertexStreamList) : void
		{
			_vertexStreamList = value;
		}

		public function get indexStream() : IndexStream
		{
			return _indexStream;
		}
		
		public function set indexStream(value : IndexStream) : void
		{
			_indexStream = value;
		}

		public function Mesh(vertexStreamList	: VertexStreamList	= null,
							 indexStream		: IndexStream 		= null)
		{
			super();
			
			_vertexStreamList = vertexStreamList;
			_indexStream = indexStream;
			
			initialize();
		}
		
		private function initialize() : void
		{
			if (!_indexStream && _vertexStreamList)
				_indexStream = new IndexStream(null, vertexStreamList.length, vertexStreamList.dynamic);
			
			actions[0] = MeshAction.meshAction;
		}
		
		public function clone() : IMesh
		{
			var meshes				: Vector.<IMesh>			= Vector.<IMesh>([this])
			var vertexFormat		: VertexFormat				= _vertexStreamList.format.clone();
			var vertexComponents	: Vector.<VertexComponent>	= computeVertexComponentList(vertexFormat);
			var mergedMesh			: IMesh						= createMesh(meshes, vertexComponents, vertexFormat);
			
			return mergedMesh;
		}
		
		public static function mergeMeshes(meshes : Vector.<IMesh>) : IMesh
		{
			var vertexFormat		: VertexFormat				= computeVertexFormat(meshes);
			var vertexComponents	: Vector.<VertexComponent>	= computeVertexComponentList(vertexFormat);
			var mergedMesh			: IMesh						= createMesh(meshes, vertexComponents, vertexFormat);
			
			return mergedMesh;
		}
		
		private static function computeVertexFormat(meshes : Vector.<IMesh>) : VertexFormat
		{
			var newVertexFormat : VertexFormat = meshes[0].vertexStreamList.format.clone();
			
			for each (var mesh : IMesh in meshes)
				newVertexFormat.intersectWith(mesh.vertexStreamList.format);
			
			return newVertexFormat;	
		}
		
		private static function computeVertexComponentList(vertexFormat : VertexFormat) : Vector.<VertexComponent>
		{
			function sortComponentsByOffsets(component1 : VertexComponent, component2 : VertexComponent) : int {
				return vertexFormat.getOffsetForComponent(component1) - vertexFormat.getOffsetForComponent(component2);
			}
			
			var vertexComponents	: Object					= vertexFormat.components;
			var vertexComponentList : Vector.<VertexComponent>	= new Vector.<VertexComponent>();
			
			for (var key : String in vertexComponents)
				if (vertexComponents[key] != undefined && vertexComponents[key] is VertexComponent)
					vertexComponentList.push(vertexComponents[key] as VertexComponent);
			
			vertexComponentList = vertexComponentList.sort(sortComponentsByOffsets);
			
			return vertexComponentList;
		}
		
		private static function createMesh(meshes			: Vector.<IMesh>,
										   vertexComponents	: Vector.<VertexComponent>,
										   vertexFormat		: VertexFormat) : IMesh
		{
			var newIndexStreamData 				: Vector.<uint>				= new Vector.<uint>();
			var newVertexStreamData				: Vector.<Number>			= new Vector.<Number>();
			
			var componentCount					: uint						= vertexComponents.length;
			var vertexComponentOffsets			: Vector.<uint>				= new Vector.<uint>(componentCount, true);
			var vertexComponentSizes			: Vector.<uint>				= new Vector.<uint>(componentCount, true);
			var vertexComponentDwordsPerVertex	: Vector.<uint>				= new Vector.<uint>(componentCount, true);
			var vertexComponentDatas			: Vector.<Vector.<Number>>	= new Vector.<Vector.<Number>>(componentCount, true);
			
			var meshCount : uint = meshes.length;
			for (var i : uint = 0; i < meshCount; ++i)
			{
				var mesh		: IMesh				= meshes[i];
				
				// append index data
				var indexData	: Vector.<uint>		= mesh.indexStream.getIndices();
				var indexCount	: uint				= indexData.length;
				var indexOffset	: uint				= newVertexStreamData.length / vertexFormat.dwordsPerVertex;
				
				for (var j : uint = 0; j < indexCount; ++j)
					newIndexStreamData.push(indexData[j] + indexOffset);
				
				// get offsets, sizes, and buffers for components in the current mesh.
				for (var k : uint = 0; k < componentCount; ++k)
				{
					var vertexComponent	: VertexComponent	= vertexComponents[k];
					var vertexStream	: VertexStream		= mesh.vertexStreamList.getVertexStreamByComponent(vertexComponent);
					
					vertexComponentOffsets[k]				= vertexStream.format.getOffsetForComponent(vertexComponent);
					vertexComponentDwordsPerVertex[k]		= vertexStream.format.dwordsPerVertex;
					vertexComponentSizes[k]					= vertexComponent.dwords;
					vertexComponentDatas[k]					= vertexStream._data;
				}
				
				// push vertex data into the new buffer.
				var vertexCount : uint = mesh.vertexStreamList.length;
				for (var vertexId : uint = 0; vertexId < vertexCount; ++vertexId)
					for (var componentId : uint = 0; componentId < componentCount; ++componentId)
					{
						var vertexData		: Vector.<Number>	= vertexComponentDatas[componentId];
						var componentSize	: uint				= vertexComponentSizes[componentId];
						var componentOffset	: uint				= vertexComponentOffsets[componentId] + vertexId * vertexComponentDwordsPerVertex[componentId];
						var componentLimit	: uint				= componentSize + componentOffset;
						
						for (var n : uint = componentOffset; n < componentLimit; ++n)
							newVertexStreamData.push(vertexData[n]);
					}
			}
			
			var newIndexStream		: IndexStream		= new IndexStream(newIndexStreamData);
			var newVertexStream		: VertexStream		= new VertexStream(newVertexStreamData, vertexFormat);
			var newVertexStreamList : VertexStreamList	= new VertexStreamList(newVertexStream);
			return new Mesh(newVertexStreamList, newIndexStream);
		}
		
//		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
//		{
//			var transform : TransformManager = query.transform;
//			
//			query.style.set(BasicStyle.WORLD_MATRIX, 			transform.world)
//				  	   .set(BasicStyle.VIEW_MATRIX, 			transform.view)
//				  	   .set(BasicStyle.PROJECTION_MATRIX,		transform.projection)
//				  	   .set(BasicStyle.LOCAL_TO_SCREEN_MATRIX, 	transform.getLocalToScreen());
//			
//			query.draw(vertexStreamList, indexStream);
//		}
		
	}
}