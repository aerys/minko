package aerys.minko.type.loader.parser
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;

	public class GeometrySanitizer
	{
		private static const INDEX_LIMIT	: uint = 524270;
		private static const VERTEX_LIMIT	: uint = 65535;
		
		/**
		 * Split vertex and index buffers too big the be rendered in flash.
		 * The index stream limit is 524270, the vertex stream limit is 65536.
		 *  
		 * @param inVertexData
		 * @param inIndexData
		 * @param outVertexDatas
		 * @param outIndexDatas
		 * @param dwordsPerVertex
		 */		
		public static function splitBuffers(inVertexData	: Vector.<Number>,
											inIndexData		: Vector.<uint>,
											outVertexDatas	: Vector.<Vector.<Number>>,
											outIndexDatas	: Vector.<Vector.<uint>>,
											dwordsPerVertex	: uint = 3) : void
		{
			if (inIndexData.length < INDEX_LIMIT && inVertexData.length / dwordsPerVertex < VERTEX_LIMIT)
			{
				outVertexDatas.push(inVertexData);
				outIndexDatas.push(inIndexData);
				return;
			}
			
			while (inIndexData.length != 0)
			{
				var indexDataLength		: uint				= inIndexData.length;
				
				// new buffers
				var partialVertexData	: Vector.<Number>	= new Vector.<Number>();
				var partialIndexData	: Vector.<uint>		= new Vector.<uint>();
				
				// local variables
				var oldVertexIds		: Vector.<int>		= new Vector.<int>(3, true);
				var newVertexIds		: Vector.<int>		= new Vector.<int>(3, true);
				var newVertexNeeded		: Vector.<Boolean>	= new Vector.<Boolean>(3, true);
				
				var usedVerticesDic		: Dictionary		= new Dictionary();		// this dictionary maps old and new indices
				var usedVerticesCount	: uint				= 0;					// this counts elements in the dictionary
				var usedIndicesCount	: uint				= 0;					// how many indices have we used?
				var neededVerticesCount	: uint;
				
				// iterators & limits
				var localVertexId		: uint;
				var dwordId				: uint;
				var dwordIdLimit		: uint;
				
				while (usedIndicesCount < indexDataLength)
				{
					// check if next triangle can fill into the index buffer
					var remainingIndexes	: uint		= INDEX_LIMIT - usedIndicesCount;
					if (remainingIndexes < 3)
						break;
					
					// check if next triangle can fill into the vertex buffer
					var remainingVertices	: uint		= VERTEX_LIMIT - usedVerticesCount;
					
					neededVerticesCount = 0;
					for (localVertexId = 0; localVertexId < 3; ++localVertexId)
					{
						oldVertexIds[localVertexId]		= inIndexData[uint(usedIndicesCount + localVertexId)];
						
						var tmp : Object = usedVerticesDic[oldVertexIds[localVertexId]];
						
						newVertexNeeded[localVertexId]	= tmp == null;
						newVertexIds[localVertexId]		= uint(tmp);
						
						if (newVertexNeeded[localVertexId])
							++neededVerticesCount;
					}
					
					if (remainingVertices < neededVerticesCount)
						break;
					
					// it fills, let insert the triangle
					for (localVertexId = 0; localVertexId < 3; ++localVertexId)
					{
						
						if (newVertexNeeded[localVertexId])
						{
							// copy current vertex into the new array
							dwordId			= oldVertexIds[localVertexId] * dwordsPerVertex;
							dwordIdLimit	= dwordId + dwordsPerVertex;
							for (; dwordId < dwordIdLimit; ++dwordId)
								partialVertexData.push(inVertexData[dwordId]);
							
							// updates the id the temporary variable, to allow use filling partial index data later
							newVertexIds[localVertexId] = usedVerticesCount;
							
							// put its previous id in the dictionary
							usedVerticesDic[oldVertexIds[localVertexId]] = usedVerticesCount++;
						}
						
						partialIndexData.push(newVertexIds[localVertexId]);
					}
					
					// ... increment indices counter
					usedIndicesCount += 3;
					
					// some assertions for debug purposes..
					//	if (usedIndicesCount != partialIndexData.length)
					//		throw new Error('');
					//					
					//	if (usedVerticesCount != usedVertices.length)
					//		throw new Error('');
					//					
					//	if (usedVerticesCount != partialVertexData.length / dwordsPerVertex)
					//		throw new Error('');
				}
				
				outIndexDatas.push(partialIndexData);
				outVertexDatas.push(partialVertexData);
				
				inIndexData.splice(0, usedIndicesCount);
			}
		}
		
		/**
		 * Remove unused and duplicated entries in the vertexBuffer.
		 * 
		 * The index buffer must also be rewritten, because indexes get
		 * offseted on each removal.
		 * 
		 * @param dirtyIndexData
		 * @param dirtyVertexData
		 * @param indexData
		 * @param vertexData
		 * @param dwordsPerVertex
		 */
		public static function sanitizeBuffers(inVertexData		: Vector.<Number>,
											   inIndexData	 	: Vector.<uint>,
											   dwordsPerVertex	: uint = 3) : Vector.<Number>
		{
			var numDwords		: uint		= inVertexData.length;
			var bytesPerVertex	: uint		= dwordsPerVertex * 4;
			var numVertices		: uint		= numDwords / dwordsPerVertex;
			
			var vertexCopy		: ByteArray	= new ByteArray();
			for (var i : uint = 0; i < numDwords; ++i)
				vertexCopy.writeFloat(inVertexData[i]);
			
			vertexCopy.position = 0;
			var vertexHashes	: Vector.<uint> = new Vector.<uint>(numVertices, true);
			for (var vertexId : uint = 0; vertexId < numVertices; ++vertexId)
				vertexHashes[vertexId] = CRC32.computeForByteArrayChunk(vertexCopy, bytesPerVertex);
			
			var vertexIndex				: int;
			
			var tmpVertexComponentId	: int			= 0;
			
			var numIndices				: int			= inIndexData.length;
			var currentNumVertices		: int			= 0;
			
			var outVertexData			: Vector.<Number> = new Vector.<Number>(numDwords);
			var vertexPos				: int = 0;
			var verticesToIndex			: Array = new Array();
			
			for (var indexId : int = 0; indexId < numIndices; ++indexId)
			{
				var index		: int = inIndexData[indexId];
				var vertexHash	: int = vertexHashes[index]; 
				
				if (verticesToIndex[vertexHash] == undefined)
				{
					var positionIndex	: int = dwordsPerVertex * index;
					var positionLimit	: int = positionIndex + dwordsPerVertex;
					
					for (; positionIndex < positionLimit; ++positionIndex)
						outVertexData[int(vertexPos++)] = inVertexData[positionIndex];
					
					verticesToIndex[vertexHash] = vertexIndex = currentNumVertices++;
				}
				else
					vertexIndex = verticesToIndex[vertexHash];
				
				inIndexData[indexId] = vertexIndex;
			}
			
			outVertexData.length = vertexPos;
			return outVertexData;
		}
		
	}
}