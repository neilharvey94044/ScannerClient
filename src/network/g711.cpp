#include <string>
#include <span>
#include "network/g711.h"

namespace sc {

/*
  ==========================================================================

   FUNCTION NAME: itu_expand

   DESCRIPTION: Mu law decoding rule according ITU-T Rec. G.711.


   PROTOTYPE: size_t ulaw_expand(std::string logbuf, std::span<short> linbuf)

   PARAMETERS:
     logbuf:	(In)  std::string with compressed samples (8 bit right justified,
                      without sign extension)
     linbuf:	(Out) buffer with linear samples (14 bits left justified in 16 bit short)

   RETURN VALUE: number of samples

   HISTORY:
   10.Dec.91	1.0	Separated mu law expansion function
   09.Jan.23    Modernized to C++ (2023)
  ============================================================================
*/
 
size_t ulaw_expand (const std::string logbuf, std::span<unsigned short> linbuf) {

    auto it(linbuf.begin());      // the output buffer iterator

    for(unsigned char c: logbuf){
        *it++ = itu_expand(c);
    }
    return logbuf.length();
}

// computed segment
unsigned short itu_expand (unsigned char log_in) {
  unsigned short segment;                 // segment (Table 2/G711, column 1)
  unsigned mantissa;                      // low nibble of log companded sample
  unsigned exponent;                      // high nibble of log companded sample
  short sign;                             // sign of output sample
  unsigned short step;

    sign = log_in < (0x0080)               // sign-bit = 1 for positiv values
      ? -1 : 1;
    mantissa = ~log_in;                    // 1's complement of input value
    exponent = (mantissa >> 4) & (0x0007);      // extract exponent
    segment = exponent + 1;               // compute segment number
    mantissa = mantissa & (0x000F);       // extract mantissa

    // Compute Quantized Sample (14 bit left justified!)
    step = (4) << segment;                // position of the LSB
    // = 1 quantization step)
    unsigned short lin_out = sign *        // sign
      (((0x0080) << exponent)             // '1', preceding the mantissa
       +step * mantissa                   // left shift of mantissa
       + step / 2                         // 1/2 quantization step
       - 4 * 33);

    return lin_out;
}


} //namespace