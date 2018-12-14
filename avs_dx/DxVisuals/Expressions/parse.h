#pragma once

namespace Expressions
{
	using Assignment = std::pair<CStringA, CStringA>;
	using Assignments = std::vector<Assignment>;

	HRESULT parseAssignments( CStringA code, Assignments& assignments );
}