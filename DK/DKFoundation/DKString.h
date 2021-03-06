//
//  File: DKString.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2015 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "DKStringUE.h"
#include "DKStringU8.h"
#include "DKStringW.h"
#include "DKMap.h"
#include "DKSet.h"

////////////////////////////////////////////////////////////////////////////////
// DKString
// basic string class.
// using utf-16 for Win32, else utf-32 (for Unix, Mac OS X) with current implementation.
// using wchar_t for character type. (see DKStringW.h)
////////////////////////////////////////////////////////////////////////////////

namespace DKFoundation
{
	typedef DKStringW DKString;

	// Template Spealization for DKString. (for DKMap, DKSet)
	template <> struct DKMapKeyComparator<DKStringW>
	{
		int operator () (const DKStringW& lhs, const DKStringW& rhs) const
		{
			return lhs.Compare(rhs);
		}
	};
	template <> struct DKMapKeyComparator<DKStringU8>
	{
		int operator () (const DKStringU8& lhs, const DKStringU8& rhs) const
		{
			return lhs.Compare(rhs);
		}
	};
	template <> struct DKSetComparator<DKStringW>
	{
		int operator () (const DKStringW& lhs, const DKStringW& rhs) const
		{
			return lhs.Compare(rhs);
		}
	};
	template <> struct DKSetComparator<DKStringU8>
	{
		int operator () (const DKStringU8& lhs, const DKStringU8& rhs) const
		{
			return lhs.Compare(rhs);
		}
	};

}
