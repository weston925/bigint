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
	/// converts a BigUnsigned to a BigInteger
	BigInteger BigIntegerUtil::toSigned(const BigUnsigned &value)
	{
		BigInteger retVal;
		retVal.data = value;
		return move(retVal);
	}

	/// converts a BigInteger to a BigUnsigned
	BigUnsigned BigIntegerUtil::toUnsigned(const BigInteger &value)
	{
		if (value.negative)
			throw runtime_error("BigIntegerUtil::toUnsigned: cannot convert a negative number to an unsigned data type");

		return value.data;
	}

	/// converts a string to a BigUnsigned
	BigUnsigned BigIntegerUtil::stringToBigUnsigned(string _str, unsigned int base)
	{
		if (base == 1 || base > 36)
			throw out_of_range("BigIntegerUtil::stringToBigUnsigned: invalid base number");

		if (_str.empty())
			throw runtime_error("BigIntegerUtil::stringToBigUnsigned: string recieved is not a valid number");

		switch (_str.front())
		{
		case '-':
			throw runtime_error("BigIntegerUtil::stringToBigUnsigned: cannot convert a negative number to an unsigned data type");
		case '+':
			_str.erase(begin(_str));
			if (_str.empty())
				throw runtime_error("BigIntegerUtil::stringToBigUnsigned: string recieved is not a valid number");
			break;
		default:
			break;
		}

		if ((base == 0 || base == 8 || base == 16) && (_str.front() == '0' && _str.size() > 1))
		{
			_str.erase(begin(_str));

			if (_str.front() == 'x' || _str.front() == 'X')
			{
				_str.erase(begin(_str));

				if (!_str.empty())
				{
					if (base == 0)
						base = 16;
					else if (base == 8)
						throw runtime_error("BigIntegerUtil::stringToBigUnsigned: invalid character found for specified base");
				}
				else
					throw runtime_error("BigIntegerUtil::stringToBigUnsigned: string recieved is not a valid number");
			}
			else if (base == 0)
				base = 8;
		}

		if (base == 0)
			base = 10;

		BigUnsigned data, add(1);

		for (auto iter = rbegin(_str); iter != rend(_str); ++iter)
		{
			if (base <= 10)
			{
				if (isdigit(*iter) && *iter < (char)base + '0')
					data += add * (*iter - '0');
				else
					throw runtime_error("BigIntegerUtil::stringToBigUnsigned: invalid character found for specified base");
			}
			else
			{
				if (isdigit(*iter))
					data += add * (*iter - '0');
				else
				{
					if (islower(*iter))
					{
						if ((*iter - 'a') + 10 < (char)base)
							data += add * ((*iter - 'a') + 10);
						else
							throw runtime_error("BigIntegerUtil::stringToBigUnsigned: invalid character found for specified base");
					}
					else if (isupper(*iter))
					{
						if ((*iter - 'A') + 10 < (char)base)
							data += add * ((*iter - 'A') + 10);
						else
							throw runtime_error("BigIntegerUtil::stringToBigUnsigned: invalid character found for specified base");
					}
					else
						throw runtime_error("BigIntegerUtil::stringToBigUnsigned: invalid character found for specified base");
				}
			}

			add *= base;
		}

		return move(data);
	}

	/// converts a string to a BigInteger
	BigInteger BigIntegerUtil::stringToBigInteger(string _str, unsigned int base)
	{
		if (_str.empty())
			throw runtime_error("BigIntegerUtil::stringToBigUnsigned: string recieved is not a valid number");

		BigInteger retVal; // return value

		if (_str.front() == '-') // check for negative sign
		{
			_str.erase(begin(_str)); // remove negative sign
			retVal.negative = true; // set negative to true
		}

		retVal.data = stringToBigUnsigned(move(_str), base); // get the data

		return move(retVal);
	}

	/// converts a BigUnsigned to a string
	string BigIntegerUtil::bigUnsignedToString(BigUnsigned value, unsigned int base)
	{
		if (base < 2 || base > 36)
			throw out_of_range("BigIntegerUtil::bigUnsignedToString: invalid base number");

		string results;

		if (value)
		{
			unsigned int val;

			while (value > base)
			{
				val = (unsigned int)value.divideWithRemainder(base);

				if (val < 10)
					results.insert(begin(results), (char)val + '0');
				else
					results.insert(begin(results), char(val - 10) + 'a');
			}

			if (value)
			{
				val = (unsigned int)value;

				if (val < 10)
					results.insert(begin(results), (char)val + '0');
				else
					results.insert(begin(results), char(val - 10) + 'a');
			}
		}
		else
			results = "0";

		return move(results);
	}

	/// converts a BigInteger to a string
	string BigIntegerUtil::bigIntegerToString(const BigInteger &value, unsigned int base)
	{
		if (base < 2 || base > 36)
			throw out_of_range("BigIntegerUtil::bigIntegerToString: invalid base number");

		string results = bigUnsignedToString(value.data, base);

		if (value.negative)
			results.insert(begin(results), '-');

		return move(results);
	}

	/// sets a BigUnsigned to zero
	void BigIntegerUtil::clear(BigUnsigned &value)
	{
		if (value)
			value.clearData();
	}

	/// sets a BigInteger to zero
	void BigIntegerUtil::clear(BigInteger &value)
	{
		clear(value.data);
		value.negative = false;
	}

	/// returns the absolute value of a BigInteger
	BigInteger BigIntegerUtil::abs(const BigInteger &value)
	{
		if (value.negative)
			return move(-value);
		else
			return value;
	}

	ostream &operator <<(ostream &os, const BigUnsigned &num)
	{
		if ((os.flags() & ios_base::showpos) != 0)
			os << "+";

		if ((os.flags() & ios_base::hex) != 0)
		{
			if ((os.flags() & ios_base::showbase) != 0)
				os << "0x";

			os << BigIntegerUtil::bigUnsignedToString(num, 16);
		}
		else if ((os.flags() & ios_base::oct) != 0)
		{
			if (num)
			{
				if ((os.flags() & ios_base::showbase) != 0)
					os << "0";

				os << BigIntegerUtil::bigUnsignedToString(num, 8);
			}
			else
				os << "0";
		}
		else
			os << BigIntegerUtil::bigUnsignedToString(num, 10);

		return os;
	}

	ostream &operator <<(ostream &os, const BigInteger &num)
	{
		if (num >= 0 && (os.flags() & ios_base::showpos) != 0)
			os << "+";

		if ((os.flags() & ios_base::hex) != 0)
		{
			if ((os.flags() & ios_base::showbase) != 0)
				os << "0x";

			os << BigIntegerUtil::bigIntegerToString(num, 16);
		}
		else if ((os.flags() & ios_base::oct) != 0)
		{
			if (num)
			{
				if ((os.flags() & ios_base::showbase) != 0)
					os << "0";

				os << BigIntegerUtil::bigIntegerToString(num, 8);
			}
			else
				os << "0";
		}
		else
			os << BigIntegerUtil::bigIntegerToString(num, 10);

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
