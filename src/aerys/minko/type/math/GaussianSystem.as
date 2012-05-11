package aerys.minko.type.math
{
	public class GaussianSystem
	{
		private var _numVariables	: uint;
		private var _equations		: Vector.<Vector.<Number>>;
		
		public function GaussianSystem(numVariables : uint)
		{
			_numVariables	= numVariables;
			_equations		= new Vector.<Vector.<Number>>();
		}
		
		public function addEquation(v : Vector.<Number>) : void
		{
			if (v.length != _numVariables + 1)
				throw new ArgumentError('This is not a valid equation');
			
			_equations.push(v);
		}
		
		public function solve() : Vector.<Number>
		{
			sort();
			
			var numEquations	: uint = _equations.length;
			var equationId		: int;
			
			for (equationId = 0; equationId < numEquations; ++equationId)
				for (var eqId2 : int = equationId + 1; eqId2 < numEquations; ++eqId2)
					simplify(eqId2, equationId, equationId);
			
			for (equationId = numEquations - 1; equationId >= 0; --equationId)
				for (eqId2 = 0; eqId2 < equationId; ++eqId2)
					simplify(eqId2, equationId, equationId);
			
			var result : Vector.<Number> = new Vector.<Number>();
			for (equationId = 0; equationId < _numVariables; ++equationId)
				result.push(_equations[equationId][_numVariables] / _equations[equationId][equationId]);
			
			return result;
		}
		
		private function sort() : void
		{
			var numEquations : uint = _equations.length;
			
			for (var equationId1 : uint = 0; equationId1 < numEquations; ++equationId1)
				for (var equationId2 : uint = equationId1; equationId2 < numEquations; ++equationId2)
					if (_equations[equationId2][equationId1] != 0)
					{
						var tmp : Vector.<Number>	= _equations[equationId1];
						_equations[equationId1]		= _equations[equationId2];
						_equations[equationId2]		= tmp;
						break;
					}
		}
		
		private function simplify(targetLine		: uint,
								  sourceLine		: uint,
								  targetComponent	: uint) : void
		{
			var ratio : Number = 
				_equations[targetLine][targetComponent] / _equations[targetComponent][targetComponent];
			
			for (var variableId : uint = 0; variableId <= _numVariables; ++variableId)
				_equations[targetLine][variableId] -= _equations[sourceLine][variableId] * ratio;
		}
	}
}