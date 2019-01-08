#include "stdafx.h"
#include "TableData.h"
#include <atlpath.h>
#include <Utils/DirectXErrors/DirectXErrors.h>
#include <EASTL/sort.h>

void TableData::clear()
{
	measures.clear();
	mapColumns.RemoveAll();
	mapEffects.RemoveAll();
	names.clear();
}

TableData::TableData() :
	mapEffects( 41, 0.75f, 0.25f, 1.25f, 64 ),
	mapColumns( 103, 0.75f, 0.25f, 1.25f, 256 )
{ }

void TableData::add( uint32_t frame, const vector<sProfilerEntry>& entries )
{
	if( !m_file )
		return;

	if( entries.empty() )
		return;

	for( POSITION pos = mapEffects.GetStartPosition(); pos;)
	{
		Effect& fx = mapEffects.GetNextValue( pos );
		fx.currentIndex = 0;
	}

	Entry newEntry{ frame, 0, 0, 0 };

	uint64_t prevColumn = 0;
	for( const auto &e : entries )
	{
		newEntry.milliseconds = e.milliseconds;

		auto p = mapEffects.Lookup( e.measure );
		if( nullptr != p )
		{
			// Add a new data point, we already saw that effect
			Effect& fx = p->m_value;
			fx.currentIndex++;
			const uint64_t ck = fx.columnKey();
			newEntry.id = fx.id;
			newEntry.index = fx.currentIndex;
			measures.push_back( newEntry );

			if( fx.currentIndex <= fx.totalIndices )
			{
				// And, we already saw and placed that column
				prevColumn = ck;
				continue;
			}

			// Add column
			assert( nullptr == mapColumns.Lookup( ck ) );
			insertColumnAfter( ck, prevColumn );
			fx.totalIndices = fx.currentIndex;

			prevColumn = ck;
		}
		else
		{
			// Add name
			names.push_back( e.measure );
			const uint32_t id = names.size();

			// Add effect
			Effect fx;
			fx.id = id;
			fx.currentIndex = 1;
			fx.totalIndices = 1;
			mapEffects.SetAt( e.measure, fx );

			newEntry.id = id;
			newEntry.index = 1;
			measures.push_back( newEntry );

			// Add column
			const uint64_t ck = fx.columnKey();
			assert( nullptr == mapColumns.Lookup( ck ) );
			insertColumnAfter( ck, prevColumn );
			prevColumn = ck;
		}
	}
}

void TableData::insertColumnAfter( uint64_t newCol, uint64_t prevCol )
{
	uint32_t insertAt;
	if( prevCol <= 0 )
		insertAt = 0;
	else
		insertAt = mapColumns[ prevCol ] + 1;

	for( POSITION pos = mapColumns.GetStartPosition(); pos;)
	{
		uint32_t& index = mapColumns.GetNextValue( pos );
		if( index <= insertAt )
			continue;
		index++;
	}

	mapColumns.SetAt( newCol, insertAt );
}

void TableData::reportError( HRESULT hr, const char* what, HWND wnd )
{
	CString msg{ what };
	msg.Trim();
	msg += L"\r\n";
	msg += formatDxMessageW( hr );

	MessageBoxW( wnd, msg, L"Operation failed", MB_OK | MB_ICONERROR );
}

#define CHECK_MSG( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { reportError( __hr, what, wnd ); return false; } }

static const LPCTSTR fileName = L"avs-gpu-perf.tsv";

bool TableData::start( HWND wnd )
{
	clear();

	const char* what = "Unable to open the log file.";
	CPath dest;
	if( !dest.m_strPath.GetEnvironmentVariable( L"TEMP" ) )
		CHECK_MSG( getLastHr() );

	dest.Append( fileName );
	if( dest.FileExists() )
		if( !DeleteFile( dest.m_strPath ) )
			CHECK_MSG( getLastHr() );

	CHECK_MSG( m_file.Create( dest.m_strPath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS ) );

	return true;
}

HRESULT TableData::write( CAtlFile& file, CStringA& buffer )
{
	const HRESULT hr = file.Write( cstr( buffer ), (DWORD)buffer.GetLength() );
	buffer = "";
	return hr;
}

HRESULT TableData::appendRow( CAtlFile& file, uint32_t frame, const vector<float>& rows, CStringA& buffer )
{
	buffer.AppendFormat( "%i", frame );
	for( float f : rows )
	{
		if( f < -0.01f )
			buffer += '\t';
		else
			buffer.AppendFormat( "\t%.4f", f );
	}
	buffer += "\r\n";
	if( buffer.GetLength() < 0x10000 )
		return S_FALSE;
	return write( file, buffer );
}

bool TableData::stop( HWND wnd )
{
	if( !m_file )
		return false;

	// Swap m_file with a local variable
	CAtlFile hFile;
	hFile.Attach( m_file.Detach() );
	if( measures.empty() )
		return false;

	CStringA result;
	result.Preallocate( 0x11000 );

	// First line, with column names
	vector<float> row;
	const uint32_t firstFrame = measures.begin()->frame;
	{
		struct sCol
		{
			uint32_t order;
			uint64_t id;

			bool operator < ( const sCol &that ) const
			{
				return order < that.order;
			}
		};
		vector<sCol> columns;
		columns.reserve( mapColumns.GetCount() );
		for( POSITION pos = mapColumns.GetStartPosition(); pos; )
		{
			sCol sc;
			mapColumns.GetNextAssoc( pos, sc.id, sc.order );
			columns.push_back( sc );
		}

		eastl::sort( columns.begin(), columns.end() );

		result.AppendFormat( "frame %i+", firstFrame );
		for( const sCol& c : columns )
		{
			result += '\t';
			result += names[ ( c.id >> 32 ) - 1 ];
			const uint32_t sn = (uint32_t)( c.id );
			if( sn <= 1 )
				continue;
			result.AppendFormat( "#%i", sn );
		}
		result += "\r\n";

		row.resize( columns.size() );
		eastl::fill( row.begin(), row.end(), -1 );
	}

	const char* what = "Error writing the data";
	{
		uint32_t frames = 0;
		uint32_t prevFrame = measures.begin()->frame;
		for( const Entry& e : measures )
		{
			const auto column = mapColumns[ e.columnKey() ];
			if( e.frame == prevFrame )
			{
				row[ column ] = e.milliseconds;
				continue;
			}

			CHECK_MSG( appendRow( hFile, prevFrame - firstFrame, row, result ) );
			eastl::fill( row.begin(), row.end(), -1 );
			prevFrame = e.frame;
			row[ column ] = e.milliseconds;
			frames++;
		}
		CHECK_MSG( write( hFile, result ) );
		frames++;

		result.Format( "Wrote %i frames to %%TEMP%%\\%S.", frames, fileName );
		MessageBoxA( wnd, result, "Completed OK", MB_OK | MB_ICONINFORMATION );
		clear();
	}
	return true;
}