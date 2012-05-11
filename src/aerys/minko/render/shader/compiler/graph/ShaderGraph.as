package aerys.minko.render.shader.compiler.graph
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
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
		
		private var _position				: AbstractNode;
		private var _positionComponents		: uint;
		private var _interpolates			: Vector.<AbstractNode>;
		private var _color					: AbstractNode;
		private var _colorComponents		: uint;
		private var _kills					: Vector.<AbstractNode>;
		private var _killComponents			: Vector.<uint>;
		private var _computableConstants	: Object;
		
		public function get position()		: AbstractNode			{ return _position;		}
		public function get interpolates()	: Vector.<AbstractNode>	{ return _interpolates;	}
		public function get color()			: AbstractNode			{ return _color;		}
		public function get kills()			: Vector.<AbstractNode>	{ return _kills;		}
		
		public function get positionComponents()	: uint			{ return _positionComponents;	}
		public function get colorComponents()		: uint			{ return _colorComponents;		}
		public function get killComponents()		: Vector.<uint>	{ return _killComponents;		}
		
		public function get computableConstants()	: Object		{ return _computableConstants;	}
		
		public function set position			(v : AbstractNode)	: void	{ _position = v;			}
		public function set color				(v : AbstractNode)	: void	{ _color = v;				}
		public function set positionComponents	(v : uint)			: void	{ _positionComponents = v;	}
		public function set colorComponents		(v : uint)			: void	{ _colorComponents = v;		}
		
		public function ShaderGraph(position	: AbstractNode,
									color		: AbstractNode,
									kills		: Vector.<AbstractNode>)
		{
			_nodeToParents			= new Dictionary();
			
			_position				= position;
			_positionComponents		= Components.createContinuous(0, 0, 4, position.size);
			_interpolates			= new Vector.<AbstractNode>();
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
