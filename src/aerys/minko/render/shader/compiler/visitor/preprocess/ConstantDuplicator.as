package aerys.minko.render.shader.compiler.visitor.preprocess
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;

	public class ConstantDuplicator implements IShaderNodeVisitor
	{
		protected var _stack					: Vector.<INode>;

		protected var _isVertexShader	 		: Boolean;

		protected var _vertexConstants			: Vector.<AbstractConstant>;
		protected var _fragmentConstants		: Vector.<AbstractConstant>;

		protected var _fgConstantsToReplace		: Vector.<AbstractConstant>;
		protected var _fgReplacementConstants	: Vector.<AbstractConstant>;

		protected var _vtConstantsToReplace		: Vector.<AbstractConstant>;
		protected var _vtReplacementConstants	: Vector.<AbstractConstant>;

		public function ConstantDuplicator()
		{
		}

		public function processShader(clipspacePos	: INode,
									  color			: INode) : void
		{
			reset();

			_isVertexShader = true;
			visit(clipspacePos);

			_isVertexShader = false;
			visit(color);
		}

		protected function reset() : void
		{
			_stack		= new Vector.<INode>();

			_vertexConstants		= new Vector.<AbstractConstant>();
			_fragmentConstants		= new Vector.<AbstractConstant>();

			_fgConstantsToReplace	= new Vector.<AbstractConstant>();
			_fgReplacementConstants	= new Vector.<AbstractConstant>();

			_vtConstantsToReplace	= new Vector.<AbstractConstant>();
			_vtReplacementConstants	= new Vector.<AbstractConstant>();
		}

		public function visit(shaderNode:INode):void
		{
			_stack.push(shaderNode);

			if (shaderNode is AbstractConstant)
			{
				var constantNode:AbstractConstant = AbstractConstant(shaderNode);

				if (_isVertexShader)
					replace(constantNode, _vtConstantsToReplace,
						_vtReplacementConstants, _vertexConstants,
						_fragmentConstants);
				else
					replace(constantNode, _fgConstantsToReplace,
						_fgReplacementConstants,_fragmentConstants,
						_vertexConstants);
			}
			else if (shaderNode is Interpolate)
			{
				if (_isVertexShader)
					throw new Error('Found an interpolation node reachable '
									+ 'from the clipspace position node');

				_isVertexShader = true;
				shaderNode.accept(this);
				_isVertexShader = false;
			}
			else
			{
				shaderNode.accept(this);
			}

			_stack.pop();
		}

		protected function replace(shaderNode			: AbstractConstant,
								   constantsToReplace	: Vector.<AbstractConstant>,
								   replacementConstants	: Vector.<AbstractConstant>,
								   allowedConstants		: Vector.<AbstractConstant>,
								   forbiddenConstants	: Vector.<AbstractConstant>):void
		{
			var replaceId	: int		= constantsToReplace.indexOf(shaderNode);
			var clone		: AbstractConstant	= shaderNode;

			if (replaceId === -1 && forbiddenConstants.indexOf(shaderNode) !== -1)
			{
				constantsToReplace.push(shaderNode);
				clone = AbstractConstant(shaderNode).clone();
				replacementConstants.push(clone);
				replaceId = constantsToReplace.length - 1;
			}

			if (replaceId !== -1)
			{
				if (_stack.length < 2)
					throw new Error('Clipspace position and/or color nodes cannot be ' +
						'constant');

				var parent:AbstractOperation = _stack[_stack.length - 2] as AbstractOperation;

				if (!parent)
					throw new Error('Clipspace position node and/or color nodes must ' +
						'extend an AbstractOperation');

				parent.swapChildren(shaderNode, replacementConstants[replaceId]);
			}

			allowedConstants.push(clone);
		}
	}
}
