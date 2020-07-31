#pragma once
#include "ByteBuffer.hpp"
class MCPacket :public ByteBuffer {
public:

    MCPacket() {

    }
    MCPacket(const char rawdata[], size_t l) {
        write(rawdata, l);
    }
    MCPacket(uint8_t id) {
        writeVarInt(id);
    }

    void send(sf::TcpSocket& socket, bool compr = false) {
        if (compr)writeVarIntBegin(0);
        writeVarIntBegin(getSize());

        socket.send(getPtr(), getSize());
    }
    int readVarInt(sf::TcpSocket& s) {
        int i = 0;
        int j = 0;
        while (true) {
            int k;
            sf::Int8 c = 0;
            size_t recv = 0;
            s.receive(&c, 1, recv);
            k = c;
            i |= (k & 0x7F) << j++ * 7;
            if (j > 5) {
                printf("VarInt too big\n");
            }
            if ((k & 0x80) != 128) break;
        }
        return i;
    }
    void receive(sf::TcpSocket& socket) {
        size_t packetlength = readVarInt(socket);

    }
};