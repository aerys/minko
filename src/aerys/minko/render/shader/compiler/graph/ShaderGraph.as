package aerys.minko.render.shader.compiler.graph
{
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.register.Components;

	public class ShaderGraph
	{
		private var _position				: INode;
		private var _positionComponents		: uint;
		
		private var _interpolates			: Vector.<INode>;
		
		private var _color					: INode;
		private var _colorComponents		: uint;
		
		private var _kills					: Vector.<INode>;
		private var _killComponents			: Vector.<uint>;
		
		private var _computableConstants	: Object;
		
		public function get position()		: INode				{ return _position;		}
		public function get interpolates()	: Vector.<INode>	{ return _interpolates;	}
		public function get color()			: INode				{ return _color;		}
		public function get kills()			: Vector.<INode>	{ return _kills;		}
		
		public function get positionComponents()	: uint			{ return _positionComponents;	}
		public function get colorComponents()		: uint			{ return _colorComponents;		}
		public function get killComponents()		: Vector.<uint>	{ return _killComponents;		}
		
		public function get computableConstants()	: Object		{ return _computableConstants;	}
		
		public function set position			(v : INode)	: void	{ _position = v;			}
		public function set color				(v : INode)	: void	{ _color = v;				}
		public function set positionComponents	(v : uint)	: void	{ _positionComponents = v;	}
		public function set colorComponents		(v : uint)	: void	{ _colorComponents = v;		}
		
		public function ShaderGraph(position	: INode,
									color		: INode,
									kills		: Vector.<INode>)
		{
			_position				= position;
			_positionComponents		= Components.createContinuous(0, 0, 4, position.size);
			_interpolates			= new Vector.<INode>();
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
