/*

uint64_t a = 0xFFFFFFFF;
uint64_t b = 0xFFFFFFFF;

uint64_t c = a * b;

00831393  mov         eax,dword ptr [a]        eax= a_low
00831396  mov         ecx,dword ptr [esi+44h]   ecx = a_high
00831399  mov         edx,dword ptr [b]            edx = b_low
0083139C  mov         edi,dword ptr [esi+3Ch]     edi = b_high
0083139F  mov         ebx,eax                       ebx = eax= a_low
008313A1  imul        ebx,edi                       ebx = ebx * edi = a_low * b_high
008313A4  mov         dword ptr [esi+14h],edx        esi14 = edx = b_low
008313A7  mul         eax,edx                        eax = eax * edx = a_low * b_low
008313A9  add         edx,ebx                         edx = edx + ebx = b_low + a_low * b_high
008313AB  mov         edi,dword ptr [esi+14h]          edi = esi14 = b_low
008313AE  imul        ecx,edi                          ecx = ecx * edi = a_high * b_low
008313B1  add         edx,ecx                          edx = edx + ecx  = b_low + a_low * b_high + a_high * b_low
008313B3  mov         dword ptr [c],eax               c_low = eax = a_low * b_low
008313B6  mov         dword ptr [esi+34h],edx         c_high = edx = b_low + a_low * b_high + a_high * b_low

*/

/*




*/