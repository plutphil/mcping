#pragma once
#include "Response.hpp"
#include "MCPacket.hpp"
#include <zlib.h>
class MCProtocol {
public:
    sf::TcpSocket* socket=NULL;
    MCProtocol() {

    }
    MCProtocol(sf::TcpSocket& socket) {
        this->socket=&socket;
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
                return 0;
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
    char* readBytes(sf::TcpSocket& s,size_t len) {
        
        char* str = new char[len];
        
        size_t l = 1;
        size_t pos = 0;
        while (pos < l) {
            (*socket).receive(str + pos, len, l);
            //printf("received %ld\n", l);
            pos += l;
        }
        return str;
    }
    void sendHandshake(sf::TcpSocket& socket, int next = 1, int protocolVersion = 47, std::string host = "localhost", int port = 25565) {
        MCPacket bbmctest = MCPacket();
        //bbmctest.writeVarInt(15);
        bbmctest.writeVarInt(0);
        bbmctest.writeVarInt(protocolVersion);
        bbmctest.writeString(host);
        bbmctest.writeUShort(port);
        bbmctest.writeVarInt(next);
        bbmctest.send(socket);
        
        //bbmctest.print();
    }
    
    void receiveHandshake() {
        char data2[] = { 0x01,0x00 };
        MCPacket(0).send(*socket);
        int packsize = readVarInt(*socket);
        printf("packet size : %i\n", packsize);
        int packid = readVarInt(*socket);
        printf("packet id : %02x\n", packid);

        //printf("%s\n", readString().c_str());
    }
    void ping() {
        MCPacket bbmctest = MCPacket();
        bbmctest.writeVarInt(1);
        long a = (long)time(0);
        printf("a: %ld\n", a);
        bbmctest.writeLong(a);
        bbmctest.send(*socket);

        size_t packsize = readVarInt(*socket);
        printf("packet size : %i\n", packsize);

        /*if (readVarInt(*socket) == 0x01) {
            long b = readLong(*socket);
            printData((char*)&b, 8);
            printf("b: %ld\n", b);
            //printf("sizeof long %i", sizeof(long));
            long cl = (long)time(0);
            printf("c: %ld\n", cl);
            printf("ping is %ld ms\n", (cl - a));
        }*/
    }

    void login(string name= "1234567890123456") {
        sendHandshake(*socket, 2);
        MCPacket pack(0x00);
        pack.writeString(name);//player name
        pack.print();
        pack.send(*socket);
        
        /*
        */
        return;
    }
    bool compression = false;
    bool loginsucces = false;
    enum ConnectionState{PLAY};
    char* c = NULL;
    int compression_threshold = -1;
    int packsize = 0;
    vector<string> chatmesgs;
    int pingttime = 0;
    void parseuncompressed(Response& response) {
        response.readId();
        
        if (response.id == 0x00) {
            MCPacket pack(0x00);
            int i = response.readVarInt();
            pack.writeVarInt(i);
            pack.send(*socket, compression);
            pingttime = i;
        }
        if (loginsucces) {
            //printf("id %02x size %02x\n", (unsigned int)response.id, (unsigned int)packsize);
            if (response.id == 0x02) {
                
                string chat = response.readString();
                printf("chat %i %s\n", response.read(1)[0], chat.c_str());
                chatmesgs.push_back(chat);
            }else if (response.id > 0x49) {
                printf("unknown packet id %i\n", response.id);
                exit(-1);
            }
            else {
                //response.parse();
            }
        }
        else {
            if (response.id == 0x02) {
                printf("login succes\n");
                string uuid = response.readString();
                string username = response.readString();
                printf("uuid %s\n", uuid.c_str());
                printf("username %s\n", username.c_str());
                loginsucces = true;
            }else
            if (response.id == 0x03) {
               
                    if (!compression) {
                        compression_threshold = response.readVarInt();
                        printf("compression threshold : %i\n", compression_threshold);
                        compression = true;
                        return;
                    }
            }
            
        }
    }
    int readId() {
        
        Response response( c, packsize);
        if (compression) {
            //IMPORTANT 
            /*
            response.readVarInt() is before if-block because once compression is set, 
            additional to packet size the uncompressed size is sent as a VarInt.
            If the uncompressed size is 0 the WHOLE packet is UNCOMPRESSED.
            When the uncompressed size over 0 the packet is COMPRESSED.
            */
            int uncompressedpacksize = response.readVarInt();
            if (uncompressedpacksize!=0) {
                unsigned char* rawbytes = (unsigned char*)malloc(uncompressedpacksize);
                unsigned long outlen;
                if (uncompress((Bytef*)rawbytes, &outlen, (const Bytef*)&response.buffer[response.readpos],response.size-response.readpos ) != Z_OK)
                {
                    printf("Could not uncompress .\n");
                }
                else {
                    if (outlen) {
                        //printf("outlen %i uncsze %i\n", outlen, uncompressedpacksize);
                    }
                }
                Response uncompressed((char*)rawbytes, uncompressedpacksize);
                parseuncompressed(uncompressed);
                delete[] uncompressed.buffer;
                return 2;
            }
        }
        parseuncompressed(response);
            
            
        
        
        
        
        /*if (response.id == 0x03) {
            if (!loginsucces)
            if (!compression) {
                compression_threshold = response.readVarInt();
                printf("compression threshold : %i\n", compression_threshold);
                compression = true;
                return 3;
            }
        }
        
        if (response.id == 0x00) {
            MCPacket pack(0x00);
            int i=response.readVarInt();
            pack.writeVarInt(i);
            pack.send(*socket,compression);
        }
       
        if(response.id == 0x02) {
            if (loginsucces) {
                string chat = response.readString();
                printf("chat %i %s\n", response.read(1)[0], chat.c_str());
                chatmesgs.push_back(chat);
            }
            else {
                printf("login succes\n");
                string uuid = response.readString();
                string username = response.readString();
                printf("uuid %s\n", uuid.c_str());
                printf("username %s\n", username.c_str());
                loginsucces = true;
            }

        }else if(response.id == 0x06) {
            MCPacket pack(0x16);
            pack.writeVarInt(0);
            pack.send(*socket, compression);
        }
        
        
        if (response.id > 0x49) {
            printf("unknown packet id %i\n", response.id);
            exit(-1);
        }
        else
             {
            response.parse();
        }*/
    }
    int pcount = 0;
    int readPacket() {
        int maxread = 100;
        int pr = 0;
        while (pr < maxread) {
            packsize = readVarInt(*socket);
            //printf("packet size : %i\n", packsize);

            /*if (compression) {
                int uncompressedpacksize = readVarInt(*socket);
                printf("uncompressed packet size : %i\n", uncompressedpacksize);
                if (packsize > compression_threshold) {
                    return 2;
                }
            }*/
            //pcount++;

            c = readBytes(*socket, packsize);
            //printf("%i size %i\n", pcount,  packsize);
            //printData(c, packsize);
            readId();

            delete[] c;
            pr++;
        }
        /*
        if (compression) {
            if (packsize > 256) {
                int uncompressedsize = readVarInt(*socket);
                printf("uncompressed size : %i\n", uncompressedsize);

                c = readBytes(*socket, packsize-1);
                //printData(c, packsize);
                delete[] c;
                //socket->receive(NULL,packsize)
                //c = NULL;
                return 0;
            }
        }
        int packid = readVarInt(*socket);
        printf("packet id : %02x\n", packid);

        if (packid == 0x03) {
            int threshold = readVarInt(*socket);
            printf("compression threshold : %i\n", threshold);
            compression = true;
            return 3;
        }

        if (packid > 0x49) {
            printf("unknown packet id!\n");
            //exit(-1);
        }
        packsize -= 1;
        char* c = readBytes(*socket, packsize);
        printData(c, packsize);
        Response response(packid, c, packsize);

        delete[] c;
        /*switch (packid)
        {
        case 0x03: {
            int threshold = readVarInt(*socket);
            printf("compression threshold : %i\n", threshold);
            compression = true;
            break; }
        case 0xff: {
            
            std::string serverid = readString();
            printf("server id:%s\n", serverid.c_str());
            int keyl = readVarInt(*socket);
            printData(readBytes(*socket, keyl), keyl);
            int tokl = readVarInt(*socket);
            printData(readBytes(*socket, tokl), tokl);
            break;
        }
        case 0x01: {
            printf("entity id %i\n", readInt(*socket));
            printf("gamemode %i\n", readByte(*socket));
            printf("difficulty %i\n", readByte(*socket));
            printf("max players %i\n", readByte(*socket));
            printf("level type %s\n", readString().c_str());
            printf("reduced debug %i\n", readByte(*socket));

        }
        case 0x02: {
            if (loginsucces) {
                string chat = readString();
                printf("chat %i %s\n", readByte(*socket) , chat.c_str());
            }else{
                printf("login succes\n");
                string uuid = readString();
                string username = readString();
                printf("uuid %s\n", uuid.c_str());
                printf("username %s\n", username.c_str());
                loginsucces = true;
            }
            
            break;
        }
        default:
            if (packsize - 2 > 0) {
                char* c = readBytes(*socket, packsize);
                printData(c, packsize);
            }
            break;
        }*/
        return 1;
    }
   
};