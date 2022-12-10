// Copyright (c) Neil D. Harvey



#include <string>
#include <memory>

namespace sc {


class UDPSocket {
    public:
        static std::unique_ptr<UDPSocket> socket(std::string scanner_ip, int scanner_port);
        virtual int sendto(std::string) = 0;
        virtual std::string recvfrom() = 0;
        virtual ~UDPSocket(){};
    
    protected:
        UDPSocket() = default;
};

} // sc