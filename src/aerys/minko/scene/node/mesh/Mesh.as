package aerys.minko.scene.node.mesh
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.data.DataBinding;
	import aerys.minko.type.data.IBindable;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.utils.Dictionary;

	public class Mesh extends AbstractScene
	{
		use namespace minko_scene;
		use namespace minko_stream;
		
		private static const FORMAT_NORMALS		: VertexFormat	= new VertexFormat(
			VertexComponent.NORMAL
		);
		private static const FORMAT_TANGENTS	: VertexFormat	= new VertexFormat(
			VertexComponent.TANGENT
		);
		private static const FORMAT_TN			: VertexFormat	= new VertexFormat(
			VertexComponent.TANGENT,
			VertexComponent.NORMAL
		);
		
		minko_scene var _drawCalls	: Vector.<DrawCall>			= new <DrawCall>[];
		
		private var _bindings		: DataBinding				= new DataBinding(_drawCalls);
		private var _effect			: Effect					= null;
		private var _vertexStreams	: Vector.<IVertexStream>	= null;
		private var _indexStream	: IndexStream				= null;
		
		public function get bindings() : DataBinding
		{
			return _bindings;
		}
		
		public function get effect() : Effect
		{
			return _effect;
		}
		public function set effect(value : Effect) : void
		{
			if (_effect)
				_effect.changed.remove(effectChangedHandler);
			
			_effect = value;
			_effect.changed.add(effectChangedHandler);
			effectChangedHandler(_effect);
		}
		
		public function Mesh(effect			: Effect,
							 vertexStreams	: Vector.<IVertexStream>,
							 indexStream	: IndexStream	= null)
		{
			super();

			_vertexStreams = vertexStreams;
			_indexStream = indexStream;
			
			initialize();
			
			this.effect = effect;
		}
		
		private function initialize() : void
		{
			if (!_indexStream && _vertexStreams && _vertexStreams.length)
			{
				_indexStream = new IndexStream(
					StreamUsage.STATIC,
					null,
					_vertexStreams[0].length
				);
			}
		}
		
		override protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
			super.addedHandler(child, parent);
			
			_bindings.addParameter("local to world", parent.localToWorld);
		}
		
		override protected function removedHandler(child : ISceneNode, parent : Group) : void
		{
			super.removedHandler(child, parent);
			
			_bindings.removeParameter("local to world");
			_bindings.update();
		}
				
		private function effectChangedHandler(effect : Effect, property : String = null) : void
		{
			var passes		: Vector.<ActionScriptShader>	= _effect.passes;
			var numPasses 	: int 							= passes.length;
			
			_drawCalls.length = 0;
			for (var i : int = 0; i < numPasses; ++i)
			{
				var drawCall 	: DrawCall 					= passes[i].createDrawCall();
				var components 	: Vector.<VertexComponent> 	= drawCall.vertexComponents;
				
				if (components.indexOf(VertexComponent.TANGENT) >= 0
					&& _vertexStreams[0].getStreamByComponent(VertexComponent.TANGENT) == null)
				{
					computeTangentSpace(StreamUsage.STATIC);
				}
				else if (components.indexOf(VertexComponent.NORMAL) >= 0
						 && _vertexStreams[0].getStreamByComponent(VertexComponent.NORMAL) == null)
				{
					computeNormals(StreamUsage.STATIC);
				}
				
				drawCall.setStreams(_vertexStreams, _indexStream);
				_drawCalls[i] = drawCall;
			}
			
			_bindings.update();
		}
				
		public function clone() : Mesh
		{
			return new Mesh(_effect, _vertexStreams.concat(), _indexStream);
		}
		
		private function computeNormals(usage : uint) : void
		{
			var indices			: Vector.<uint>		= _indexStream._data;
			var numTriangles	: int				= _indexStream.length / 3;
			var numStreams		: int				= _vertexStreams.length;
			
			for (var streamId : int = 0; streamId < numStreams; ++streamId)
			{
				var stream			: IVertexStream		= _vertexStreams[streamId];
				var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
				var vertexOffset	: int 				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
				var vertexLength	: int 				= xyzStream.format.dwordsPerVertex;
				var vertices		: Vector.<Number>	= xyzStream._data;
				var numVertices		: int				= vertices.length / vertexLength;
				var normals			: Vector.<Number>	= new Vector.<Number>(3 * numVertices);
				var i				: int				= 0;
				
				for (i = 0; i < numTriangles; ++i)
				{
					var i0	: int 		= indices[int(3 * i)];
					var i1	: int 		= indices[int(3 * i + 1)];
					var i2	: int 		= indices[int(3 * i + 2)];
					
					var ii0	: int 		= vertexOffset + vertexLength * i0;
					var ii1	: int		= vertexOffset + vertexLength * i1;
					var ii2	: int 		= vertexOffset + vertexLength * i2;
					
					var x0	: Number 	= vertices[ii0];
					var y0	: Number 	= vertices[int(ii0 + 1)];
					var z0	: Number 	= vertices[int(ii0 + 2)];
					
					var x1	: Number 	= vertices[ii1];
					var y1	: Number 	= vertices[int(ii1 + 1)];
					var z1	: Number 	= vertices[int(ii1 + 2)];
					
					var x2	: Number 	= vertices[ii2];
					var y2	: Number 	= vertices[int(ii2 + 1)];
					var z2	: Number 	= vertices[int(ii2 + 2)];
					
					var nx	: Number 	= (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
					var ny	: Number 	= (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
					var nz	: Number 	= (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
					
					normals[int(i0 * 3)] += nx;
					normals[int(i0 * 3 + 1)] += ny;
					normals[int(i0 * 3 + 2)] += nz;
					
					normals[int(i1 * 3)] += nx;
					normals[int(i1 * 3 + 1)] += ny;
					normals[int(i1 * 3 + 2)] += nz;
					
					normals[int(i2 * 3)] += nx;
					normals[int(i2 * 3 + 1)] += ny;
					normals[int(i2 * 3 + 2)] += nz;
				}
				
				
				for (i = 0; i < numVertices; ++i)
				{
					var x	: Number = normals[int(i * 3)];
					var y	: Number = normals[int(i * 3 + 1)];
					var z	: Number = normals[int(i * 3 + 2)];
					
					var mag	: Number = Math.sqrt(x * x + y * y + z * z);
					
					if (mag != 0.)
					{
						normals[int(i * 3)] /= mag;
						normals[int(i * 3 + 1)] /= mag;
						normals[int(i * 3 + 2)] /= mag;
					}
				}
				
				pushVertexStream(
					streamId,
					new VertexStream(usage, FORMAT_NORMALS, normals)
				);
			}
		}
		
		private function computeTangentSpace(usage : uint) : void
		{
			var indices			: Vector.<uint>		= _indexStream._data;
			var numTriangles	: int				= _indexStream.length / 3;
			var numStreams		: int				= _vertexStreams.length;
			
			for (var streamId : int = 0; streamId < numStreams; ++streamId)
			{
				var stream			: IVertexStream		= _vertexStreams[streamId];
				
				var withNormals		: Boolean			= stream.getStreamByComponent(VertexComponent.NORMAL) == null;
				var f				: int				= withNormals ? 6 : 3;
				
				// (x, y, z) positions
				var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
				var xyzOffset		: int 				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
				var xyzSize			: int 				= xyzStream.format.dwordsPerVertex;
				var xyz				: Vector.<Number>	= xyzStream._data;
				
				// (u, v) texture coordinates
				var uvStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.UV);
				
				if (!uvStream)
					throw new Error("Missing vertex components u, v.");
				
				var uvOffset		: int 				= uvStream.format.getOffsetForComponent(VertexComponent.UV);
				var uvSize			: int 				= uvStream.format.dwordsPerVertex;
				var uv				: Vector.<Number>	= uvStream._data;
				
				var numVertices		: int				= xyz.length / xyzSize;
				
				var data			: Vector.<Number>	= new Vector.<Number>(f * numVertices);
				
				// normal
				var nx				: Number 			= 0.;
				var ny				: Number 			= 0.;
				var nz				: Number 			= 0.;
				
				// tangent
				var tx				: Number 			= 0.;
				var ty				: Number 			= 0.;
				var tz				: Number 			= 0.;
				
				var i				: int				= 0;
				var ii 				: int 				= 0;
				
				for (i = 0; i < numTriangles; ++i)
				{
					ii = i * 3;
					
					var i0		: int 		= indices[ii];
					var i1		: int 		= indices[int(ii + 1)];
					var i2		: int 		= indices[int(ii + 2)];
					
					var ii0		: int 		= xyzOffset + xyzSize * i0;
					var ii1		: int		= xyzOffset + xyzSize * i1;
					var ii2		: int 		= xyzOffset + xyzSize * i2;
					
					var x0		: Number 	= xyz[ii0];
					var y0		: Number 	= xyz[int(ii0 + 1)];
					var z0		: Number 	= xyz[int(ii0 + 2)];
					var u0		: Number	= uv[int(uvOffset + uvSize * i0)];
					var v0		: Number	= uv[int(uvOffset + uvSize * i0 + 1)];
					
					var x1		: Number 	= xyz[ii1];
					var y1		: Number 	= xyz[int(ii1 + 1)];
					var z1		: Number 	= xyz[int(ii1 + 2)];
					var u1		: Number	= uv[int(uvOffset + uvSize * i1)];
					var v1		: Number	= uv[int(uvOffset + uvSize * i1 + 1)];
					
					var x2		: Number 	= xyz[ii2];
					var y2		: Number 	= xyz[int(ii2 + 1)];
					var z2		: Number 	= xyz[int(ii2 + 2)];
					var u2		: Number	= uv[int(uvOffset + uvSize * i2)];
					var v2		: Number	= uv[int(uvOffset + uvSize * i2 + 1)];
					
					var v0v2	: Number 	= v0 - v2;
					var v1v2 	: Number 	= v1 - v2;
					var coef 	: Number 	= (u0 - u2) * v1v2 - (u1 - u2) * v0v2;
					
					if (coef == 0.)
						coef = 1.;
					else
						coef = 1. / coef;
					
					tx = coef * (v1v2 * (x0 - x2) - v0v2 * (x1 - x2));
					ty = coef * (v1v2 * (y0 - y2) - v0v2 * (y1 - y2));
					tz = coef * (v1v2 * (z0 - z2) - v0v2 * (z1 - z2));
					
					if (withNormals)
					{
						nx = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
						ny = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
						nz = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
					}
					
					ii = i0 * f;
					data[ii] += tx;
					data[int(ii + 1)] += ty;
					data[int(ii + 2)] += tz;
					if (withNormals)
					{
						data[int(ii + 3)] += nx;
						data[int(ii + 4)] += ny;
						data[int(ii + 5)] += nz;
					}
					
					ii = i1 * f;
					data[ii] += tx;
					data[int(ii + 1)] += ty;
					data[int(ii + 2)] += tz;
					if (withNormals)
					{
						data[int(ii + 3)] += nx;
						data[int(ii + 4)] += ny;
						data[int(ii + 5)] += nz;
					}
					
					ii = i2 * f;
					data[ii] += tx;
					data[int(ii + 1)] += ty;
					data[int(ii + 2)] += tz;
					if (withNormals)
					{
						data[int(ii + 3)] += nx;
						data[int(ii + 4)] += ny;
						data[int(ii + 5)] += nz;
					}
				}
				
				for (i = 0; i < numVertices; ++i)
				{
					ii = i * f;
					
					tx = data[ii];
					ty = data[int(ii + 1)];
					tz = data[int(ii + 2)];
					
					var mag	: Number = Math.sqrt(tx * tx + ty * ty + tz * tz);
					
					if (mag != 0.)
					{
						data[ii] /= mag;
						data[int(ii + 1)] /= mag;
						data[int(ii + 2)] /= mag;
					}
					
					if (withNormals)
					{
						nx = data[int(ii + 3)];
						ny = data[int(ii + 4)];
						nz = data[int(ii + 5)];
						
						mag = Math.sqrt(nx * nx + ny * ny + nz * nz);
						
						if (mag != 0.)
						{
							data[int(ii + 3)] /= mag;
							data[int(ii + 4)] /= mag;
							data[int(ii + 5)] /= mag;
						}
					}
				}
				
				pushVertexStream(
					streamId,
					new VertexStream(usage, withNormals ? FORMAT_TN : FORMAT_TANGENTS, data)
				);
			}
		}
		
		public function applyTransform(transform : Matrix4x4) : void
		{
			var numStreams	: int	= _vertexStreams.length;
			
			for (var vertexStreamId : int = 0; vertexStreamId < numStreams; ++vertexStreamId)
			{
				var stream		: IVertexStream		= _vertexStreams[vertexStreamId];
				var xyzStream	: VertexStream		= VertexStream.extractSubStream(
					stream,
					StreamUsage.STATIC,
					new VertexFormat(VertexComponent.XYZ)
				);
				
				xyzStream._data = transform.transformRawVectors(xyzStream._data);
				
				pushVertexStream(vertexStreamId, xyzStream, true);
			}
		}
		
		private function pushVertexStream(index 		: int,
										  vertexStream 	: VertexStream,
										  force 		: Boolean	= false) : void
		{
			var stream	: IVertexStream	= _vertexStreams[index];
			
			if (!(stream is VertexStreamList))
				_vertexStreams[index] = stream = new VertexStreamList(stream);
			
			(stream as VertexStreamList).pushVertexStream(vertexStream, force);
		}
	}
}