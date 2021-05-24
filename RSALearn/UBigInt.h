#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <cmath>
#include <string.h>
#include <utility>

typedef uint32_t batch_t;
#define batchMask 0xFFFFFFFF

#define batchBitLength (sizeof(batch_t)*8)
#define maxNumberBitSize 3*32

class UBigInt {
public:
	UBigInt(const char* str) {
		init(maxNumberBitSize);
		
		

		int strLength = strlen(str);
		int cursor = 0;

		if (str != NULL) {

			int i = strLength - 1;

			while (i >= 0) {
				int substrStart;
				int substrLength;
				if (i>0) {
					substrStart = i - 1;
					substrLength = 2;
				}
				else {
					substrStart = i;
					substrLength = 1;
				}
				char substr[] = { 0, 0, 0 };
				memcpy(substr, str + substrStart, substrLength);
				this->number[cursor] = (uint8_t)strtol(substr, NULL, 16);
				cursor++;
				i -=2;
			}
		}
	}

	UBigInt() {
		init(maxNumberBitSize);
	}

	UBigInt(const UBigInt& num2) {
		init(maxNumberBitSize);
		this->copy(num2);
	}

	UBigInt(UBigInt&& num2) noexcept {

		this->number = num2.number;
		this->length = num2.length;
		num2.number = NULL;
	}

	~UBigInt() {
		int a = 0;
		if (this->number != NULL) {
			free(this->number);
		}
		a = 1;
	}

	//UBigInt(int bitLength) {
	//	init(bitLength);
	//}

	UBigInt operator & (const UBigInt& num2) {
		UBigInt result;

		for (int i = 0; i < num2.length; i += sizeof(batch_t)) {
			batch_t* pnum1 = (batch_t*)(this->number + i);
			batch_t* pnum2 = (batch_t*)(num2.number + i);
			batch_t* pnum3 = (batch_t*)(result.number + i);

			*pnum3 = *pnum1 & *pnum2;

		}

		return result;
	}

	UBigInt& operator = (const UBigInt &num2) {
		
		if (this != &num2) {
			this->copy(num2);
		}
		

		return *this;
	}

	UBigInt operator+ (const UBigInt& val) {
		UBigInt result;

		int loopEnd = this->length - 1;
		batch_t carry = 0;
		for (int i = 0; i <= loopEnd; i+=sizeof(batch_t)) {
			batch_t* num1 = (batch_t*)(this->number + i);
			batch_t* num2 = (batch_t*)(val.number + i);
			batch_t* numRes = (batch_t*)(result.number + i);
			uint64_t numAdd = *((uint64_t*)num1) + *((uint64_t*)num2);

			
			numAdd += carry;
			

			*numRes = numAdd & batchMask;

			carry = numAdd >> 32;
		}

		return result;
	}

	UBigInt operator* (const UBigInt& val) {
		UBigInt result;

		int loopEnd = this->length - 1;
		int lengthProdArray = 2 * this->length / sizeof(batch_t);
		uint64_t* productArray = (uint64_t*)malloc(lengthProdArray * sizeof(uint64_t));

		if (productArray == NULL) {
			printf("out of memory");
			exit(-1);
		}

		for (int i = 0, indexI = 0; i <= loopEnd; i += sizeof(batch_t), indexI++) {
			
			for (int j = 0, indexJ = 0; j <= loopEnd; j += sizeof(batch_t), indexJ++) {
				batch_t* num1 = (batch_t*)(this->number + i);
				batch_t* num2 = (batch_t*)(val.number + j);

				uint64_t* numProd = productArray + indexI + indexJ;

				*numProd = *((uint64_t*)num1) * *((uint64_t*)num2);
			}
		}

		batch_t carry = 0;

		for (int i = 0, indexI = 0; i <= loopEnd; i += sizeof(batch_t), indexI++) {
			batch_t* numRes = (batch_t*)(result.number + i);

			uint64_t* numProd = productArray + indexI;

			uint64_t numProdWithCarry = *numProd + carry;

			*numRes = numProdWithCarry & batchMask;

			carry = numProdWithCarry >> 32;
		}

		free(productArray);
		return result;
	}
	/*
	UBigInt operator -(const UBigInt& val) {
		UBigInt result;

		batch_t borrow = 0;

		int loopEnd = this->length - sizeof(batch_t);

		for (int i = 0; i <=loopEnd ; i += sizeof(batch_t)) {
			batch_t* num1 = (batch_t*)(this->number + i);
			batch_t* num2 = (batch_t*)(val.number + i);
			batch_t* numResult = (batch_t*)(result.number + i);
			int64_t numSub = *num1 - *num2 - borrow;
			
			if (numSub < 0) {
				if (i != loopEnd) {
					borrow = 1;
					numSub += (1LL + batchMask);
				}
				else {
					memset(result.number, 0, result.length);
					break;
				}
			}
			else {
				borrow = 0;
			}

			*numResult = (batch_t)numSub;
		}

		return result;
	}
	*/

	UBigInt operator -( UBigInt& val) {
		UBigInt result;

		if (*this < val) {
			return result;
		}

		this->subStract(this->number, this->length, val.number, val.length, result.number);

		return result;
	}

	UBigInt operator %( UBigInt& val) {
		UBigInt result;

		this->division(val, &result);



		return result;
	}

	bool operator<(const UBigInt& val) {
		char res = this->cmp(val);

		return res == -1;
	}

	bool operator<=(UBigInt& val) {
		char res = this->cmp(val);

		return res == -1 || res == 0;
	}

	bool operator!=(UBigInt& val) {
		char res = this->cmp(val);

		return res != 0;
	}

	bool operator !=(uint32_t val) {
		batch_t* num1 = (batch_t*)this->number;

		if (*num1 != val) {
			return true;
		}
		else {

			for (int i = sizeof(batch_t); i < this->length; i += sizeof(batch_t)) {
				batch_t* num1 = (batch_t*)(this->number+i);
				
				if (*num1 != 0) {
					return true;
				}
			}

			return false;
		}

		
	}

	UBigInt operator << (int step) {
		UBigInt result;
		batch_t carryNumber = 0;

		int mainSetp = step / batchBitLength;
		int bitStep = step % batchBitLength;

		int restBitSetp = batchBitLength - bitStep;

		int loopStart = this->length - (1 + mainSetp) * sizeof(batch_t);

		for (int i = loopStart; i >= 0; i -= sizeof(batch_t)) {
			batch_t* pnum1 = (batch_t*)(this->number + i);

			carryNumber = *pnum1 >> restBitSetp;

			int targetOffset = i + mainSetp*sizeof(batch_t);

			batch_t* pnum_target = (batch_t*)(result.number + targetOffset);

			*pnum_target = *pnum1 << bitStep;

			if (i!=loopStart) {//fixme, performance
				 batch_t* pnum_target_carry = pnum_target + 1;
				*pnum_target_carry |= carryNumber;
			}
		}

		return result;
	}



private:
	void init(int bitLength) {
		this->length = bitLength >>3;
		this->number = (uint8_t*)malloc(this->length);

		if (this->number != NULL) {
			memset(this->number, 0, this->length);
		}else {
			printf("BigInt out of memory\n");
			exit(-1);
		}
	}

private:
	
	uint8_t* number;
	int length;
	
	char cmp(const UBigInt& val) {

		int loopStart = this->length - sizeof(batch_t);

		for (int i = loopStart; i >= 0; i-=sizeof(batch_t)) {
			batch_t* num1 = (batch_t*)(this->number + i);
			batch_t* num2 = (batch_t*)(val.number + i);

			if (*num1 > * num2) {
				return 1;
			}
			else if (*num1 < *num2) {
				return -1;
			}

		}

		
		return 0;
	}

	char cmp(uint8_t* numArray1, int len1, uint8_t* numArray2, int len2) {
		
		if (len1 > len2) {
			return 1;
		}
		else if (len1 < len2) {
			return -1;
		}

		for (int i = len1-1; i >= 0; i--) {
			uint8_t num1 = numArray1[i];
			uint8_t num2 = numArray2[i];

			if (num1 < num2) {
				return -1;
			}
			else if (num1 > num2) {
				return 1;
			}
			
		}
		return 0;
	}

	int validLength() {
		for (int i = this->length - 1; i >= 0; i--) {
			if (this->number[i] != 0) {
				return i + 1;
			}
		}
		return 0;
	}

	UBigInt division( UBigInt& val, UBigInt* remainder) {
		UBigInt result;

		UBigInt div_tmp = *this;
		int validLength1 = this->validLength();
		int validLength2 = val.validLength();
		int deltaLength = validLength1 - validLength2;
		while (deltaLength>=0) {
			memset(remainder->number, 0, remainder->length);
			

			for (int i = validLength1 - 1; i >= deltaLength; i--) {
				remainder->number[i] = val.number[i-deltaLength];
			}

			
			int remainderLength = remainder->validLength();
			int byteDiv = 0;
			while (cmp(div_tmp.number, validLength1, remainder->number, remainderLength)>=0) {
				subStract(div_tmp.number, validLength1, remainder->number, remainderLength, div_tmp.number);
				byteDiv++;
				validLength1 = div_tmp.validLength();
			}
			result.number[deltaLength] = byteDiv;
			
			validLength1 = div_tmp.validLength();
			deltaLength--;
		}

		*remainder = div_tmp;
		return result;
	}

	void subStract(uint8_t* num1, int len1, uint8_t* num2, int len2, uint8_t* result) {

		if (len1 < len2) {
			throw;
		}



		int borrow = 0;
		for (int i = 0; i < len1; i++) {
			int subResult = num1[i] - num2[i] - borrow;
			if (subResult < 0) {
				borrow = 1;
				subResult += 256;
			}
			else {
				borrow = 0;
			}
			result[i] = (uint8_t)subResult;
		}
	}

	void copy(const UBigInt& num2) {
		this->length = num2.length;

		memcpy(this->number, num2.number, num2.length);
	}

};
