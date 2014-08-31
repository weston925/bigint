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
		static BigInteger toSigned(const BigUnsigned &);
		static BigUnsigned toUnsigned(const BigInteger &);

		static BigUnsigned stringToBigUnsigned(std::string);
		static BigInteger stringToBigInteger(std::string);

		static std::string bigUnsignedToString(const BigUnsigned &, std::ios_base::fmtflags = std::ios_base::dec);
		static std::string bigIntegerToString(const BigInteger &, std::ios_base::fmtflags = std::ios_base::dec);

		static void clear(BigUnsigned &);
		static void clear(BigInteger &);

		static BigInteger abs(const BigInteger &);

	private:
		static std::string convertToOct(const BigUnsigned &);
		static std::string convertToDec(const BigUnsigned &);
		static std::string convertToHex(const BigUnsigned &, bool);
		static bool strHalf(std::string &);
		static void strAdd(std::string &, const std::string &);
		static void strDouble(std::string &);
	};

	std::ostream &operator <<(std::ostream &, const BigUnsigned &);
	std::ostream &operator <<(std::ostream &, const BigInteger &);
	std::istream &operator >>(std::istream &, BigUnsigned &);
	std::istream &operator >>(std::istream &, BigInteger &);
} // namespace BigNumber

#endif // BIGINTUTILITIES_H
