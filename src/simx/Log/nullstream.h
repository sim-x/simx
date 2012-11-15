// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//
// Copyright Maciej Sobczak, 2002
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//

#ifndef NULLSTREAM_H_INCLUDED
#define NULLSTREAM_H_INCLUDED

#include <streambuf>
#include <ostream>

// null stream buffer class
template <class charT, class traits = std::char_traits<charT> >
class NullBuffer : public std::basic_streambuf<charT, traits>
{
public:
	typedef typename std::basic_streambuf<charT, traits>::int_type int_type;

	NullBuffer() {}

private:
	virtual int_type overflow(int_type c)
	{
		// just ignore the character
		return traits::not_eof(c);
	}
};

// generic null output stream class
template <class charT, class traits = std::char_traits<charT> >
class GenericNullStream
	: private NullBuffer<charT, traits>,
	public std::basic_ostream<charT, traits>
{
public:
	GenericNullStream()
		: std::basic_ostream<charT, traits>(this)
	{
	}
};

// helper declarations for narrow and wide streams
typedef GenericNullStream<char> NullStream;
typedef GenericNullStream<wchar_t> WNullStream;

#endif // NULLSTREAM_H_INCLUDED
