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

#include "BigInteger.h"

using namespace std;

namespace BigNumber
{
	/// Conversion operator to bool
	BigInteger::operator bool() const
	{
		// this returns false only when the data is zero

		// negative is always false when the data is zero
		// so return true if negative is true
		if (negative)
			return true;
		else
			return (bool)data; // return BigUnsigned's bool conversion
	}

	/// Move assignment operator
	BigInteger &BigInteger::operator =(BigInteger &&rhs)
	{
		data = move(rhs.data);
		negative = rhs.negative;
		rhs.negative = false;
		return *this;
	}

	/// Bool assignment operator
	BigInteger &BigInteger::operator =(const bool &rhs)
	{
		data = rhs; // set the data
		negative = false; // a boolean value is never negative
		return *this;
	}

	/// Addition compound assignment operator
	BigInteger &BigInteger::operator +=(const BigInteger &rhs)
	{
		if (rhs) // check if not zero
		{
			if (*this) // check if not zero
			{
				// if they both have the same negative value, leave the
				// negative the same and add the data
				if (negative == rhs.negative)
					data += rhs.data;
				else
				{
					if (data >= rhs.data)
					{
						// this data is greater than or equal to rhs's data, so
						// it is safe to subtract them normally
						data -= rhs.data;

						if (!data)
							negative = false; // make sure negative is false when the data is zero
					}
					else
					{
						// this data is smaller than rhs's data, so subtract
						// rhs's data from this data and swap the negative
						data = rhs.data - data;
						negative = rhs.negative;
					}
				}
			}
			else
				operator =(rhs); // this data is zero so just copy rhs
		}

		return *this;
	}

	/// Subtraction compound assignment operator
	BigInteger &BigInteger::operator -=(const BigInteger &rhs)
	{
		// change the negative value of rhs and use += since it
		// covers both positive and negative values
		return operator +=(-rhs);
	}

	/// Multiplication compound assignment operator
	BigInteger &BigInteger::operator *=(const BigInteger &rhs)
	{
		data *= rhs.data;

		if (data)
			negative = negative != rhs.negative; // the result is positive when the negative values are the same
		else
			negative = false; // make sure negative is false when the data is zero

		return *this;
	}

	/// Division compound assignment operator
	BigInteger &BigInteger::operator /=(const BigInteger &rhs)
	{
		data /= rhs.data;

		if (data)
			negative = negative != rhs.negative; // the result is positive when the negative values are the same
		else
			negative = false; // make sure negative is false when the data is zero

		return *this;
	}

	/// Modulo compound assignment operator
	BigInteger &BigInteger::operator %=(const BigInteger &rhs)
	{
		data %= rhs.data;

		// the negative value of this data stays the same no matter
		// what rhs's negative value is

		if (!data)
			negative = false; // make sure negative is false when the data is zero

		return *this;
	}

	/// Bitwise AND compound assignment operator
	BigInteger &BigInteger::operator &=(const BigInteger &rhs)
	{
		data &= rhs.data;

		if (data)
			negative &= rhs.negative;
		else
			negative = false; // make sure negative is false when the data is zero

		return *this;
	}

	/// Bitwise OR compound assignment operator
	BigInteger &BigInteger::operator |=(const BigInteger &rhs)
	{
		data |= rhs.data;
		negative |= rhs.negative;
		return *this;
	}

	/// Bitwise XOR compound assignment operator
	BigInteger &BigInteger::operator ^=(const BigInteger &rhs)
	{
		data ^= rhs.data;

		if (data)
			negative ^= rhs.negative;
		else
			negative = false; // make sure negative is false when the data is zero

		return *this;
	}

	/// Bitshift left compound assignment operator
	BigInteger &BigInteger::operator <<=(const BigInteger &rhs)
	{
		if (rhs.negative)
			operator >>=(-rhs); // rhs is negative, so shift the other way
		else
			data <<= rhs.data;

		return *this;
	}

	/// Bitshift right compound assignment operator
	BigInteger &BigInteger::operator >>=(const BigInteger &rhs)
	{
		if (rhs.negative)
			operator <<=(-rhs); // rhs is negative, so shift the other way
		else
		{
			data >>= rhs.data;

			if (!data)
				negative = false; // make sure negative is false when the data is zero
		}

		return *this;
	}

	/// Bitwise NOT
	BigInteger BigInteger::operator ~() const
	{
		BigInteger retVal; // return value

		retVal.data = ~data; // set the data

		// only change the negative if the data isn't zero
		if (retVal.data)
			retVal.negative = !negative; // flip the negative sign

		return move(retVal);
	}

	/// Unary minus
	BigInteger BigInteger::operator -() const
	{
		if (*this) // check if not zero
		{
			BigInteger retVal(*this); // return value

			retVal.negative = !retVal.negative; // flip the negative sign

			return move(retVal);
		}
		else
			return *this;
	}

	/// Prefix increment
	BigInteger &BigInteger::operator ++()
	{
		return operator +=(1);
	}

	/// Prefix decrement
	BigInteger &BigInteger::operator --()
	{
		return operator -=(1);
	}

	/// Postfix increment
	BigInteger BigInteger::operator ++(int unused)
	{
		BigInteger temp(*this);
		operator ++();
		return move(temp);
	}

	/// Postfix decrement
	BigInteger BigInteger::operator --(int unused)
	{
		BigInteger temp(*this);
		operator --();
		return move(temp);
	}

	/// Equal to operator
	bool operator ==(const BigInteger &lhs, const BigInteger &rhs)
	{
		return lhs.negative == rhs.negative && lhs.data == rhs.data;
	}

	/// Less than operator
	bool operator <(const BigInteger &lhs, const BigInteger &rhs)
	{
		if (lhs.negative == rhs.negative) // check if the negatives are the same
		{
			if (!lhs.negative)
				return lhs.data < rhs.data; // both are positive so return true if lhs is less than rhs
			else
				return rhs.data < lhs.data; // both are negative so return true if rhs is less than lhs
		}
		else
			return lhs.negative; // the negatives are different so return true if lhs is the negative one
	}
} // namespace BigNumber
