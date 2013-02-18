package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.geometry.primitive.LineGeometry;
	import aerys.minko.render.geometry.stream.iterator.TriangleIterator;
	import aerys.minko.render.geometry.stream.iterator.TriangleReference;
	import aerys.minko.render.material.line.LineMaterial;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.Mesh;
	
	import flash.utils.Dictionary;
	
	public final class WireframeDebugController extends AbstractController
	{
		private var _wireframeMaterial	: LineMaterial;
		private var _targetToWireframe	: Dictionary	= new Dictionary();
		
		public function get material() : LineMaterial
		{
			return _wireframeMaterial
				|| (_wireframeMaterial = new LineMaterial({diffuseColor : 0xffffffff, lineThickness	: 1.}));
		}
		
		public function WireframeDebugController()
		{
			super(Mesh);
			
			initialize();
		}

		private function initialize() : void
		{
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(ctrl	: WireframeDebugController,
											target	: Mesh) : void
		{
			target.added.add(addedHandler);
			target.removed.add(removedHandler);
		}
		
		private function addedHandler(target 	: Mesh,
									  ancestor	: Group) : void
		{
			if (!target.scene)
				return ;
			
			var triangles	: TriangleIterator = new TriangleIterator(
				target.geometry.getVertexStream(),
				target.geometry.indexStream
			);
			var linesGeom	: LineGeometry		= new LineGeometry();
			for each (var triangle : TriangleReference in triangles)
			{
				linesGeom.moveTo(triangle.v0.x, triangle.v0.y, triangle.v0.z)
					.lineTo(triangle.v1.x, triangle.v1.y, triangle.v1.z)
					.lineTo(triangle.v2.x, triangle.v2.y, triangle.v2.z)
					.lineTo(triangle.v0.x, triangle.v0.y, triangle.v0.z);
			}
			
			var lines		: Mesh				= new Mesh(linesGeom, this.material);
			_targetToWireframe[target] = lines;
			 
			target.parent.addChild(lines);
		}
		
		private function removeWireframes(target : Mesh) : void
		{
			if (_targetToWireframe[target])
				_targetToWireframe[target].parent = null;
			_targetToWireframe[target] = null;
		}
		
		private function targetRemovedHandler(ctrl		: WireframeDebugController,
											  target	: Mesh) : void
		{
			target.added.remove(addedHandler);
			target.removed.remove(removedHandler);
			removeWireframes(target);
		}
		
		private function removedHandler(target : Mesh, ancestor : Group) : void
		{
			removeWireframes(target);
		}
	}
}