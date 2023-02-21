
#pragma once

#include <span>
#include <string>
 
namespace sc {
size_t ulaw_expand (const std::string logbuf, std::span<unsigned short> linbuf);
unsigned short itu_expand(unsigned char login);


} // namespace