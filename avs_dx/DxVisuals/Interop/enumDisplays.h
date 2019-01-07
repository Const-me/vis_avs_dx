#pragma once
#include "drawDx.h"

CStringA displayName( const CStringA& deviceName );

void enumDisplays( CAtlArray<sDisplayInfo>& list );

const sDisplayInfo* displayOrDefault( const CAtlArray<sDisplayInfo>& list, CStringA& deviceName );