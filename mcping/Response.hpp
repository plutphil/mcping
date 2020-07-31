#pragma once
class Response {
public:
    int id = -1;
    int size = -1;
    bool compressed = false;
    char* buffer;
    Response() {

    }
    ~Response() {

    }
    Response(char* ptr, int size) {
        buffer = ptr;
        this->size = size;


    }
    void readId() {
        this->id = readVarInt();
        //printf("response id %i size %i\n", this->id, size);
    }
    void parse() {

        /*if (id == 0x00) {
            int entityid = readVarInt();
            char* uuid = read(16);
            int type = readVarInt();
            char* xyz = read(24);
            char* pitch = read(1);
            char* yaw = read(1);
            char* data = read(4);
            char* vxyz = read(24);

        }*/
        if (id == 0x01) {
            printf("entity id %i\n", readInt());
            printf("gamemode %i\n", readByte());
            printf("dimension %i\n", readByte());
            printf("difficulty %i\n", readByte());
            printf("max players %i\n", readByte());
            printf("level type %s\n", readString().c_str());
            printf("reduced debug %i\n", readByte());
        }
        else if (id == 3 || id == 4 || id==7 || id == 8 || id == 14 || id == 15 ||id == 18 || id == 22 || id == 23 || id == 24 || id == 25 || id == 28|| id == 41 || id == 57 ) {
            /*
            7 'Respawn'
            8 'Player Position And Look'
            15 'Spawn Mob'
            18 'Entity Velocity'
            22 Entity Look
            23 'Entity Look And Relative Move'
            24 'Entity Teleport'
            25 'Entity Head Look'
            57 'Player Abilities'
            
            */
        }
        else if (id == 0x38) {
            printf("---Player List Item---\n");
            int action = readVarInt();
            printf("action %i\n", action);
            int npls = readVarInt();
            printf("number players %i\n", npls);
            for (size_t i = 0; i < npls; i++)
            {
                if (action == 0) {
                    printf("uuid %s\n", readUUID().c_str());
                    printf("  name %s\n", readString().c_str());
                    int nprops = readVarInt();
                    printf("  number props %i\n", nprops);
                    for (size_t x = 0; x < nprops; x++)
                    {
                        printf("    prop name %s\n", readString().c_str());
                        printf("    prop val %s\n", readString().c_str());
                        bool issigned = readByte();
                        printf("    prop issigned %i\n", issigned);
                        if (issigned) {
                            printf("    prop signature %s\n", readString().c_str());
                        }
                    }

                    printf("  gamemode %i\n", readVarInt());
                    printf("  ping %i\n", readVarInt());
                    bool hasdpname = readByte();
                    printf("  has display name %i\n", hasdpname);
                    if (hasdpname) {
                        printf("  display name %s\n", readString().c_str());
                    }
                }
                else if (action == 1) {
                    printf("  gamemode %i\n", readVarInt());
                }
                else if (action == 2) {
                    printf("  ping %i\n", readVarInt());
                }
                else if (action == 3) {
                    bool hasdpname = readByte();
                    printf("  has display name %i\n", hasdpname);
                    if (hasdpname) {
                        printf("  display name %s\n", readString().c_str());
                    }
                }
                else if (action == 5) {
                    printf("  removed \n");
                }
            }


        }
        else
            if (mcpacketdata.count(id)) {
                printf("name for %i '%s'\n", id, mcpacketdata[id][0][0].c_str());
                for (size_t i = 1; i < mcpacketdata[id].size(); i++)
                {
                    auto row = mcpacketdata[id][i];
                    if (row.size() > 1) {
                        printf("'%s' '%s' ", row[0].c_str(), row[1].c_str());
                        auto type = row[1];
                        if (type.find("Int") == 0) {
                            printf("_%i_", readInt());
                        }
                        else if (type.find("Long") == 0) {
                            printf("_%l_", readLong());
                        }
                        else if (type.find("VarInt") == 0) {
                            printf("_%i_", readVarInt());
                        }
                        else if (type.find("Short") == 0) {
                            printf("_%i_", readShort());
                        }
                        else
                            if (type.find("Byte") == 0) {
                                printf("_%i_", (int)readByte());
                            }
                            else
                                if (type.find("Unsigned Byte") == 0) {
                                    printf("_%i_", (unsigned int)readByte());
                                }
                                else
                                    if (type.find("Angle") == 0) {
                                        printf("_%i_", (unsigned int)readByte());
                                    }
                                    else
                                        if (type.find("String") == 0) {
                                            printf("_%s_", readString().c_str());
                                        }
                                        else if (type.find("UUID") == 0) {
                                            printf("_%s_", readUUID().c_str());
                                        }
                                        else
                                            if (type.find("Boolean") == 0) {
                                                printf("_%s_\n", readByte() == 1 ? "true" : "false");
                                            }
                                            else
                                                if (type.find("Position") == 0) {
                                                    auto val = readLong();
                                                    int x = val >> 38;
                                                    int y = val & 0xFFF;
                                                    int z = (val << 26 >> 38);
                                                    printf("_%i|%i|%i_\n", x, y, z);
                                                }
                                                else if (type.find("Float") == 0) {
                                                    printf("_%f_", readFloat());
                                                }
                                                else if (type.find("Double") == 0) {
                                                    printf("_%f_", readDouble());
                                                }
                                                else {
                                                    printf("unknown type !!!!!!");

                                                }
                    }
                    if (row.size() > 2) {
                        printf("'%s' ", row[2].c_str());
                    }
                    printf("\n");
                }
            }
    }
    size_t readpos = 0;
    void read(void* s, size_t l, size_t& read) {
        if ((readpos + l) > size) {
            printf("out of bounds(1)\n");
            //exit(0);
            return;

        }
        memcpy(s, buffer + readpos, l);
        read = l;
        readpos += l;
    }
    char* read(size_t l) {
        char* out = new char[l];
        if ((readpos + l) > size) {
            printf("out of bounds(2)\n");
            exit(0);
            return 0;
        }
        memcpy(out, buffer + readpos, l);
        readpos += l;
        return out;
    }
    char readByte() {
        readpos++;
        return buffer[readpos - 1];
    }
    int readVarInt() {
        int i = 0;
        int j = 0;
        while (true) {
            int k;
            sf::Int8 c = 0;
            size_t recv = 0;
            read((char*)&c, 1, recv);
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
    void rotate(char* ptr, size_t l) {
        char c = 0;
        for (size_t i = 0; i < (l / 2); i++)
        {
            c = ptr[i];
            ptr[i] = ptr[l - i - 1];
            ptr[l - i - 1] = c;
        }
    }

    int64_t readLong() {
        int64_t out = 1;
        size_t recv = 0;
        read(&out, 8, recv);
        //printData((char*)&out, 8);
        rotate((char*)&out, 8);
        //printData((char*)&out, 8);
        return out;
    }
    int16_t readShort() {
        int16_t out = 1;
        size_t recv = 0;
        read(&out, 2, recv);
        //printData((char*)&out, 4);
        rotate((char*)&out, 2);
        //printData((char*)&out, 4);
        return out;
    }
    int32_t readInt() {
        int32_t out = 1;
        size_t recv = 0;
        read(&out, 4, recv);
        //printData((char*)&out, 4);
        rotate((char*)&out, 4);
        //printData((char*)&out, 4);
        return out;
    }
    string readUUID() {
        //int32_t out = 1;

        char uuid[16];
        size_t recv = 0;
        read(&uuid, 16, recv);
        //printData((char*)&out, 4);
        //rotate((char*)&out, 4);
        //printData((char*)&out, 4);

        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; ++i) {
            ss << std::setw(2) << std::hex << (0xff & (unsigned int)uuid[i]);

        }
        string out = ss.str();
        out = out.substr(0, 8) + "-" +
            out.substr(7, 4) + "-" +
            out.substr(11, 4) + "-" +
            out.substr(15, 4) + "-" +
            out.substr(19, 12);

        return out;
    }
    float readFloat() {
        float out = 1;
        size_t recv = 0;
        read(&out, 4, recv);
        //printData((char*)&out, 4);
        rotate((char*)&out, 4);
        //printData((char*)&out, 4);
        return out;
    }
    double readDouble() {
        double out = 1;
        size_t recv = 0;
        read(&out, 4, recv);
        //printData((char*)&out, 4);
        rotate((char*)&out, 4);
        //printData((char*)&out, 4);
        return out;
    }
    std::string readString() {
        size_t length = readVarInt();
        printf("string size : %i\n", length);
        char* str = new char[length + 1];
        str[length] = '\0';
        size_t l = 1;
        size_t pos = 0;
        while (pos < length) {
            read(str + pos, length, l);
            //printf("received %ld\n", l);
            pos += l;
        }
        return std::string(str, length);
    }


};