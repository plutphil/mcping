#pragma once
void writeVarInt(sf::Packet& s, sf::Uint32 i) {
    unsigned char b[5];
    size_t idx = 0;
    do
    {
        b[idx] = (i & 0x7f) | ((i > 0x7f) ? 0x80 : 0x00);
        i = i >> 7;

        idx++;
    } while (i > 0);
    s.append(b, idx);
    /*

    while (true) {
        if ((i & 0xFFFFFF80) == 0) {
            sf::Uint8 b = i;
            p <<(sf::Uint8) b;
            return;
        }
        p << (sf::Uint8)(i & 0x7F | 0x80);
        i >>= 7;
    }
    */
}
void printPacket(sf::Packet p) {
    for (size_t i = 0; i < p.getDataSize(); i++)
    {
        printf("%02x.", (uint8_t)((char*)p.getData())[i]);
    }
    printf("\n");
}
void printData(char* ptr, size_t l) {
    for (size_t i = 0; i < l; i++)
    {
        printf("%02x", (uint8_t)(ptr)[i]);
    }
    printf("\n");
}