#pragma once
#include "../SymbolTable/SymbolTable.h"

namespace Expressions
{
	// This class parses NSEEL into expression tree, and applies some transformations to the tree.
	// There's one more downstream compiler, HLSL, so we don't need to parse everything. This class only parses functions and variables, leaving everything else in raw stream of characters.
	// Reasons we need variables:
	// * To find their types: in NSEEL everything is double, HLSL is strongly-typed.
	// * To allocate right amount of space in the state buffer that's updated by the state CS and consumed by per-effects shaders.
	// Reasons we need functions:
	// * To transform if(), assign(), equals() functions into statements.
	// * To detect need for, and implement, RNG.
	// * To implement double-precision intrinsics: some presets use time to calculate sin/cos for periodic effects, floats don't have enough precision for that, just couple days of PC uptime and sin(time) will stop updating each frame: https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/
	// * Also, since we know which custom functions are used by each shader, we assemble global functions without #including too much stuff, this should improve HLSL compilation times.
	class Tree
	{
		enum struct eNode : uint8_t
		{
			// A sub-expression
			Expr,
			// Variable
			Var,
			// Function invocation
			Func,
			// Everything else, e.g. "(" or ")+14"
			Code,
		};

		struct Node
		{
			eNode node;
			eVarType vt = eVarType::unknown;

#ifdef DEBUG
			CStringA source;
#endif
			// For variables and functions, the ID from SymbolTable. For codes, offset in m_codez.
			int id = -1;
			// For codes, length of the data in m_codez. For expressions, children count. For functions, arguments count.
			int length = 0;

			// The parser emits nodes in depth-first order. This field holds index in m_nodes of the next sibling element, or -1 of this is the final node of the sub-tree.
			int nextSibling = -1;
		};

		SymbolTable& symbols;
		int m_lastStatement = -1;
		std::vector<char> m_codez;
		std::vector<Node> m_nodes;

		struct ExpressionContext
		{
			int prevSibling = -1;
			int children = 0;
		};

		void pushNode( ExpressionContext& ec, Node&& node );

		void pushCode( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushMacro( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushFloatLiteral( const CStringA& val, ExpressionContext& ec );
		void pushVar( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushFunc( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushExpression( const CStringA& expr, ExpressionContext& ec, int begin, int end );

		void parseExpression( const CStringA& expr, int begin, int end );

		eVarType nodeType( int indNode );
		eVarType expressionType( int iFirstChild );
		eVarType functionType( int iFunc );

		void dbgPrintList( int ind, int level ) const;
		void dbgPrintNode( int ind, int level ) const;

		struct EmitContext;
		void emitNode( EmitContext& ec, int ind ) const;
		void emitFunction( EmitContext& ec, int ind ) const;

		template<eInternalFunc id>
		void emitInternal( EmitContext& ec, const Node& node, int ind ) const;

		using pfnEmitNode = void ( Tree::* )( EmitContext& ec, const Node& node, int ind ) const;
		static const std::array<pfnEmitNode, eInternalFunc::valuesCount> s_emitInternal;

		bool nextSibling( int& ind ) const
		{
			ind = m_nodes[ ind ].nextSibling;
			return ind >= 0;
		}

	public:

		Tree( SymbolTable& symbolsTable );

		// Clear all nodes, however this will keep the symbols.
		void clear();

		HRESULT parse( const CStringA& expr );

		HRESULT appendAssignment( const CStringA& lhs, const CStringA& rhs );

		HRESULT deduceTypes();

		bool transformDoubleFuncs();

		void dbgPrint() const;

		HRESULT emitHlsl( CStringA& hlsl, bool& usesRng ) const;

		void getVariableUsage( std::vector<eVarAccess>& usage ) const;
	};
}