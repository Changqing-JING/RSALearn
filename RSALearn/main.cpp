#include<stdio.h>
#include "UBigInt.h"
#include <string>



/*
(a*b)%m=((a%m)(b%m))%m

m^n%d
*/

UBigInt modular_exponentiation(UBigInt m, UBigInt n, UBigInt d) {

    UBigInt result("1");
    UBigInt power = m % d;
    UBigInt mask("1");
    while(mask<=n) {
        if ((n & mask) != 0) {
            result = (result * power) % d;
        }
        power = (power * power) % d;
        mask = mask << 1;
    }
    return result;

}

void fx() {
    
    UBigInt b2("50000");
    UBigInt b1("1000000");
    
    

    

    UBigInt b3 = b1 % b2;

    return;
}



int main() {

    fx();


    

	return 0;
}