#pragma once
#include "../SymbolTable/SymbolTable.h"
#include "VarUseFlags.h"

namespace Expressions
{
	// This class parses NSEEL into expression tree, and applies some transformations to the tree.
	// There's one more downstream compiler, HLSL, so we don't need to parse everything. This class only parses functions and variables, leaving everything else in raw stream of characters.
	// Reasons we need variables:
	// * To find their types: in NSEEL everything is double, the JIT compiler emitted x87 code. HLSL is strongly-typed, and doubles are several times slower than floats or uints.
	// * To allocate space in the state buffer that's updated by the state CS and consumed by per-effects shaders.
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
			// For codes, count of characters in m_codez. For expressions, children count. For functions, arguments count.
			int length = 0;

			// The parser emits nodes in depth-first order. This field holds index in m_nodes of the next sibling element, or -1 if this is the last node of the sub-tree.
			int nextSibling = -1;
		};

		SymbolTable& symbols;
		// Index of the last top-level node, or -1 for an empty tree.
		int m_lastStatement = -1;
		vector<char> m_codez;
		vector<Node> m_nodes;

		bool nextSibling( int& ind ) const
		{
			ind = m_nodes[ ind ].nextSibling;
			return ind > 0;
		}

		// ==== Parsing ====

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

		// ==== Type deduction ====

		using pfnFunctionType = eVarType( Tree::* )( int indFunc );
		static const array<pfnFunctionType, eInternalFunc::valuesCount> s_functionTypeInternal;
		template<eInternalFunc id>
		eVarType functionTypeInternal( int indFunc );

		eVarType nodeType( int indNode );
		eVarType expressionType( int iFirstChild );
		eVarType functionType( int iFunc );

		// ==== Debugging ====

		void dbgPrintList( int ind, int level ) const;
		void dbgPrintNode( int ind, int level ) const;

		// ==== Emit HLSL ====

		struct EmitContext;
		void emitNode( EmitContext& ec, int ind ) const;
		void emitFunction( EmitContext& ec, int ind ) const;

		template<eInternalFunc id>
		void emitInternal( EmitContext& ec, const Node& node, int ind ) const;

		using pfnEmitNode = void ( Tree::* )( EmitContext& ec, const Node& node, int ind ) const;
		static const array<pfnEmitNode, eInternalFunc::valuesCount> s_emitInternal;

	public:

		Tree( SymbolTable& symbolsTable );

		// Clear nodes, keeping the symbols
		void clear();

		// Append a single NSEEL statement, the code must not have '=' inside.
		HRESULT appendStatement( const CStringA& nseel, int begin, int end );

		// Append an assignment NSEEL statement, e.g. "y=f(x)"
		HRESULT appendAssignment( const CStringA& nseel, int begin, int equals, int end );

		HRESULT deduceTypes();

		bool transformDoubleFuncs();

		void dbgPrint() const;

		HRESULT emitHlsl( CStringA& hlsl, bool& usesRng ) const;

		// Gather data on how variables are used by this expression tree. The values are from eVarAccess enum, they're written to the vector with bitwise OR.
		// nBlock bust be between 0 and 3, the values are shifted left by (nExpressionBlock<<2)
		void getVariablesUsage( vector<VarUseFlags>& usage, uint8_t nExpressionBlock ) const;
	};
}