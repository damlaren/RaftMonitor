#include <vector>
#include <stdint.h>

class Parser {
    public:
        virtual int packetType(std::vector<uint8_t>);
};