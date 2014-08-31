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

#include "BigIntegerUtil.h"

using namespace std;

namespace BigNumber
{
	// converts a BigUnsigned to a BigInteger
	BigInteger BigIntegerUtil::toSigned(const BigUnsigned &value)
	{
		BigInteger retVal;
		retVal.data = value;
		return move(retVal);
	}

	// converts a BigInteger to a BigUnsigned
	BigUnsigned BigIntegerUtil::toUnsigned(const BigInteger &value)
	{
		if (value.negative)
			runtime_error("BigIntegerUtil::toUnsigned: cannot convert a negative value to an unsigned data type");

		return value.data;
	}

	// converts a string to a BigUnsigned
	BigUnsigned BigIntegerUtil::stringToBigUnsigned(string value)
	{
		BigUnsigned retVal;

		if (value.empty())
			return retVal;

		if (value.front() == '-')
			runtime_error("BigIntegerUtil::stringToBigUnsigned: cannot convert a negative value to unsigned data type");

		bool isNumber = true;

		for (auto iter = begin(value); iter != end(value); ++iter)
		{
			if (!isdigit(*iter))
			{
				isNumber = false;
				break;
			}
		}

		if (!isNumber)
			runtime_error("BigIntegerUtil::stringToBigUnsigned: string recieved is not a number");

		BigUnsigned::dataType data = 0;
		size_t bitsSet = 0;
		while (value != "0")
		{
			if (bitsSet == BigUnsigned::dataTypeSize)
			{
				retVal.pData->push_back(data);
				bitsSet = 0;
				data = 0;
			}

			data >>= 1;
			if (strHalf(value))
				data |= 1 << (BigUnsigned::dataTypeSize - 1);

			++bitsSet;
		}

		if (bitsSet != 0)
		{
			data >>= BigUnsigned::dataTypeSize - bitsSet;
			retVal.pData->push_back(data);
		}

		return move(retVal);
	}

	// converts a string to a BigInteger
	BigInteger BigIntegerUtil::stringToBigInteger(string value)
	{
		if (value.empty())
			runtime_error("BigIntegerUtil::stringToBigUnsigned: cannot get BigUnsigned from an empty string");

		BigInteger retVal;

		if (value.front() == '-')
		{
			value.erase(begin(value));
			retVal.negative = true;
		}

		retVal.data = stringToBigUnsigned(value);

		return move(retVal);
	}

	// converts a BigUnsigned to a string
	string BigIntegerUtil::bigUnsignedToString(const BigUnsigned &value, ios_base::fmtflags flags)
	{
		string results;

		if (value)
		{
			if (value.pData->size() == 1)
			{
				stringstream ss;

				ss.flags(flags);

				ss << value.pData->front();

				results = ss.str();
			}
			else
			{
				if ((flags & ios_base::hex) != 0)
				{
					results = move(convertToHex(value, (flags & ios_base::uppercase) != 0));

					if ((flags & ios_base::showbase) != 0)
						results.insert(0, "0x");
				}
				else if ((flags & ios_base::oct) != 0)
				{
					results = move(convertToOct(value));

					if ((flags & ios_base::showbase) != 0)
						results.insert(begin(results), '0');
				}
				else
					results = move(convertToDec(value));
			}
		}
		else
		{
			results = "0";

			if ((flags & (ios_base::hex | ios_base::showbase)) != 0)
				results.insert(0, "0x");

		}

		if ((flags & ios_base::showpos) != 0)
			results.insert(begin(results), '+');

		return move(results);
	}

	// converts a BigInteger to a string
	string BigIntegerUtil::bigIntegerToString(const BigInteger &value, ios_base::fmtflags flags)
	{
		string results = bigUnsignedToString(value.data, flags);

		if (value.negative)
		{
			if (flags & ios_base::showpos)
				results.front() = '-';
			else
				results.insert(begin(results), '-');
		}

		return move(results);
	}

	// sets a BigUnsigned to zero
	void BigIntegerUtil::clear(BigUnsigned &value)
	{
		if (value)
			value.clearData();
	}

	// sets a BigInteger to zero
	void BigIntegerUtil::clear(BigInteger &value)
	{
		clear(value.data);
		value.negative = false;
	}

	// returns the absolute value of a BigInteger
	BigInteger BigIntegerUtil::abs(const BigInteger &value)
	{
		if (value.negative)
			return move(-value);
		else
			return value;
	}

	string BigIntegerUtil::convertToOct(const BigUnsigned &value)
	{
		string results;
		BigUnsigned::dataType bitsToGet;
		char cVal = 0;
		size_t offset = (BigUnsigned::dataTypeSize - 3) + ((value.pData->size() * sizeof(BigUnsigned::dataType)) % 3);

		for (auto iter = rbegin(*value.pData); iter != rend(*value.pData); ++iter)
		{
			bitsToGet = (BigUnsigned::dataType)7 << offset;
			results.push_back((cVal | (char((*iter & bitsToGet) >> offset))) + '0');
			offset -= 3;
			bitsToGet = (BigUnsigned::dataType)7 << offset;

			for (; bitsToGet >= 7; bitsToGet >>= 3, offset -= 3)
				results.push_back(char((*iter & bitsToGet) >> offset) + '0');

			offset += 3;

			cVal = char((*iter & bitsToGet) << ((3 - offset) % 3));
			offset += BigUnsigned::dataTypeSize - 3;
		}

		while (results.front() == '0')
			results.erase(begin(results));

		return move(results);
	}

	string BigIntegerUtil::convertToDec(const BigUnsigned &value)
	{
		string results = "0";

		string add = "1";
		BigUnsigned::dataType bit;

		for (auto item : *value.pData)
		{
			bit = 1;
			for (size_t i = 0; i < BigUnsigned::dataTypeSize; ++i)
			{
				if ((item & bit) != 0)
					strAdd(results, add);

				strDouble(add);
				bit <<= 1;
			}
		}

		return move(results);
	}

	string BigIntegerUtil::convertToHex(const BigUnsigned &value, bool uppercase)
	{
		string results;
		BigUnsigned::dataType bitsToGet;
		size_t val;

		for (auto iter = rbegin(*value.pData); iter != rend(*value.pData); ++iter)
		{
			bitsToGet = (BigUnsigned::dataType)0xF << (BigUnsigned::dataTypeSize - 4);

			for (size_t i = 2 * sizeof(BigUnsigned::dataType); i > 0; --i)
			{
				val = (*iter & bitsToGet) >> ((i - 1) * 4);
				if (val < 10)
					results.push_back((char)val + '0');
				else
					results.push_back(char(val - 10) + (uppercase ? 'A' : 'a'));
				bitsToGet >>= 4;
			}
		}

		while (results.front() == '0')
			results.erase(begin(results));

		return move(results);
	}

	bool BigIntegerUtil::strHalf(string &_str)
	{
		if (!_str.empty())
		{
			size_t val = _str.front() - '0';
			bool remainder = val % 2 == 1;
			string halfVal;

			if (val != 0)
				halfVal = to_string(val / 2);

			for (auto iter = ++begin(_str); iter != end(_str); ++iter)
			{
				val = *iter - '0';
				if (remainder)
					val += 10;
				remainder = val % 2 == 1;
				halfVal.append(to_string(val / 2));
			}

			_str = halfVal;
			return remainder;
		}
		else
		{
			_str = "0";
			return false;
		}
	}

	void BigIntegerUtil::strAdd(string &_str, const string &_add)
	{
		auto strIter = rbegin(_str);
		auto addIter = rbegin(_add);
		size_t val;
		size_t carry = 0;

		for (; strIter != rend(_str) && addIter != rend(_add); ++strIter, ++addIter)
		{
			val = (*strIter - '0') + (*addIter - '0') + carry;
			carry = val / 10;
			*strIter = (val % 10) + '0';
		}

		for (; strIter != rend(_str); ++strIter)
		{
			if (carry != 0)
			{
				val = (*strIter - '0') + carry;
				carry = val / 10;
				*strIter = (val % 10) + '0';
			}
			else
				break;
		}

		for (; addIter != rend(_add); ++addIter)
		{
			if (carry != 0)
			{
				val = (*addIter - '0') + carry;
				carry = val / 10;
				_str.insert(begin(_str), (val % 10) + '0');
			}
			else
				_str.insert(begin(_str), *addIter);
		}

		if (carry != 0)
			_str.insert(begin(_str), carry + '0');
	}

	void BigIntegerUtil::strDouble(string &_str)
	{
		size_t val;
		size_t carry = 0;

		for (auto iter = rbegin(_str); iter != rend(_str); ++iter)
		{
			val = (*iter - '0') * 2 + carry;
			carry = val / 10;
			*iter = (val % 10) + '0';
		}

		if (carry != 0)
			_str.insert(begin(_str), carry + '0');
	}

	ostream &operator <<(ostream &os, const BigUnsigned &num)
	{
		os << BigIntegerUtil::bigUnsignedToString(num, os.flags());
		return os;
	}

	ostream &operator <<(ostream &os, const BigInteger &num)
	{
		os << BigIntegerUtil::bigIntegerToString(num, os.flags());
		return os;
	}

	istream &operator >>(istream &is, BigUnsigned &num)
	{
		string text;

		is >> text;

		num = BigIntegerUtil::stringToBigUnsigned(text);

		return is;
	}

	istream &operator >>(istream &is, BigInteger &num)
	{
		string text;

		is >> text;

		num = BigIntegerUtil::stringToBigInteger(text);

		return is;
	}
} // namespace BigNumber
