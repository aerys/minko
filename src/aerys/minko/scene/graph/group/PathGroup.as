package aerys.minko.scene.graph.group
{
	import aerys.minko.scene.graph.Model;
	import aerys.minko.scene.graph.texture.ColorTexture;
	import aerys.minko.scene.graph.texture.ITexture;
	import aerys.minko.scene.graph.mesh.IMesh;
	import aerys.minko.scene.graph.mesh.primitive.SplineMesh;
	import aerys.minko.scene.graph.mesh.primitive.TriangleMesh;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.interpolation.AbstractSegment;
	import aerys.minko.type.interpolation.BezierCubicSegment;
	import aerys.minko.type.interpolation.BezierQuadSegment;
	import aerys.minko.type.interpolation.CatmullRomSegment;
	import aerys.minko.type.interpolation.CubicSegment;
	import aerys.minko.type.interpolation.HermiteSegment;
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.interpolation.LinearSegment;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * The PathGroup3D is an extension of TransformGroup thath permits
	 * easily tweening over a path.
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class PathGroup extends TransformGroup
	{
		protected var _path					: IInterpolation;
		protected var _lastRatio			: Number;
		
		/**
		 * current interpolation ratio in the path.
		 */
		public function get ratio() : Number
		{
			return _lastRatio;
		}
		
		public function set ratio(t : Number) : void
		{
			_lastRatio = t;
			_path.updateTransform(transform, t);
		}
		
		/**
		 * @param start First checkpoint of the path.
		 * @param at forward direction of the elements in this group. This vector will be kept parralel to the tangent of the path when updating the ratio value.
		 * @param up  up direction of the elements in this group. This vector will determine the rotation around the tangent of the path when updateing the ratio value.
		 * @param ...childen Children to be addChilded at the group creation
		 */
		public function PathGroup(path : IInterpolation, ...children)
		{
			super(children);
			
			_path		= path;
			
			ratio = 0;
		}
		
		public function addDebugMarkers(parent : IGroup) : void 
		{
			for each (var position : Vector4 in _path.checkpoints)
			{
				var markerMesh	: IMesh		= new TriangleMesh();
				var material	: ITexture	= ColorTexture.ORANGE;
				var marker		: Model		= new Model(markerMesh, material);
				
				marker.transform.position.set(position.x, position.y, position.z, 0);
				parent.addChild(marker);
			}
		}
		
		public function addDebugSpline(parent	: IGroup,
									   width	: Number	= 0.05,
									   chunks	: uint		= 80) : void
		{
			var mesh	: SplineMesh = new SplineMesh(_path, width, chunks);
			var mat		: ColorTexture = ColorTexture.GREEN;
			var spline	: Model = new Model(mesh, mat);
			
			parent.addChild(spline);
		}
		
	}
}
