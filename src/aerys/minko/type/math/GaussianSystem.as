package aerys.minko.type.math
{
	public class GaussianSystem
	{
		private var _numVariables	: uint;
		private var _equations		: Vector.<Vector.<Number>>;
		
		public function GaussianSystem(numVariables : uint)
		{
			_numVariables = numVariables;
			_equations = new Vector.<Vector.<Number>>();
		}
		
		public function addEquation(v : Vector.<Number>) : void
		{
			_equations.push(v);
		}
		
		public function solve() : Vector.<Number>
		{
			sort();
			
			var numEq : uint = _equations.length;
			for (var eqId : int = 0; eqId < numEq; ++eqId)
				for (var eqId2 : int = eqId + 1; eqId2 < numEq; ++eqId2)
					simplify(eqId2, eqId, eqId);
			
			for (eqId = numEq - 1; eqId >= 0; --eqId)
				for (eqId2 = 0; eqId2 < eqId; ++eqId2)
					simplify(eqId2, eqId, eqId);
			
			var result : Vector.<Number> = new Vector.<Number>();
			for (eqId = 0; eqId < _numVariables; ++eqId)
				result.push(_equations[eqId][_numVariables] / _equations[eqId][eqId]);
			
			return result;
		}
		
		private function sort() : void
		{
			var numEq : uint = _equations.length;
			
			for (var eqId : uint = 0; eqId < numEq; ++eqId)
				for (var eqId2 : uint = eqId; eqId2 < numEq; ++eqId2)
					if (_equations[eqId2][eqId] != 0)
					{
						var tmp : Vector.<Number>	= _equations[eqId];
						_equations[eqId]			= _equations[eqId2];
						_equations[eqId2]			= tmp;
						break;
					}
		}
		
		private function simplify(targetLine		: uint,
								  sourceLine		: uint,
								  targetComponent	: uint) : void
		{
			var ratio : Number = 
				_equations[targetLine][targetComponent] / _equations[targetComponent][targetComponent];
			
			for (var i : uint = 0; i < _numVariables + 1; ++i)
				_equations[targetLine][i] -= _equations[sourceLine][i] * ratio;
		}
	}
}