package aerys.minko.scene.controller.debug
{
	import aerys.minko.render.effect.basic.BasicProperties;
	import aerys.minko.scene.controller.ScriptController;
	import aerys.minko.scene.controller.camera.ArcBallController;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.debug.OrientationAxis;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.math.Plane;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;
	
	import flash.ui.MouseCursor;
	
	public final class OrientationAxisController extends ScriptController
	{
		private static const SELECTED_COLOR	: uint				= 0xffff00ee;
		private static const AXIS			: Vector.<Vector4>	= new <Vector4>[
			Vector4.X_AXIS,
			Vector4.Y_AXIS,
			Vector4.Z_AXIS
		];
		private static const PLANES			: Vector.<Plane>	= new <Plane>[
			new Plane(0, 1, 0, 0),
			new Plane(0, 0, 1, 0),
			new Plane(1, 0, 0, 0)
		];
		
		private var _lastHit			: Mesh		= null;
		private var _oldDiffuseColor	: uint		= 0;
		
		private var _dragOffset			: Vector4	= null;
		private var _dragPlane			: Plane		= null;
		private var _dragAxis			: Vector4	= null;
		
		public function OrientationAxisController()
		{
//			super(OrientationAxis);
		}
		
		override protected function update(target : ISceneNode) : void
		{
			var scene 	: Scene 	= target.root as Scene;
			var camera 	: Camera 	= scene.activeCamera;
			var ray 	: Ray 		= camera.unproject(viewport.mouseX, viewport.mouseY);
			var group 	: Group 	= target as Group;
			
			updateScale(target, camera);
			updateMouseOver(group, ray);
//			updateDragAndDrop(group, camera, ray);
		}
		
		private function updateScale(target : ISceneNode, camera : Camera) : void
		{
			if (_dragAxis != null)
				return ;
			
			var distance : Number = Vector4.distance(
				camera.localToWorld.getTranslation(),
				target.localToWorld.getTranslation()
			);
			
			distance *= .05;
			
			target.transform.setScale(distance, distance, distance);
		}
		
		private function updateMouseOver(target : Group, ray : Ray) : void
		{
			if (_dragAxis != null)
				return ;
			
			var hit : Mesh = (target as Group).cast(ray)[0];
			
			if (_lastHit != null && _lastHit != hit)
			{
				_lastHit.properties.setProperty(
					BasicProperties.DIFFUSE_COLOR,
					_oldDiffuseColor
				);
				mouse.popCursor();
			}
			
			if (hit)
			{
				if (hit != _lastHit)
				{
					_oldDiffuseColor = hit.properties.getProperty(BasicProperties.DIFFUSE_COLOR);
					mouse.pushCursor(MouseCursor.HAND);
					hit.properties.setProperty(
						BasicProperties.DIFFUSE_COLOR,
						SELECTED_COLOR
					);
				}
			}
			
			_lastHit = hit;
		}
		
		private function updateDragAndDrop(target : Group, camera : Camera, ray : Ray) : void
		{
			if (mouse.leftButtonDown)
			{
				if (!_dragAxis && _lastHit)
				{
					var index : uint = target.getChildIndex(_lastHit);
					
					_dragAxis = AXIS[index];
					_dragPlane = PLANES[index];
					_dragOffset = getDragPlaneIntersection(ray).subtract(target.transform.getTranslation());
				}
				
				if (_dragAxis != null)
				{
					var intersection 	: Vector4 	= getDragPlaneIntersection(ray);
					var offsetX			: Number	= (intersection.x - _dragOffset.x) * _dragAxis.x;
					var offsetY			: Number	= (intersection.y - _dragOffset.y) * _dragAxis.y;
					var offsetZ			: Number	= (intersection.z - _dragOffset.z) * _dragAxis.z;
					
					_dragOffset.x += offsetX;
					_dragOffset.y += offsetY
					_dragOffset.z += offsetZ;
					
					target.transform.appendTranslation(offsetX, offsetY, offsetZ);
				}
			}
			else
			{
				_dragAxis = null;
			}
			
			var ctrl : ArcBallController = camera.getControllersByType(ArcBallController)[0]
				as ArcBallController;
			
			if (ctrl)
				ctrl.enabled = _dragAxis == null;
		}
		
		private function getDragPlaneIntersection(ray : Ray) : Vector4
		{
			var t : Number = -(Vector4.dotProduct(ray.origin, _dragPlane.normal) + _dragPlane.d) /
				Vector4.dotProduct(ray.direction, _dragPlane.normal);
			
			return Vector4.add(
				ray.origin,
				new Vector4(t * ray.direction.x, t * ray.direction.y, t * ray.direction.z)
			);
		}
	}
}