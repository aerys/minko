package aerys.minko.scene.controller.mesh.skinning
{
	import aerys.minko.ns.minko_stream;
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
	import flash.utils.Dictionary;
	
	internal final class SoftwareSkinningHelper extends AbstractSkinningHelper
	{
		private static const TMP_VECTOR : Vector.<uint> = new <uint>[];
		
		private var _meshToOriginalGeometry 	: Dictionary		= new Dictionary();
		private var _meshToFlattenedGeometry	: Dictionary		= new Dictionary();
		
		public function SoftwareSkinningHelper(method			: uint,
											   bindShape		: Matrix3D, 
											   invBindMatrices	: Vector.<Matrix3D>)
		{
			super(method, bindShape, invBindMatrices);
			
			if (method != SkinningMethod.SOFTWARE_MATRIX)
				throw new Error('Method not supported');
		}
		
		override public function addMesh(mesh : Mesh) : void
		{
			super.addMesh(mesh);
			
			_meshToOriginalGeometry[mesh]		= mesh.geometry;
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
		
		override public function update(skeletonRoot	: Group, 
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
			var dwordsPerVertex		: uint;
			var positionStride		: uint;
			var normalStride		: uint;
			var numInfluences		: uint;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
			{
				var realStream			: VertexStream		= VertexStream(realGeometry.getVertexStream(streamId));
				var fakeStream			: VertexStream		= VertexStream(fakeGeometry.getVertexStream(streamId));
				var realData			: Vector.<Number>	= realStream.minko_stream::_data
				var fakeData			: Vector.<Number>	= fakeStream.minko_stream::_data
				
				if (streamId == 0)
				{
					format				= realStream.format;
					dataLength			= realData.length;
					dwordsPerVertex		= format.size;
					positionStride		= format.getOffsetForComponent(VertexComponent.XYZ);
					normalStride		= format.getOffsetForComponent(VertexComponent.NORMAL);
					getInfluenceStrides(format, TMP_VECTOR);
					numInfluences		= influenceStrides.length;
				}
				
				for (var offset : uint = 0; offset < dataLength; offset += dwordsPerVertex)
				{
					var positionOffset	: uint		= offset + positionStride;
					var inX				: Number	= realData[positionOffset];
					var inY				: Number	= realData[positionOffset + 1];
					var inZ				: Number	= realData[positionOffset + 2];
					
					var normalOffset	: uint		= offset + normalStride;
					var inNx			: Number	= realData[normalOffset];
					var inNy			: Number	= realData[normalOffset + 1];
					var inNz			: Number	= realData[normalOffset + 2];
					
					var outX			: Number	= 0;
					var outY			: Number	= 0;
					var outZ			: Number	= 0;
					var outNx			: Number	= 0;
					var outNy			: Number	= 0;
					var outNz			: Number	= 0;
					
					for (var influenceId : uint = 0; influenceId < numInfluences; ++influenceId)
					{
						var influenceOffset : uint		= offset + influenceStrides[influenceId];
						var jointId			: uint		= realData[influenceOffset];
						var jointWeight		: Number	= realData[influenceOffset + 1];
						var matrixOffset	: uint		= 16 * jointId;
						
						if (jointWeight != 0)
						{
							outX += jointWeight * (inX * _matrices[matrixOffset + 0] + inY * _matrices[matrixOffset + 1] + inZ * _matrices[matrixOffset + 2] + _matrices[matrixOffset + 3]);
							outY += jointWeight * (inX * _matrices[matrixOffset + 4] + inY * _matrices[matrixOffset + 5] + inZ * _matrices[matrixOffset + 6] + _matrices[matrixOffset + 7]);
							outZ += jointWeight * (inX * _matrices[matrixOffset + 8] + inY * _matrices[matrixOffset + 9] + inZ * _matrices[matrixOffset + 10] + _matrices[matrixOffset + 11]);
							
							outNx += jointWeight * (inNx * _matrices[matrixOffset + 0] + inNy * _matrices[matrixOffset + 1] + inNz * _matrices[matrixOffset + 2]);
							outNy += jointWeight * (inNx * _matrices[matrixOffset + 4] + inNy * _matrices[matrixOffset + 5] + inNz * _matrices[matrixOffset + 6]);
							outNz += jointWeight * (inNx * _matrices[matrixOffset + 8] + inNy * _matrices[matrixOffset + 9] + inNz * _matrices[matrixOffset + 10]);
						}
					}
					
					var invNormalLength : Number = 1 / Math.sqrt(outNx * outNx + outNy * outNy + outNz * outNz);
					
					fakeData[positionOffset]		= outX;
					fakeData[positionOffset + 1]	= outY;
					fakeData[positionOffset + 2]	= outZ;
					fakeData[normalOffset]			= outNx * invNormalLength;
					fakeData[normalOffset + 1]		= outNy * invNormalLength;
					fakeData[normalOffset + 2]		= outNz * invNormalLength;
				}
				
				fakeStream.changed.execute(fakeStream);
			}
		}
	}
}
