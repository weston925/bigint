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

#include "BigUnsigned.h"

using namespace std;

namespace BigNumber
{
	/// Conversion operator to bool
	BigUnsigned::operator bool() const
	{
		if (pData) // check if not nullptr
			return !pData->empty(); // return true if not empty
		else
			return false;
	}

	/// Move assignment operator
	BigUnsigned &BigUnsigned::operator =(BigUnsigned &&rhs)
	{
		pData = move(rhs.pData);
		return *this;
	}

	/// Bool assignment operator
	BigUnsigned &BigUnsigned::operator =(const bool &rhs)
	{
		// set the data to zero
		clearData();

		if (rhs)
			pData->push_back(1); // set the data to one if true

		return *this;
	}

	/// Addition compound assignment operator
	BigUnsigned &BigUnsigned::operator +=(const BigUnsigned &rhs)
	{
		if (rhs) // check if not zero
		{
			// things would mess up pretty badly if
			// they are the exact same data (a += a)
			// so makes sure it's not the same before
			// adding
			if (this != &rhs)
			{
				if (*this) // check if not zero
					add(rhs); // add rhs into this
				else
					operator =(rhs); // the data is zero so just copy rhs
			}
			else
			{
				// this and rhs is the exact same data so
				// this creates a temporary BigUnsigned
				// using rhs and then adds them together
				add(BigUnsigned(rhs));
			}
		}

		return *this;
	}

	/// Subtraction compound assignment operator
	BigUnsigned &BigUnsigned::operator -=(const BigUnsigned &rhs)
	{
		if (rhs) // check if not zero
		{
			if (*this > rhs)
				subtract(rhs); // subtract rhs from this
			else if (*this == rhs)
				clearData(); // subtracting two of the same value returns zero
			else
				runtime_error("BigUnsigned: Negative result in unsigned calculation");
		}

		return *this;
	}

	/// Multiplication compound assignment operator
	BigUnsigned &BigUnsigned::operator *=(const BigUnsigned &rhs)
	{
		if (*this) // check if not zero
		{
			// things would mess up pretty badly if
			// they are the exact same data (a *= a)
			// so makes sure it's not the same before
			// multiplying
			if (this != &rhs)
			{
				if (rhs) // check if not zero
					multiply(rhs); // multiply this by rhs
				else
					clearData(); // anything multiplied by zero is zero
			}
			else
			{
				// this and rhs is the exact same data so
				// this creates a temporary BigUnsigned
				// using rhs and then multiplies them
				// together
				multiply(BigUnsigned(rhs));
			}
		}

		return *this;
	}

	/// Division compound assignment operator
	BigUnsigned &BigUnsigned::operator /=(const BigUnsigned &rhs)
	{
		// divideWithRemainder does all the checking
		// needed so just call the function
		divideWithRemainder(rhs);
		return *this;
	}

	/// Modulo compound assignment operator
	BigUnsigned &BigUnsigned::operator %=(const BigUnsigned &rhs)
	{
		// the return value of divideWithRemainder is
		// the remainder so just call the function and
		// save the results
		return operator =(divideWithRemainder(rhs));
	}

	/// Bitwise AND compound assignment operator
	BigUnsigned &BigUnsigned::operator &=(const BigUnsigned &rhs)
	{
		if (*this) // check if not zero
		{
			if (rhs) // check if not zero
				bitAND(rhs); // this function does the rest
			else
				clearData(); // rhs is zero so the result is zero
		}

		return *this;
	}

	/// Bitwise OR compound assignment operator
	BigUnsigned &BigUnsigned::operator |=(const BigUnsigned &rhs)
	{
		if (rhs) // check if not zero
		{
			if (*this) // check if not zero
				bitOR(rhs); // this function does the rest
			else
				operator =(rhs); // this is zero so copy rhs
		}

		return *this;
	}

	/// Bitwise XOR compound assignment operator
	BigUnsigned &BigUnsigned::operator ^=(const BigUnsigned &rhs)
	{
		if (rhs) // check if not zero
		{
			if (*this) // check if not zero
				bitXOR(rhs); // this function does the rest
			else
				operator =(rhs); // this is zero so copy rhs
		}

		return *this;
	}

	/// Bitshift left compound assignment operator
	BigUnsigned &BigUnsigned::operator <<=(const BigUnsigned &rhs)
	{
		if (*this && rhs) // make sure both this and rhs is not zero
			bitShiftLeft(rhs); // this function does the rest

		return *this;
	}

	/// Bitshift right compound assignment operator
	BigUnsigned &BigUnsigned::operator >>=(const BigUnsigned &rhs)
	{
		if (*this && rhs) // make sure both this and rhs is not zero
			bitShiftRight(rhs); // this function does the rest

		return *this;
	}

	/// Bitwise NOT
	BigUnsigned BigUnsigned::operator ~() const
	{
		BigUnsigned val(*this); // return value with copy of this

		if (val) // check if not zero
		{
			val.makeDataUnique(); // make the data unique so we can mess with it
			for (auto &data : *val.pData) // loop through every item
				data = ~data; // invert the data
		}

		return move(val);
	}

	/// Prefix increment
	BigUnsigned &BigUnsigned::operator ++()
	{
		return operator +=(1);
	}

	/// Prefix decrement
	BigUnsigned &BigUnsigned::operator --()
	{
		return operator -=(1);
	}

	/// Postfix increment
	BigUnsigned BigUnsigned::operator ++(int)
	{
		BigUnsigned temp(*this);
		operator ++();
		return move(temp);
	}

	/// Postfix decrement
	BigUnsigned BigUnsigned::operator --(int)
	{
		BigUnsigned temp(*this);
		operator --();
		return move(temp);
	}

	// clearData safely sets the data to zero
	void BigUnsigned::clearData()
	{
		if (pData.unique()) // check if the data is unique
			pData->clear(); // clear it using the data's member function
		else
			pData = make_shared<colType>(); // make a new shared pointer since it's not unique
	}

	// makeDataUnique makes the data unique so the data can be safely messed with
	void BigUnsigned::makeDataUnique()
	{
		if (!pData.unique()) // check if the data is not unique
		{
			if (pData) // check if the pointer is not a nullptr
				pData = make_shared<colType>(*pData); // make a new shared pointer with the data copied
			else
				pData = make_shared<colType>(); // make a new shared pointer
		}
	}

	// removeLeadingZeros does exactly what it sounds like
	void BigUnsigned::removeLeadingZeros()
	{
		while (!pData->empty()) // loop until empty or break is called
		{
			if (pData->back() == 0)
				pData->pop_back(); // the back is zero so remove the leading zero
			else
				break; // the back is not zero so exit
		}
	}

	// addition helper function
	void BigUnsigned::add(const BigUnsigned &rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		auto iter = begin(*rVal.pData); // iterator to beginning of rVal
		bool carry = false; // the carry bit

		// loop through the data
		for (auto &data : *pData)
		{
			if (iter != end(*rVal.pData)) // check if the iterator didn't end
			{
				data += *iter; // add the data

				if (data < *iter) // check for overflow
				{
					// add one if there is a carry bit
					if (carry)
						++data;

					carry = true; // because of overflow the next one has a carry bit
				}
				else if (carry)
				{
					++data; // add the carry bit
					carry = (data == 0); // check if there is still overflow
				}

				++iter; // increment the iterater
			}
			else if (carry)
			{
				++data; // add the carry bit
				carry = (data == 0); // check if there is still overflow
			}
			else
				break; // nothing else to add so exit
		}

		// loop through the rest of rVal if this data ended before it did
		for (; iter != end(*rVal.pData); ++iter)
		{
			if (carry)
			{
				pData->push_back(*iter + 1); // insert the data and add the carry bit
				carry = (pData->back() == 0); // check if there is still overflow
			}
			else
			{
				// there's nothing left to add so just insert
				// the rest and exit this loop
				pData->insert(end(*pData), iter, end(*rVal.pData));
				break;
			}
		}

		// if there is an extra carry bit, then add it
		if (carry)
			pData->push_back(1);
	}

	// subtraction helper function
	void BigUnsigned::subtract(const BigUnsigned &rVal)
	{
		// this function uses two's compliment to subtract the two values so
		// it will look a lot like the add function.

		// get the data ready to be modified
		makeDataUnique();

		auto iter = begin(*rVal.pData); // iterator to beginning of rVal
		bool carry = true; // the carry bit

		// loop through the data
		for (auto &data : *pData)
		{
			if (iter != end(*rVal.pData)) // check if the iterator didn't end
			{
				data += ~*iter; // add the inverse of the data

				if (data < ~*iter) // check for overflow
				{
					// add one if there is a carry bit
					if (carry)
						++data;

					carry = true; // because of overflow the next one has a carry bit
				}
				else if (carry)
				{
					++data; // add the carry bit
					carry = (data == 0); // check if there is still overflow
				}

				++iter; // increment the iterater
			}
			else if (!carry)
			{
				data += ~(dataType)0; // add the inverse of the data
				carry = (data < ~(dataType)0); // check if there is still overflow
			}
			else
				break; // nothing else to add so exit
		}

		// After subtraction with two's compliment, the most significant bit
		// bit should be removed. But since this should always be bigger than
		// rVal; carry should always be true after the above loop finishes,
		// so just ignore the extra carry bit.

		// remove possible leading zeros
		removeLeadingZeros();
	}

	// multiplication helper function
	void BigUnsigned::multiply(const BigUnsigned &rVal)
	{
		// this function uses the shift and add technique to multiply the values

		BigUnsigned results, shiftAmount;

		// loop through rVal
		for (const auto &data : *rVal.pData)
		{
			// loop through each bit
			for (dataType bitToGet = 1; bitToGet != 0; bitToGet <<= 1)
			{
				// shift and add if the current bit is set
				if ((data & bitToGet) != 0)
				{
					operator <<=(shiftAmount); // shift
					results += *this; // add
					shiftAmount.clearData(); // reset the shift amount
				}

				++shiftAmount; // increment the shift amount
			}
		}

		// move the results into this
		*this = move(results);
	}

	// division and modulus helper function
	BigUnsigned BigUnsigned::divideWithRemainder(const BigUnsigned &rVal)
	{
		if (!rVal) // check if zero
			invalid_argument("BigUnsigned: cannot divide by zero");

		// this is what gets returned
		BigUnsigned remainder;

		if (*this == rVal) // check if they are the same value
		{
			// because they are the same value, the results is one
			// and the remainder is zero
			clearData();
			pData->push_back(1);
		}
		else if (*this < rVal) // check if this is less than rVal
			remainder = move(*this); // the results is zero and the remainder is this
		else
		{
			dataType bit = (dataType)1 << (dataTypeSize - 1); // the current bit set to the left most bit
			BigUnsigned results;

			// loop until the bit is the most significant bit of this
			while ((pData->back() & bit) == 0)
				bit >>= 1; // get the next bit

			// loop through the data
			for (auto iter = rbegin(*pData); iter != rend(*pData); ++iter)
			{
				// this uses a shift and subtract technique to divide the two values
				
				// loop through each bit
				for (; bit != 0; bit >>= 1)
				{
					// check if the remainder is bigger than rVal
					if (remainder >= rVal)
					{
						++results; // increment the results
						remainder -= rVal; // subtract rVal from remainder
					}

					// shift the results and remainder to the left by one
					results <<= 1;
					remainder <<= 1;

					// the new remainder bit recieved by shifting is
					// set to the value of the current bit
					if ((*iter & bit) != 0)
						++remainder;
				}

				// reset the current bit to the left most bit
				bit = (dataType)1 << (dataTypeSize - 1);
			}

			// check if the remainder is bigger than rVal
			if (remainder >= rVal)
			{
				++results; // increment the results
				remainder -= rVal; // subtract rVal from remainder
			}

			// move the results into this
			*this = move(results);
		}

		// return the remainder
		return move(remainder);
	}

	// bitwise AND helper function
	void BigUnsigned::bitAND(const BigUnsigned &rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		auto lIter = begin(*pData); // iterator to beginning of this data

		// loop through until one ends
		for (auto rIter = begin(*rVal.pData); lIter != end(*pData) && rIter != end(*rVal.pData); ++lIter, ++rIter)
			*lIter &= *rIter;

		// erase whats left over if rVal ended first
		pData->erase(lIter, end(*pData));

		// remove possible leading zeros
		removeLeadingZeros();
	}

	// bitwise OR helper function
	void BigUnsigned::bitOR(const BigUnsigned &rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		auto rIter = begin(*rVal.pData); // iterator to beginning of rVal's data

		// loop until one ends
		for (auto lIter = begin(*pData); lIter != end(*pData) && rIter != end(*rVal.pData); ++lIter, ++rIter)
			*lIter |= *rIter;

		// insert whats left over if this ended first
		pData->insert(end(*pData), rIter, end(*rVal.pData));
	}

	// bitwise XOR helper function
	void BigUnsigned::bitXOR(const BigUnsigned &rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		auto rIter = begin(*rVal.pData); // iterator to beginning of rVal's data

		// loop until one ends
		for (auto lIter = begin(*pData); lIter != end(*pData) && rIter != end(*rVal.pData); ++lIter, ++rIter)
			*lIter ^= *rIter;

		// insert whats left over if this ended first
		pData->insert(end(*pData), rIter, end(*rVal.pData));

		// remove possible leading zeros
		removeLeadingZeros();
	}

	// bitshift left helper function
	void BigUnsigned::bitShiftLeft(BigUnsigned rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		// use the function divideWithRemainder so that rVal gets
		// the number of times to add a zero in the front and the
		// remainder is the number of bits to shift left over
		const size_t numOfBits = (size_t)rVal.divideWithRemainder(dataTypeSize);

		// insert zeros to the front of the data
		if (rVal && rVal.pData->size() == 1)
			pData->insert(begin(*pData), rVal.pData->front(), 0);
		else
		{
			// loop until zero
			for (; rVal; --rVal)
				pData->push_front(0);
		}

		// it won't do anything if it's shifted zero bits
		if (numOfBits != 0)
		{
			// the new data to set and the bits carried from the previous data
			dataType newData, carriedBits = 0;

			// loop through the data
			for (auto &data : *pData)
			{
				newData = carriedBits | (data << numOfBits); // get the new data
				carriedBits = data >> (dataTypeSize - numOfBits); // get the bits to carry
				data = newData; // set the new data
			}

			// add one at the end if there are any carried bits
			if (carriedBits != 0)
				pData->push_back(carriedBits);
		}
	}

	// bitshift right helper function
	void BigUnsigned::bitShiftRight(BigUnsigned rVal)
	{
		// get the data ready to be modified
		makeDataUnique();

		// use the function divideWithRemainder so that rVal gets
		// the number of times to remove an item from the front and
		// the remainder is the number of bits to shift left over
		const size_t numOfBits = (size_t)rVal.divideWithRemainder(dataTypeSize);

		// loop until zero or break is called
		for (; rVal; --rVal)
		{
			// make sure it's not empty before trying to remove one
			if (!pData->empty())
				pData->pop_front(); // remove the front item
			else
				break; // exit the loop
		}

		// make sure the data isn't empty and there are bits to be shifted
		if (!pData->empty() && numOfBits != 0)
		{
			// the new data to set and the bits carried from the previous data
			dataType newData, carriedBits = 0;

			// loop through the data backwards
			for (auto iter = rbegin(*pData); iter != rend(*pData); ++iter)
			{
				newData = carriedBits | (*iter >> numOfBits); // get the new data
				carriedBits = *iter << (dataTypeSize - numOfBits); // get the bits to carry
				*iter = newData; // set the new data
			}

			// remove possible leading zeros
			removeLeadingZeros();
		}
	}

	/*
	bool BigUnsigned::isPowOfTwo() const
	{
		// this function returns true only if there is
		// exactly one bit set and the rest are zero

		if (*this) // check if not zero
		{
			// loop through each bit
			for (dataType bit = 1; bit != 0; bit <<= 1)
			{
				if ((pData->back() & bit) != 0) // check if the bit is set
				{
					if ((pData->back() & ~bit) != 0) // check if the bit is the only one set
						return false;
					else
						break;
				}
			}

			// loop through the rest of the data and make sure they are all zero
			for (auto iter = ++rbegin(*pData); iter != rend(*pData); ++iter)
			{
				if (*iter != 0)
					return false;
			}

			return true;
		}
		else
			return false;
	}
	*/

	/// Equal to operator
	bool operator ==(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		if (lhs.pData == rhs.pData)
			return true; // the pointers are the same so the data is the same

		if (lhs.pData && rhs.pData) // check if both pointers point to something
			return *lhs.pData == *rhs.pData;
		else
		{
			// one of the two pointers is nullptr so return true
			// if the other pointer's collection is empty

			if (lhs.pData)
				return lhs.pData->empty();
			else
				return rhs.pData->empty();
		}
	}

	/// Less than operator
	bool operator <(const BigUnsigned &lhs, const BigUnsigned &rhs)
	{
		if (lhs.pData == rhs.pData)
			return false; // the pointers are the same so the data is the same

		if (lhs.pData && rhs.pData) // check if both pointers point to something
		{
			if (lhs.pData->size() != rhs.pData->size()) // check if they are different sizes
				return (lhs.pData->size() < rhs.pData->size()); // return true if the lhs data's size is smaller

			// compare the two and returns true if the lhs data is smaller
			return lexicographical_compare(rbegin(*lhs.pData), rend(*lhs.pData), rbegin(*rhs.pData), rend(*rhs.pData));
		}
		else
		{
			// one of the two pointers is nullptr so only return
			// true if the lhs pointer is nullptr and the rhs
			// pointer's collection is not empty

			if (!lhs.pData)
				return !rhs.pData->empty();
			else
				return false;
		}
	}
} // namespace BigNumber
