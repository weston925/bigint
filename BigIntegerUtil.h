/*

C++ Big Integer Library
Copyright (C) 2014 Weston Witt

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
USA


Email address: weston925@gmail.com

*/

#ifndef BIGINTUTILITIES_H
#define BIGINTUTILITIES_H

#include "BigInteger.h"
#include <iostream>
#include <string>
#include <sstream>

namespace BigNumber
{
	class BigIntegerUtil
	{
	public:
		/// Conversions between signed and unsigned types
		static BigInteger toSigned(const BigUnsigned &);
		static BigUnsigned toUnsigned(const BigInteger &);

		/// Conversions from a string to a BigNumber
		static BigUnsigned stringToBigUnsigned(std::string, unsigned int = 10);
		static BigInteger stringToBigInteger(std::string, unsigned int = 10);

		/// Conversions from a BigNumber to a string
		static std::string bigUnsignedToString(BigUnsigned, unsigned int = 10);
		static std::string bigIntegerToString(const BigInteger &, unsigned int = 10);

		/// Set a BigNumber to zero
		static void clear(BigUnsigned &);
		static void clear(BigInteger &);

		/// Get the absolute value of a BigInteger
		static BigInteger abs(const BigInteger &);
	};

	/// ostream and istream operator overloads
	std::ostream &operator <<(std::ostream &, const BigUnsigned &);
	std::ostream &operator <<(std::ostream &, const BigInteger &);
	std::istream &operator >>(std::istream &, BigUnsigned &);
	std::istream &operator >>(std::istream &, BigInteger &);
} // namespace BigNumber

#endif // BIGINTUTILITIES_H
