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

#ifndef BIGUNSIGNED_H
#define BIGUNSIGNED_H

#include <memory>
#include <deque>

namespace BigNumber
{
	/// Required declarations of classes and functions
	class BigIntegerUtil;
	class BigUnsigned;
	class BigInteger;
	bool operator ==(const BigUnsigned &, const BigUnsigned &);
	bool operator <(const BigUnsigned &, const BigUnsigned &);

	// template alias for commonly used statement
	template <typename T>
	using EnableIfIntegral = typename std::enable_if<std::is_integral<T>::value, int>::type;

	class BigUnsigned
	{
		/// Friend classes and functions
		friend BigIntegerUtil;
		friend BigInteger;
		friend bool operator ==(const BigUnsigned &, const BigUnsigned &);
		friend bool operator <(const BigUnsigned &, const BigUnsigned &);

	public:
		/// Constructor
		BigUnsigned() = default;

		/// Copy constructor
		BigUnsigned(const BigUnsigned &) = default;

		/// Move constructor
		BigUnsigned(BigUnsigned &&rhs) :pData(std::move(rhs.pData)) {}

		/// Construct from integral type
		template <typename T, EnableIfIntegral<T> = 0>
		BigUnsigned(const T &rhs) { operator =(rhs); }

		/// Destructor
		~BigUnsigned() = default;

		/// Conversion operator to bool
		explicit operator bool() const;

		/// Conversion operator to integral type
		template <typename T, EnableIfIntegral<T> = 0> explicit operator T() const;

		/// Assignment operators
		BigUnsigned &operator =(const BigUnsigned &) = default;
		BigUnsigned &operator =(BigUnsigned &&);
		BigUnsigned &operator =(const bool &);
		template <typename T, EnableIfIntegral<T> = 0> BigUnsigned &operator =(const T &);

		/// Compound assignment operators
		BigUnsigned &operator +=(const BigUnsigned &);
		BigUnsigned &operator -=(const BigUnsigned &);
		BigUnsigned &operator *=(const BigUnsigned &);
		BigUnsigned &operator /=(const BigUnsigned &);
		BigUnsigned &operator %=(const BigUnsigned &);
		BigUnsigned &operator &=(const BigUnsigned &);
		BigUnsigned &operator |=(const BigUnsigned &);
		BigUnsigned &operator ^=(const BigUnsigned &);
		BigUnsigned &operator <<=(const BigUnsigned &);
		BigUnsigned &operator >>=(const BigUnsigned &);

		/// Bitwise NOT
		BigUnsigned operator ~() const;

		/// Prefix increment and decrement
		BigUnsigned &operator ++();
		BigUnsigned &operator --();

		/// Postfix increment and decrement
		BigUnsigned operator ++(int);
		BigUnsigned operator --(int);

	private:
		typedef unsigned long dataType; // data type
		typedef std::deque<dataType> colType; // collection type
		static const size_t dataTypeSize = 8 * sizeof(dataType); // number of bits

		std::shared_ptr<colType> pData; // pointer to data

		/// Helper functions
		void clearData();
		void makeDataUnique();
		void removeLeadingZeros();
		void add(const BigUnsigned &);
		void subtract(const BigUnsigned &);
		void multiply(const BigUnsigned &);
		BigUnsigned divideWithRemainder(const BigUnsigned &);
		void bitAND(const BigUnsigned &);
		void bitOR(const BigUnsigned &);
		void bitXOR(const BigUnsigned &);
		void bitShiftLeft(BigUnsigned);
		void bitShiftRight(BigUnsigned);
	};

	/// Conversion operator to integral
	template <typename T, EnableIfIntegral<T>>
	BigUnsigned::operator T() const
	{
		T retVal = 0; // return value

		if (*this) // check if not zero
		{
			// check if this is smaller than or equal to the
			// return type's maximum value and throw an
			// overflow error if it isn't

			if (*this <= std::numeric_limits<T>::max())
			{
				// this next if statement should always be true but it's here
				// just in case some idiot decides to change the class data
				// type to an unsigned char or something
				if (sizeof(dataType) >= sizeof(T))
					retVal = (T)pData->front(); // copy the data to the return value
				else
				{
					// the size of the class's data type is smaller than
					// the return data type so it is possible for there
					// to be more than one item in the data collection.

					auto iter = begin(*pData);

					for (size_t i = 0; i < sizeof(T) / sizeof(dataType); ++i)
					{
						// shift the return value to the right
						retVal >>= dataTypeSize;

						if (iter != end(*pData)) // check if there is more data
						{
							memcpy(&retVal, &*iter, sizeof(dataType)); // copy the data to the return value
							++iter; // increment the iterator
						}
					}
				}
			}
			else
				std::overflow_error("BigUnsigned: Value is too big to fit in the requested type");
		}

		return retVal;
	}

	/// Integral assignment operator
	template <typename T, EnableIfIntegral<T>>
	BigUnsigned &BigUnsigned::operator =(const T &rhs)
	{
		// set the data to zero
		clearData();

		if (rhs != 0) // check if rhs is not zero
		{
			if (rhs < 0)
				std::invalid_argument("BigUnsigned: Cannot set from a negative number");

			// this next if statement should always be true. see comments
			// in the function above this one as to why it could be false
			// and why it's here
			if (sizeof(dataType) >= sizeof(T))
				pData->push_back((dataType)rhs); // insert rhs into the data
			else
			{
				// size of the class's data type is smaller than
				// rhs's data type, so it takes more than one
				// insertion to get all of rhs's data

				// rhs is const so make a copy of it
				T value = rhs;

				// loop the amount of times T is bigger than dataType
				for (size_t i = 0; i < sizeof(T) / sizeof(dataType); ++i)
				{
					// convert value to dataType ignoring the possible overflow
					// then shift it to the right to get the next value to
					// insert
					pData->push_back((dataType)value);
					value >>= dataTypeSize;
				}

				// remove possible leading zeros
				removeLeadingZeros();
			}
		}

		return *this;
	}

	/// Arithmetic operators
	inline BigUnsigned operator +(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) += rhs);
	}

	inline BigUnsigned operator -(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) -= rhs);
	}

	inline BigUnsigned operator *(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) *= rhs);
	}

	inline BigUnsigned operator /(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) /= rhs);
	}

	inline BigUnsigned operator %(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) %= rhs);
	}
	
	inline BigUnsigned operator +(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs += rhs);
	}
	
	inline BigUnsigned operator -(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs -= rhs);
	}

	inline BigUnsigned operator *(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs *= rhs);
	}

	inline BigUnsigned operator /(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs /= rhs);
	}

	inline BigUnsigned operator %(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs %= rhs);
	}

	/// Bitwise operators
	inline BigUnsigned operator &(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) &= rhs);
	}

	inline BigUnsigned operator |(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) |= rhs);
	}

	inline BigUnsigned operator ^(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) ^= rhs);
	}

	inline BigUnsigned operator <<(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) <<= rhs);
	}

	inline BigUnsigned operator >>(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return std::move(BigUnsigned(lhs) >>= rhs);
	}

	inline BigUnsigned operator &(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs &= rhs);
	}

	inline BigUnsigned operator |(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs |= rhs);
	}

	inline BigUnsigned operator ^(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs ^= rhs);
	}

	inline BigUnsigned operator <<(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs <<= rhs);
	}

	inline BigUnsigned operator >>(BigUnsigned &&lhs, const BigUnsigned &rhs)
	{
		return std::move(lhs >>= rhs);
	}

	/// Comparison operators
	inline bool operator !=(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator >(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return rhs < lhs;
	}

	inline bool operator <=(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return !(rhs < lhs);
	}

	inline bool operator >=(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		return !(lhs < rhs);
	}
} // namespace BigNumber

#endif // BIGUNSIGNED_H
