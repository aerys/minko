package aerys.minko.render.shader.compiler.graph
{
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class ShaderGraph
	{
		private var _nodeToParents			: Dictionary;
		
		private var _position				: ANode;
		private var _positionComponents		: uint;
		private var _interpolates			: Vector.<ANode>;
		private var _color					: ANode;
		private var _colorComponents		: uint;
		private var _kills					: Vector.<ANode>;
		private var _killComponents			: Vector.<uint>;
		private var _computableConstants	: Object;
		
		public function get position()		: ANode				{ return _position;		}
		public function get interpolates()	: Vector.<ANode>	{ return _interpolates;	}
		public function get color()			: ANode				{ return _color;		}
		public function get kills()			: Vector.<ANode>	{ return _kills;		}
		
		public function get positionComponents()	: uint			{ return _positionComponents;	}
		public function get colorComponents()		: uint			{ return _colorComponents;		}
		public function get killComponents()		: Vector.<uint>	{ return _killComponents;		}
		
		public function get computableConstants()	: Object		{ return _computableConstants;	}
		
		public function set position			(v : ANode)	: void	{ _position = v;			}
		public function set color				(v : ANode)	: void	{ _color = v;				}
		public function set positionComponents	(v : uint)	: void	{ _positionComponents = v;	}
		public function set colorComponents		(v : uint)	: void	{ _colorComponents = v;		}
		
		public function ShaderGraph(position	: ANode,
									color		: ANode,
									kills		: Vector.<ANode>)
		{
			_nodeToParents			= new Dictionary();
			
			_position				= position;
			_positionComponents		= Components.createContinuous(0, 0, 4, position.size);
			_interpolates			= new Vector.<ANode>();
			_color					= color;
			_colorComponents		= Components.createContinuous(0, 0, 4, color.size);
			_kills					= kills;
			_killComponents			= new Vector.<uint>();
			_computableConstants	= new Object();
			
			var numKills : uint = kills.length;
			for (var killId : uint = 0; killId < numKills; ++killId)
				_killComponents[killId] = Components.createContinuous(0, 0, 1, 1);
		}
	}
}
