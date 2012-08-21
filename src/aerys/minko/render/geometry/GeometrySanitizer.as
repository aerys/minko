package aerys.minko.render.geometry
{
	import aerys.minko.render.shader.compiler.CRC32;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.Endian;

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
		public static const INDEX_LIMIT		: uint = 524270;
		public static const VERTEX_LIMIT	: uint = 65535;
		
		public static function isValid(indexData		: ByteArray,
									   vertexData		: ByteArray,
									   bytesPerVertex	: uint = 12) : Boolean
		{
			var startPosition	: uint	= indexData.position;
			var numIndices		: uint 	= indexData.bytesAvailable >> 2;
			var numVertices		: uint 	= vertexData.bytesAvailable / bytesPerVertex;
			
			for (var indexId : uint = 0; indexId < numIndices; ++indexId)
				if (indexData.readShort() >= numVertices)
					break;
			
			indexData.position = startPosition;
			
			return indexId >= numIndices;
		}
		
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
		public static function splitBuffers(inVertices		: ByteArray,
											inIndices		: ByteArray,
											outVertices		: Vector.<ByteArray>,
											outIndices		: Vector.<ByteArray>,
											bytesPerVertex	: uint = 12) : void
		{
			var inVerticesStartPosition	: uint	= inVertices.position;
			var numVertices				: uint	= inVertices.bytesAvailable / bytesPerVertex;
			var inIndicesStartPosition	: uint	= inIndices.position;
			var numIndices				: uint	= inIndices.bytesAvailable >>> 1;
			
			if (numIndices < INDEX_LIMIT && numVertices < VERTEX_LIMIT)
			{
				outVertices.push(inVertices);
				outIndices.push(inIndices);
				
				return;
			}
			
			while (inIndices.bytesAvailable)
			{
				var indexDataLength		: uint				= inIndices.bytesAvailable >> 1;
				
				// new buffers
				var partialVertexData	: ByteArray			= new ByteArray();
				var partialIndexData	: ByteArray			= new ByteArray();
				
				// local variables
				var oldVertexIds		: Vector.<int>		= new Vector.<uint>(3, true);
				var newVertexIds		: Vector.<int>		= new Vector.<uint>(3, true);
				var newVertexNeeded		: Vector.<Boolean>	= new Vector.<Boolean>(3, true);
				
				var usedVerticesDic		: Dictionary		= new Dictionary();	// this dictionary maps old and new indices
				var usedVerticesCount	: uint				= 0;				// this counts elements in the dictionary
				var usedIndicesCount	: uint				= 0;				// how many indices have we used?
				var neededVerticesCount	: uint;
				
				// iterators & limits
				var localVertexId		: uint;
				
				partialVertexData.endian = Endian.LITTLE_ENDIAN;
				partialIndexData.endian = Endian.LITTLE_ENDIAN;
				
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
						inIndices.position = (usedIndicesCount + localVertexId) << 2;
						oldVertexIds[localVertexId]	= inIndices.readShort();
						
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
							partialVertexData.writeBytes(
								inVertices,
								oldVertexIds[localVertexId] * bytesPerVertex,
								bytesPerVertex
							);
							
							// updates the id the temporary variable, to allow use filling partial index data later
							newVertexIds[localVertexId] = usedVerticesCount;
							
							// put its previous id in the dictionary
							usedVerticesDic[oldVertexIds[localVertexId]] = usedVerticesCount++;
						}
						
						partialIndexData.writeShort(newVertexIds[localVertexId]);
					}
					
					// ... increment indices counter
					usedIndicesCount += 3;
				}
				
				partialVertexData.position = 0;
				outVertices.push(partialVertexData);

				partialIndexData.position = 0;
				outIndices.push(partialIndexData);
				
				inVertices.position = inVerticesStartPosition;
				inIndices.position = inIndicesStartPosition;
			}
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
		public static function removeDuplicatedVertices(vertexData		: ByteArray,
														indexData		: ByteArray,
														bytesPerVertex	: uint = 12) : void
		{
			var vertexDataStartPosition		: uint			= vertexData.position;
			var numVertices					: uint			= vertexData.bytesAvailable / bytesPerVertex;
			var indexDataStartPosition		: uint			= indexData.position;
			var numIndices					: uint			= indexData.bytesAvailable >>> 1;
			var hashToNewVertexId			: Object		= {};
			var oldVertexIdToNewVertexId	: Vector.<uint>	= new Vector.<uint>(numVertices, true);
			var newVertexCount				: uint			= 0;
			var newLimit					: uint			= 0;
			
			for (var oldVertexId : uint = 0; oldVertexId < numVertices; ++oldVertexId)
			{
				vertexData.position = oldVertexId * bytesPerVertex;
				
				var numFloats	: uint		= bytesPerVertex >>> 2;
				var vertexHash	: String	= '';
				
				while (numFloats)
				{
					vertexHash += vertexData.readFloat() + '|';
					--numFloats;
				}
				
				var index		: Object	= hashToNewVertexId[vertexHash];
				var newVertexId	: uint		= 0;
				
				if (index === null)
				{
					newVertexId = newVertexCount++;
					hashToNewVertexId[vertexHash] = newVertexId;
					
					newLimit = (1 + newVertexId) * bytesPerVertex;
					
					if (newVertexId != oldVertexId)
					{
						vertexData.position = newVertexId * bytesPerVertex;
						vertexData.writeBytes(vertexData, oldVertexId * bytesPerVertex, bytesPerVertex);
					}
				}
				else
					newVertexId = uint(index);
				
				oldVertexIdToNewVertexId[oldVertexId] = newVertexId;
			}
			
			vertexData.position = vertexDataStartPosition;
			vertexData.length = newLimit;
			
			for (var indexId : int = 0; indexId < numIndices; ++indexId)
			{
				index = indexData.readShort();
				indexData.position -= 2;
				indexData.writeShort(oldVertexIdToNewVertexId[index]);
			}
			indexData.position = indexDataStartPosition;
		}
		
		/**
		 * Removes unused entries in a vertex buffer, and rewrite the index buffer according to it.
		 * The buffers are modified in place.
		 * 
		 * @param vertexData 
		 * @param indexData 
		 * @param dwordsPerVertex 
		 */		
		public static function removeUnusedVertices(vertexData		: ByteArray,
													indexData		: ByteArray,
													bytesPerVertex	: uint = 12) : void
		{
			var vertexDataStartPosition	: uint	= vertexData.position;
			var oldNumVertices			: uint	= vertexData.bytesAvailable / bytesPerVertex;
			var newNumVertices			: uint	= 0;
			var indexDataStartPosition	: uint	= indexData.position;
			var numIndices				: uint	= indexData.bytesAvailable >>> 1;
			
			// iterators
			var oldVertexId		: uint;
			var indexId			: uint;
			
			// flag unused vertices by scanning the index buffer
			var oldVertexIdToUsage : Vector.<Boolean> = new Vector.<Boolean>(oldNumVertices, true);
			for (indexId = 0; indexId < numIndices; ++indexId)
				oldVertexIdToUsage[indexData.readShort()] = true;
			
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
						vertexData.position = newVertexId * bytesPerVertex;
						vertexData.writeBytes(vertexData, oldVertexId * bytesPerVertex, bytesPerVertex);
					}
				}
			
			vertexData.position = vertexDataStartPosition;
			vertexData.length = newNumVertices * bytesPerVertex;
			
			// rewrite index buffer in place
			indexData.position = indexDataStartPosition;
			for (indexId = 0; indexId < numIndices; ++indexId)
			{
				var index : uint = indexData.readShort();
				
				indexData.position -= 4;
				indexData.writeShort(oldVertexIdToNewVertexId[index]);
			}
			indexData.position = indexDataStartPosition;
		}
	}
}