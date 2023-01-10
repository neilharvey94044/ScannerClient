// Copyright (c) Neil D. Harvey

#include <string>
#include <span>
#include "network/g711.h"

namespace sc {

/*
  ==========================================================================

   FUNCTION NAME: ulaw_expand

   DESCRIPTION: Mu law decoding rule according ITU-T Rec. G.711.


   PROTOTYPE: size_t ulaw_expand(std::string logbuf, std::span<short> linbuf)

   PARAMETERS:
     logbuf:	(In)  std::string with compressed samples (8 bit right justified,
                      without sign extension)
     linbuf:	(Out) buffer with linear samples (14 bits left justified in 16 bit short)

   RETURN VALUE: number of samples

   HISTORY:
   10.Dec.91	1.0	Separated mu law expansion function
   09.Jan.23    Modernized to C++ by Neil D. Harvey (2023)
  ============================================================================
*/
/* 
size_t ulaw_expand (const std::string logbuf, std::span<short> linbuf) {
  long n;                       // aux.var. 
  short segment;                // segment (Table 2/G711, column 1)
  short mantissa;               // low nibble of log companded sample
  short exponent;               // high nibble of log companded sample
  short sign;                   // sign of output sample
  short step;
  auto it(linbuf.begin());      // the output buffer iterator

  for (short c: logbuf) {
    sign = c < (0x0080)         // sign-bit = 1 for positiv values
      ? -1 : 1;
    mantissa = ~c;              // 1's complement of input value
    exponent = (mantissa >> 4) & (0x0007);      /* extract exponent
    segment = exponent + 1;     // compute segment number
    mantissa = mantissa & (0x000F);     /* extract mantissa

    /* Compute Quantized Sample (14 bit left justified!)
    step = (4) << segment;      // position of the LSB

    // = 1 quantization step) 
    *it++ = sign *              // sign
      (((0x0080) << exponent)   /* '1', preceding the mantissa
       +step * mantissa         /* left shift of mantissa
       + step / 2               /* 1/2 quantization step
       - 4 * 33);
  }
  return logbuf.length();
} */

 
size_t ulaw_expand (const std::string logbuf, std::span<unsigned short> linbuf) {

    auto it(linbuf.begin());      // the output buffer iterator

    for(unsigned char c: logbuf){
        *it++ = linear16FromuLaw(c);
    }
    return logbuf.length();
}

unsigned short linear16FromuLaw(unsigned char uLawByte) {
  static int const exp_lut[8] = {0,132,396,924,1980,4092,8316,16764};
  uLawByte = ~uLawByte;

  bool sign = (uLawByte & 0x80) != 0;
  unsigned char exponent = (uLawByte>>4) & 0x07;
  unsigned char mantissa = uLawByte & 0x0F;

  unsigned short result = exp_lut[exponent] + (mantissa << (exponent+3));
  if (sign) result = -result;
  return result;
}

} //namespace