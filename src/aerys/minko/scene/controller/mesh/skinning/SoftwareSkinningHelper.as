package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.animation.SkinningMethod;
	
	import flash.geom.Matrix3D;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	internal final class SoftwareSkinningHelper extends AbstractSkinningHelper
	{
		private static const TMP_VECTOR : Vector.<uint> = new <uint>[];
		
		private var _meshToOriginalGeometry 	: Dictionary		= new Dictionary();
		private var _meshToFlattenedGeometry	: Dictionary		= new Dictionary();
		
		public function SoftwareSkinningHelper(method			: uint,
											   bindShape		: Matrix3D, 
											   invBindMatrices	: Vector.<Matrix3D>,
											   flattenSkinning	: Boolean = false,
											   numFps			: uint = 0,
											   skeletonRoot		: Group = null,
											   joints			: Vector.<Group> = null
		)
		{
			super(
				method, 
				bindShape,
				invBindMatrices,
				flattenSkinning,
				numFps,
				skeletonRoot,
				joints
			);
			
			if (method != SkinningMethod.SOFTWARE_MATRIX)
				throw new Error('Method not supported');
		}
		
		override public function addMesh(mesh : Mesh) : void
		{
			super.addMesh(mesh);
			
			_meshToOriginalGeometry[mesh]	= mesh.geometry;
			_meshToFlattenedGeometry[mesh]	= flattenGeometry(mesh.geometry);
			mesh.geometry = flattenGeometry(_meshToFlattenedGeometry[mesh]);
		}
		
		override public function removeMesh(mesh : Mesh) : void
		{
			super.removeMesh(mesh);
			
			mesh.geometry = _meshToOriginalGeometry[mesh];
			delete _meshToOriginalGeometry[mesh];
			delete _meshToFlattenedGeometry[mesh];
		}
		
		override protected function updateTargetSkinning(skeletonRoot	: Group, 
														 joints			: Vector.<Group>) : void
		{
			writeMatrices(skeletonRoot, joints);
			
			var numTargets : uint = _targets.length;
			for (var targetId : uint = 0; targetId < numTargets; ++targetId)
			{
				var target			: Mesh		= _targets[targetId];
				var fakeGeometry	: Geometry	= target.geometry;
				var realGeometry	: Geometry	= _meshToFlattenedGeometry[target];
				
				updateGeometry(fakeGeometry, realGeometry);
			}
		}
		
		private function flattenGeometry(realGeometry : Geometry) : Geometry
		{
			var numStreams		: uint						= realGeometry.numVertexStreams;
			var vertexStreams	: Vector.<IVertexStream>	= new <IVertexStream>[];
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				vertexStreams[streamId] = VertexStream.extractSubStream(realGeometry.getVertexStream(streamId), StreamUsage.DYNAMIC);
			
			return new Geometry(vertexStreams, realGeometry.indexStream);
		}
		
		private function updateGeometry(fakeGeometry : Geometry, 
										realGeometry : Geometry) : void
		{
			var numStreams			: uint			= realGeometry.numVertexStreams;
			var influenceStrides	: Vector.<uint>	= TMP_VECTOR;
			var format				: VertexFormat;
			var dataLength			: uint;
			var bytesPerVertex		: uint;
			var stride				: uint;
			var numInfluences		: uint;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
			{
				var realStreamXYZ		: VertexStream	= VertexStream(realGeometry.getVertexStream(streamId).getStreamByComponent(VertexComponent.XYZ));
				var fakeStreamXYZ		: VertexStream	= VertexStream(fakeGeometry.getVertexStream(streamId).getStreamByComponent(VertexComponent.XYZ));
				
				var realStreamNormal	: VertexStream	= VertexStream(realGeometry.getVertexStream(streamId).getStreamByComponent(VertexComponent.NORMAL));
				var fakeStreamNormal	: VertexStream	= VertexStream(fakeGeometry.getVertexStream(streamId).getStreamByComponent(VertexComponent.NORMAL));
				
				var realDataXYZ			: ByteArray		= realStreamXYZ.lock();
				var fakeDataXYZ			: ByteArray		= fakeStreamXYZ.lock();
				
				var realDataNormal		: ByteArray;
				var fakeDataNormal		: ByteArray;
				
				if (realStreamXYZ != realStreamNormal)
					realDataNormal = realStreamNormal.lock();
				else
					realDataNormal = realDataXYZ;
				
				if (fakeStreamXYZ != fakeStreamNormal)
					fakeDataNormal = fakeStreamNormal.lock();
				else
					fakeDataNormal = fakeDataXYZ;
				
				if (streamId == 0)
				{
					format				= realStreamXYZ.format;
					dataLength			= realDataXYZ.length;
					bytesPerVertex		= format.numBytesPerVertex;
					stride				= format.getBytesOffsetForComponent(VertexComponent.XYZ);
					getInfluenceStrides(format, TMP_VECTOR);
					numInfluences		= influenceStrides.length;
				}
				
				for (var offset : uint = 0; offset < dataLength; offset += bytesPerVertex)
				{
					var positionOffset	: uint		= offset + stride;
					realDataXYZ.position = positionOffset;
					
					var inX				: Number	= realDataXYZ.readFloat();
					var inY				: Number	= realDataXYZ.readFloat();
					var inZ				: Number	= realDataXYZ.readFloat();
					
					
					var outX			: Number	= 0;
					var outY			: Number	= 0;
					var outZ			: Number	= 0;
					
					for (var influenceId : uint = 0; influenceId < numInfluences; ++influenceId)
					{
						realDataXYZ.position = offset + influenceStrides[influenceId];
						
						var jointId			: Number	= realDataXYZ.readFloat();
						var jointWeight		: Number	= realDataXYZ.readFloat();
						var matrixOffset	: uint		= 16 * jointId;
						
						if (jointWeight != 0)
						{
							outX += jointWeight * (inX * _matrices[uint(matrixOffset + 0)]
								+ inY * _matrices[uint(matrixOffset + 1)]
								+ inZ * _matrices[uint(matrixOffset + 2)]
								+ _matrices[uint(matrixOffset + 3)]);
							
							outY += jointWeight * (inX * _matrices[uint(matrixOffset + 4)]
								+ inY * _matrices[uint(matrixOffset + 5)]
								+ inZ * _matrices[uint(matrixOffset + 6)]
								+ _matrices[uint(matrixOffset + 7)]);
							
							outZ += jointWeight * (inX * _matrices[uint(matrixOffset + 8)]
								+ inY * _matrices[uint(matrixOffset + 9)]
								+ inZ * _matrices[uint(matrixOffset + 10)]
								+ _matrices[uint(matrixOffset + 11)]);
						}
					}
					
					fakeDataXYZ.position = positionOffset;
					fakeDataXYZ.writeFloat(outX);
					fakeDataXYZ.writeFloat(outY);
					fakeDataXYZ.writeFloat(outZ);
				}
				
				
				if (streamId == 0)
				{
					format				= realStreamNormal.format;
					dataLength			= realDataNormal.length;
					bytesPerVertex		= format.numBytesPerVertex;
					stride				= format.getBytesOffsetForComponent(VertexComponent.NORMAL);
					getInfluenceStrides(format, TMP_VECTOR);
					numInfluences		= influenceStrides.length;
				}
				
				for (offset = 0; offset < dataLength; offset += bytesPerVertex)
				{
					var normalOffset	: uint		= offset + stride;
					realDataNormal.position = normalOffset;
					var inNx			: Number	= realDataNormal.readFloat();
					var inNy			: Number	= realDataNormal.readFloat();
					var inNz			: Number	= realDataNormal.readFloat();
					
					var outNx			: Number	= 0;
					var outNy			: Number	= 0;
					var outNz			: Number	= 0;
					
					for (influenceId = 0; influenceId < numInfluences; ++influenceId)
					{
						realDataNormal.position = offset + influenceStrides[influenceId];
						
						jointId			= realDataNormal.readFloat();
						jointWeight		= realDataNormal.readFloat();
						matrixOffset	= 16 * jointId;
						
						if (jointWeight != 0)
						{							
							outNx += jointWeight * (inNx * _matrices[uint(matrixOffset + 0)]
								+ inNy * _matrices[uint(matrixOffset + 1)]
								+ inNz * _matrices[uint(matrixOffset + 2)]);
							
							outNy += jointWeight * (inNx * _matrices[uint(matrixOffset + 4)]
								+ inNy * _matrices[uint(matrixOffset + 5)]
								+ inNz * _matrices[uint(matrixOffset + 6)]);
							
							outNz += jointWeight * (inNx * _matrices[uint(matrixOffset + 8)]
								+ inNy * _matrices[uint(matrixOffset + 9)]
								+ inNz * _matrices[uint(matrixOffset + 10)]);
						}
					}
					
					var invNormalLength : Number = 1 / Math.sqrt(outNx * outNx + outNy * outNy + outNz * outNz);
					
					fakeDataNormal.position = normalOffset;
					fakeDataNormal.writeFloat(outNx * invNormalLength);
					fakeDataNormal.writeFloat(outNy * invNormalLength);
					fakeDataNormal.writeFloat(outNz * invNormalLength);
				}
				
				realStreamXYZ.unlock(false);
				if (realStreamNormal != realStreamXYZ)
					realStreamNormal.unlock(false);
				
				fakeStreamXYZ.unlock();
				if (fakeStreamNormal != fakeStreamXYZ)
					fakeStreamNormal.unlock(false);
			}
		}
	}
}
