
#include <string>
#include <chrono>
#include <SFML/Network.hpp>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <streambuf>
#include <istream>
#include <sstream>
#include <vector>
#include <ctime>
//#include "CByteBuffer.h"
using namespace std::chrono;

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
int readVarInt(sf::TcpSocket& s) {
    int i = 0;
    int j = 0;
    while (true) {
        int k;
        sf::Int8 c = 0;
        size_t recv = 0;
        s.receive(&c, 1,recv);
        k = c;
        i |= (k & 0x7F) << j++ * 7;
        if (j > 5) {
            printf("VarInt too big\n");
        }
        if ((k & 0x80) != 128) break;
    }
    return i;
}
uint8_t readByte(sf::TcpSocket& s) {
    int8_t out;
    size_t recv = 0;
    s.receive(&out, 1, recv);
    return out;
}
void rotate(char* ptr, size_t l) {
    char c = 0;
    for (size_t i = 0; i < (l/2); i++)
    {
        c = ptr[i];
        ptr[i] = ptr[l-i-1];
        ptr[l-i-1] = c;
    }
}
void printData(char* ptr, size_t l) {
    for (size_t i = 0; i < l; i++)
    {
        printf("%02x", (uint8_t)(ptr)[i]);
    }
    printf("\n");
}
int64_t readLong(sf::TcpSocket& s) {
    int64_t out=1;
    size_t recv = 0;
    s.receive(&out, 8, recv);
    printData((char*)&out, 8);
    rotate((char*)&out, 8);
    printData((char*)&out, 8);
    return out;
}
void printPacket(sf::Packet p) {
    for (size_t i = 0; i < p.getDataSize(); i++)
    {
        printf("%02x.", (uint8_t)((char*)p.getData())[i]);
    }
    printf("\n");
} 

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
        buf.insert(buf.end(),ptr, ptr+length);
    }
    void writeReverse(const char* ptr, size_t length) {
        for (int i = length-1; i >= 0 ; i--){
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
class MCPacket:public ByteBuffer {
public:
    void send(sf::TcpSocket& socket) {
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
void sendHandshake(sf::TcpSocket& socket,int next=1,int protocolVersion = 47, std::string host="localhost", int port=25565) {
    MCPacket bbmctest = MCPacket();
    //bbmctest.writeVarInt(15);
    bbmctest.writeVarInt(0);
    bbmctest.writeVarInt(47);
    bbmctest.writeString(host);
    bbmctest.writeUShort(port);
    bbmctest.writeVarInt(next);
    bbmctest.send(socket);
    //bbmctest.print();
}
int main(char** argv,int args) {
    sf::TcpSocket socket;
    std::string host = "gommehd.net";
    short port = 25565;
    if (socket.connect(host.c_str(), port) != sf::Socket::Done){
    }
    /*
    sf::Packet packet;
   
    packet.clear();
    packet << (sf::Uint8)0x0f;

    packet << (sf::Uint8)0x00;//packet id for handshake
    
    writeVarInt(packet, 47);//protocol version
    writeVarInt(packet, host.size());//hostname string length
    
    packet.append(host.c_str(),host.size());//hostname string
    printPacket(packet);
    packet << (sf::Int16)port;//host port
    printPacket(packet);
    writeVarInt(packet, 1);// next state 
    writeVarInt(packet, packet.getDataSize()-1);//packet size
    
    printPacket(packet);
    socket.send(packet);

    packet.clear();
    packet << (uint8_t)0x01;//packet size
    packet << (uint8_t)0x00;//packet id
    printPacket(packet);
    socket.send(packet);
    
    int packsize = readVarInt(socket);
    printf("packet size : %i\n",packsize);
    int packid = readVarInt(socket);
    printf("packet id : %i\n",packid);
    size_t jsonlength = readVarInt(socket);
    printf("json size : %i\n",jsonlength);

    char* str = new char[jsonlength+1];
    str[jsonlength] = '\0';
    size_t l = 0;
    socket.receive(str, jsonlength, l);
    printf("%i\n%s\n",jsonlength, str);
    

    //packet << (uint8_t)0x01;//packet size
    packet << (uint8_t)0x01;//packet id
    milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );
    packet << (sf::Int64)ms.count();
    socket.send(packet);
     */
    
    char data[] = { 0x0f,0x00,0x2f,0x09,0x6c,0x6f,0x63,0x61,0x6c,0x68,0x6f,0x73,0x74,0x63,0xdd,0x01 };
    
    ByteBuffer bb=ByteBuffer();
    bb.write(data, sizeof(data));
    bb.print();
    sendHandshake(socket);
    //socket.send(data, sizeof(data));
    //socket.send(bbmctest.getPtr(), bbmctest.getSize());
    char data2[] = { 0x01,0x00 };
    socket.send(data2, sizeof(data2));



    int packsize = readVarInt(socket);
    printf("packet size : %i\n", packsize);
    int packid = readVarInt(socket);
    printf("packet id : %i\n", packid);
    size_t jsonlength = readVarInt(socket);
    printf("json size : %i\n", jsonlength);

    char* str = new char[jsonlength + 1];
    str[jsonlength] = '\0';
    size_t l = 1;
    size_t pos=0;
    while (pos < jsonlength) {
        socket.receive(str+pos, jsonlength, l);
        printf("received %ld\n", l);
        pos += l;
    }
   
    
    printf("%i\n%s\n", jsonlength, str);


    for (size_t times = 0; times < 10; times++)
    {

    

    MCPacket bbmctest = MCPacket();
    bbmctest.writeVarInt(1);
    long a = (long)time(0);
    printf("a: %ld\n", a);
    bbmctest.writeLong(a);
    bbmctest.send(socket);
        


    packsize = readVarInt(socket);
   
    printf("packet size : %i\n", packsize);
    
    if (readVarInt(socket)==0x01) {
        long b=readLong(socket);
        printData((char*)&b, 8);
        printf("b: %ld\n", b);
        //printf("sizeof long %i", sizeof(long));
        long cl = (long)time(0);
        printf("c: %ld\n", cl);
        printf("ping is %ld ms\n", (cl - a));
    }
    sf::sleep(sf::seconds(1));
    }
    char c = 0;
    
    size_t received = 0;
    //socket.setBlocking(false);
   
    while (socket.receive(&c, 1, received) == sf::Socket::Done) {
        //if (isascii(c))printf("%c", c);
        //printf("%02x\n", (uint8_t)c);
    }
}