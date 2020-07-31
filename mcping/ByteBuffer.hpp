#pragma once
class ByteBuffer {
public:
    std::vector<char> buf;
    ByteBuffer() {
        clear();
    }
    void writeBegin(const char* ptr, size_t length) {
        buf.insert(buf.begin(), ptr, ptr + length);
    }
    void write(const char* ptr, size_t length) {
        buf.insert(buf.end(), ptr, ptr + length);
    }
    void writeReverse(const char* ptr, size_t length) {
        for (int i = length - 1; i >= 0; i--) {
            buf.push_back(ptr[i]);
        }
        //buf.insert(buf.end(), ptr, ptr + length);
    }
    char* getPtr() {

        return buf.data();
    }
    size_t getSize() {
        return buf.size();
    }
    void clear() {
        buf.clear();
    }
    void print() {
        char* ptr = getPtr();
        for (size_t i = 0; i < getSize(); i++)
        {
            printf("0x%02x,", (uint8_t)((char*)ptr)[i]);
        }
        printf("\n");
    }
    void writeVarInt(uint32_t i) {
        unsigned char b[5];
        size_t idx = 0;
        do
        {
            b[idx] = (i & 0x7f) | ((i > 0x7f) ? 0x80 : 0x00);
            i = i >> 7;

            idx++;
        } while (i > 0);
        write((const char*)b, idx);
    }
    void writeVarIntBegin(uint32_t i) {
        unsigned char b[5];
        size_t idx = 0;
        do
        {
            b[idx] = (i & 0x7f) | ((i > 0x7f) ? 0x80 : 0x00);
            i = i >> 7;

            idx++;
        } while (i > 0);
        writeBegin((const char*)b, idx);
    }
    void writeString(std::string str) {
        writeVarInt(str.size());
        write(str.c_str(), str.size());
    }
    void writeUShort(uint16_t i) {
        //  char c = (&i)[0];
        //  (&i)[0] = (&i)[1];
         // (&i)[1] = c;

        writeReverse((const char*)&i, 2);
    }
    void writeLong(int64_t i) {
        //  char c = (&i)[0];
        //  (&i)[0] = (&i)[1];
         // (&i)[1] = c;

        writeReverse((const char*)&i, 8);
    }
    size_t readpos() {

    }
};