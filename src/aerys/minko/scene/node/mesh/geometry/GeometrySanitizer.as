package aerys.minko.scene.node.mesh.geometry
{
	import aerys.minko.render.shader.compiler.CRC32;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	/**
	 * The GeometrySanitizer static class provides methods to clean up
	 * 3D geometry loaded from 3D asset files and make sure it is compliant
	 * with the limitations of the Stage3D API.
	 * 
	 * @author Romain Gilliotte
	 * 
	 */
	public final class GeometrySanitizer
	{
		private static const INDEX_LIMIT	: uint = 524270;
		private static const VERTEX_LIMIT	: uint = 65535;
		
		public static function isValid(indexData		: Vector.<uint>,
									   vertexData		: Vector.<Number>,
									   dwordsPerVertex	: uint = 3) : Boolean
		{
			var numIndices	: uint = indexData.length;
			var numVertices	: uint = vertexData.length;
			
			for (var indexId : uint = 0; indexId < numIndices; ++indexId)
				if (indexData[indexId] >= numVertices)
					return false;
			
			return true;
		}
		
		/*public static function splitBuffers(inVertexData	: Vector.<Number>,
											inIndexData		: Vector.<uint>,
											outVertexData	: Vector.<Vector.<Number>>,
											outIndexData	: Vector.<Vector.<uint>>,
											dwordsPerVertex	: uint = 3) : void
		{
			if (inIndexData.length < INDEX_LIMIT && inVertexData.length / dwordsPerVertex < VERTEX_LIMIT)
			{
				outVertexData.push(inVertexData);
				outIndexData.push(inIndexData);
				
				return ;
			}
			
			var numIndices 	: uint 	= inIndexData.length;
			var indexId		: uint	= 0;
			
			while (indexId < numIndices)
			{
				
			}
		}*/
		
		/**
		 * Split vertex and index buffers too big the be rendered.
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
				var partialVertexData	: Vector.<Number>	= new <Number>[];
				var partialIndexData	: Vector.<uint>		= new <uint>[];
				
				// local variables
				var oldVertexIds		: Vector.<int>		= new Vector.<int>(3, true);
				var newVertexIds		: Vector.<int>		= new Vector.<int>(3, true);
				var newVertexNeeded		: Vector.<Boolean>	= new Vector.<Boolean>(3, true);
				
				var usedVerticesDic		: Dictionary		= new Dictionary();	// this dictionary maps old and new indices
				var usedVerticesCount	: uint				= 0;				// this counts elements in the dictionary
				var usedIndicesCount	: uint				= 0;				// how many indices have we used?
				var neededVerticesCount	: uint;
				
				// iterators & limits
				var localVertexId		: uint;
				var dwordId				: uint;
				var dwordIdLimit		: uint;
				
				while (usedIndicesCount < indexDataLength)
				{
					// check if next triangle fits into the index buffer
					var remainingIndices	: uint		= INDEX_LIMIT - usedIndicesCount;
					
					if (remainingIndices < 3)
						break ;
					
					// check if next triangle fits into the vertex buffer
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
						break ;
					
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
					
//					// some assertions for debug purposes..
//					if (usedIndicesCount != partialIndexData.length)
//						throw new Error('Assert failed');
//					
//					
//					if (usedVerticesCount != partialVertexData.length / dwordsPerVertex)
//						throw new Error('Assert failed');
				}
				
				outIndexDatas.push(partialIndexData);
				outVertexDatas.push(partialVertexData);
				
				inIndexData.splice(0, usedIndicesCount);
			}
			
/*			outIndexDatas.pop();
			outVertexDatas.pop();*/
		}
		
		/**
		 * Removes duplicated entries in a vertex buffer, and rewrite the index buffer according to it.
		 * The buffers are modified in place.
		 * 
		 * This method simplifies implementing parsers for file-formats that store per-vertex 
		 * and per-triangle information in different buffers, such as collada or OBJ:
		 * merging them in a naive way and calling this method will do the job.
		 * 
		 * @param vertexData 
		 * @param indexData 
		 * @param dwordsPerVertex 
		 */
		public static function removeDuplicatedVertices(vertexData		: Vector.<Number>,
														indexData	 	: Vector.<uint>,
														dwordsPerVertex	: uint = 3) : void
		{
			var numDwords					: uint			= vertexData.length;
			var numVertices					: uint			= numDwords / dwordsPerVertex;
			var numIndices					: int			= indexData.length;
			var bytesPerVertex				: uint			= dwordsPerVertex * 4;
			var vertexCopy					: ByteArray		= new ByteArray();
			var hashToNewVertexId			: Array			= [];
			var oldVertexIdToNewVertexId	: Vector.<uint>	= new Vector.<uint>(numVertices, true);
			var newVertexCount				: uint			= 0;
			
			for (var i : uint = 0; i < numDwords; ++i)
				vertexCopy.writeFloat(vertexData[i]);
			
			vertexCopy.position = 0;
			
			
			var newLimit : uint = 0;
			
			for (var oldVertexId : uint = 0; oldVertexId < numVertices; ++oldVertexId)
			{
				var vertexHash	: uint		= CRC32.computeForByteArrayChunk(vertexCopy, bytesPerVertex);
				var index		: Object	= hashToNewVertexId[vertexHash];
				var newVertexId	: uint		= 0;
				
				if (index === null)
				{
					newVertexId = newVertexCount++;
					hashToNewVertexId[vertexHash] = newVertexId;
					
					newLimit = (1 + newVertexId) * dwordsPerVertex;
					
					if (newVertexId != oldVertexId)
					{
						var oldOffset	: uint = oldVertexId * dwordsPerVertex;
						var newOffset	: uint = newVertexId * dwordsPerVertex;
						
						for (; newOffset < newLimit; ++newOffset, ++oldOffset)
							vertexData[newOffset] = vertexData[oldOffset];
					}
				}
				else
					newVertexId = uint(index);
				
				oldVertexIdToNewVertexId[oldVertexId] = newVertexId;
			}
			
			vertexData.length = newLimit;
			
			for (var indexId : int = 0; indexId < numIndices; ++indexId)
				indexData[indexId] = oldVertexIdToNewVertexId[indexData[indexId]];
		}
		
		/**
		 * Removes unused entries in a vertex buffer, and rewrite the index buffer according to it.
		 * The buffers are modified in place.
		 * 
		 * @param vertexData 
		 * @param indexData 
		 * @param dwordsPerVertex 
		 */		
		public static function removeUnusedVertices(vertexData		: Vector.<Number>,
													indexData		: Vector.<uint>,
													dwordsPerVertex	: uint = 3) : void
		{
			var numIndices		: int	= indexData.length;
			var oldNumVertices	: uint	= vertexData.length / dwordsPerVertex;
			var newNumVertices	: uint	= 0;
			
			// iterators
			var oldVertexId		: uint;
			var indexId			: uint;
			
			// flag unused vertices by scanning the index buffer
			var oldVertexIdToUsage : Vector.<Boolean> = new Vector.<Boolean>(oldNumVertices, true);
			for (indexId = 0; indexId < numIndices; ++indexId)
				oldVertexIdToUsage[indexData[indexId]] = true;
			
			// scan the flags, fix vertex buffer, and store old to new vertex id mapping
			var oldVertexIdToNewVertexId : Vector.<uint> = new Vector.<uint>(oldNumVertices, true);
			for (oldVertexId = 0; oldVertexId < oldNumVertices; ++oldVertexId)
				if (oldVertexIdToUsage[oldVertexId])
				{
					var newVertexId	: uint = newNumVertices++;
					
					// store new index
					oldVertexIdToNewVertexId[oldVertexId] = newVertexId;
					
					// rewrite vertexbuffer in place
					if (newVertexId != oldVertexId)
					{
						var newOffset	: uint = newVertexId * dwordsPerVertex;
						var newLimit	: uint = newOffset + dwordsPerVertex;
						var oldOffset	: uint = oldVertexId * dwordsPerVertex;
						
						for (; newOffset < newLimit; ++newOffset, ++oldOffset)
							vertexData[newOffset] = vertexData[oldOffset];
					}
				}
			
			vertexData.length = newNumVertices * dwordsPerVertex;
			
			// rewrite index buffer in place
			for (indexId = 0; indexId < numIndices; ++indexId)
				indexData[indexId] = oldVertexIdToNewVertexId[indexData[indexId]];
		}
	}
}