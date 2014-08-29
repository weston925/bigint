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

#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include "BigUnsigned.h"

namespace BigNumber
{
	/// Required declarations of functions
	bool operator ==(const BigInteger &, const BigInteger &);
	bool operator <(const BigInteger &, const BigInteger &);

	class BigInteger
	{
		/// Friend classes and functions
		friend BigIntegerUtil;
		friend bool operator ==(const BigInteger &, const BigInteger &);
		friend bool operator <(const BigInteger &, const BigInteger &);

	public:
		/// Constructor
		BigInteger() {}

		BigInteger(BigInteger &&rhs) :data(std::move(rhs.data)), negative(rhs.negative) { rhs.negative = false; }

		/// Construct from integral type
		template <typename T>
		BigInteger(const T &other, std::enable_if_t<std::is_integral<T>::value>* = nullptr) { operator =(other); }

		/// Destructor
		~BigInteger() {}

		/// Conversion operator to bool
		explicit operator bool() const;

		/// Conversion operator to integral type
		template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>> explicit operator T() const;

		/// Assignment operators
		BigInteger &operator =(const BigInteger &) = default;
		BigInteger &operator =(BigInteger &&);
		BigInteger &operator =(const bool &);
		template <typename T> std::enable_if_t<std::is_integral<T>::value, BigInteger> &operator =(const T &);

		/// Compound assignment operators
		BigInteger &operator +=(const BigInteger &);
		BigInteger &operator -=(const BigInteger &);
		BigInteger &operator *=(const BigInteger &);
		BigInteger &operator /=(const BigInteger &);
		BigInteger &operator %=(const BigInteger &);
		BigInteger &operator &=(const BigInteger &);
		BigInteger &operator |=(const BigInteger &);
		BigInteger &operator ^=(const BigInteger &);
		BigInteger &operator <<=(const BigInteger &);
		BigInteger &operator >>=(const BigInteger &);

		/// Bitwise NOT
		BigInteger operator ~() const;

		/// Unary minus
		BigInteger operator -() const;

		/// Prefix increment and decrement
		BigInteger &operator ++();
		BigInteger &operator --();

		/// Postfix increment and decrement
		BigInteger operator ++(int);
		BigInteger operator --(int);

	private:
		typedef BigUnsigned::dataType dataType;
		typedef BigUnsigned::colType colType;
		static const size_t dataTypeSize = 8 * sizeof(dataType);

		BigUnsigned data;
		bool negative = false;
	};

	/// Conversion operator to integral type
	template <typename T, typename>
	BigInteger::operator T() const
	{
		T retVal = 0; // return value

		if (data) // check if not zero
		{
			if (negative) // check if the data is negative
			{
				// since the data is negative it can only be
				// put in a signed data type so check if the
				// return type is signed and throw a runtime
				// error if it isn't

				if (std::is_signed<T>::value)
				{
					// check if the data is bigger than or equal to
					// the smallest possible value of the return type
					// and throw an underflow error if it isn't

					if (*this >= std::numeric_limits<T>::min())
					{
						// this next if statement should always be true but it's here
						// just in case some idiot decides to change the class data
						// type to an unsigned char or something
						if (sizeof(dataType) >= sizeof(T))
							retVal = (T)data.pData->front(); // copy the data to the return value
						else
						{
							// the size of the class's data type is smaller than
							// the return data type so it is possible for there
							// to be more than one item in the data collection.

							auto iter = begin(*data.pData);

							for (size_t i = 0; i < sizeof(T) / sizeof(dataType); ++i)
							{
								// shift the return value to the right
								retVal >>= dataTypeSize;

								if (iter != end(*data.pData)) // check if there is more data
								{
									memcpy(&retVal, &*iter, sizeof(dataType)); // copy the data to the return value
									++iter; // increment the iterator
								}
							}
						}

						// it is possible to set the smallest possible
						// value of the return type and when it is that
						// value, it is already negative so only
						// multiply it by -1 when it isn't that value

						if (retVal << 1 != 0)
							retVal *= -1;
					}
					else
						std::underflow_error("BigInteger: Value is too small to fit in the requested type");
				}
				else
					std::runtime_error("BigInteger: Cannot put a negative number in an unsigned type");
			}
			else
				retVal = (T)data; // the data is positive so use the data's conversion operator
		}

		return retVal;
	}

	/// Integral assignment operator
	template <typename T>
	std::enable_if_t<std::is_integral<T>::value, BigInteger> &BigInteger::operator =(const T &rhs)
	{
		if (rhs < 0) // check if rhs is negative
		{
			// this can use the data's assignment operator
			// for every situation except when rhs is T's
			// smallest possible value

			if (rhs << 1 != 0) // check if rhs is not T's smallest possible value
				data = rhs * -1;
			else
			{
				// set the data to zero
				data.clearData();

				// this next if statement should always be true. see comments
				// in the function above this one as to why it could be false
				// and why it's here
				if (sizeof(dataType) >= sizeof(T))
					data.pData->push_back((dataType)rhs); // insert rhs into the data
				else
				{
					// size of the class's data type is smaller than
					// rhs's data type, so it takes more than one
					// insertion to get all of rhs's data

					// insert zero into data the number of times T is bigger than dataType
					data.pData->insert(begin(*data.pData), sizeof(T) / sizeof(dataType), 0);

					// copy rhs into the last item
					memcpy(&data.pData->back(), &rhs, sizeof(dataType));
				}
			}

			// rhs is negative so set negative to true
			negative = true;
		}
		else
		{
			// rhs is positive so use the data's assignment operator
			// and set negative to false
			data = rhs;
			negative = false;
		}

		return *this;
	}

	/// Arithmetic operators
	inline BigInteger operator +(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) += rhs);
	}

	inline BigInteger operator -(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) -= rhs);
	}

	inline BigInteger operator *(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) *= rhs);
	}

	inline BigInteger operator /(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) /= rhs);
	}

	inline BigInteger operator %(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) %= rhs);
	}

	inline BigInteger operator +(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs += rhs);
	}

	inline BigInteger operator -(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs -= rhs);
	}

	inline BigInteger operator *(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs *= rhs);
	}

	inline BigInteger operator /(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs /= rhs);
	}

	inline BigInteger operator %(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs %= rhs);
	}

	/// Bitwise operators
	inline BigInteger operator &(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) &= rhs);
	}

	inline BigInteger operator |(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) |= rhs);
	}

	inline BigInteger operator ^(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) ^= rhs);
	}

	inline BigInteger operator <<(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) <<= rhs);
	}

	inline BigInteger operator >>(const BigInteger &lhs, const BigInteger &rhs)
	{
		return std::move(BigInteger(lhs) >>= rhs);
	}

	inline BigInteger operator &(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs &= rhs);
	}

	inline BigInteger operator |(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs |= rhs);
	}

	inline BigInteger operator ^(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs ^= rhs);
	}

	inline BigInteger operator <<(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs <<= rhs);
	}

	inline BigInteger operator >>(BigInteger &&lhs, const BigInteger &rhs)
	{
		return std::move(lhs >>= rhs);
	}

	/// Comparison operators
	inline bool operator !=(const BigInteger &lhs, const BigInteger &rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator >(const BigInteger &lhs, const BigInteger &rhs)
	{
		return rhs < lhs;
	}

	inline bool operator <=(const BigInteger &lhs, const BigInteger &rhs)
	{
		return !(rhs < lhs);
	}

	inline bool operator >=(const BigInteger &lhs, const BigInteger &rhs)
	{
		return !(lhs < rhs);
	}
} // namespace BigNumber

#endif // BIGINTEGER_H
