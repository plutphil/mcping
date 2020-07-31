
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <streambuf>
#include <istream>
#include <sstream>
#include <vector>
#include <ctime>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <iomanip>

#include <imgui.h>
#include "imgui-sfml/imgui-SFML.h"

//#include "CByteBuffer.h"
#include "mcpackets.hpp"
#include "ByteBuffer.hpp"
#include "Util.hpp"
#include "Response.hpp"
#include "MCProtocol.hpp"
using namespace std::chrono;
using namespace std;






#ifdef WIN32
#define STDFONT "C:\\Windows\\Fonts\\arial.ttf"
#endif


int main(char** argv,int args) {
    sf::TcpSocket socket;
    std::string host = "localhost";
    socket.setBlocking(true);
    short port = 25565;
    if (socket.connect(host.c_str(), port) != sf::Socket::Done){
        printf("no connection\n");
        exit(-1);
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
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
    window.setFramerateLimit(60);
    
    ImGui::SFML::Init(window);
    //ImGui::CreateContext();
    //ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    
    MCProtocol mcprot=MCProtocol(socket);
    //mcprot.sendHandshake(socket);
    //mcprot.receiveHandshake();
    //mcprot.ping();
    mcprot.login();
    sf::Font font;
    if (!font.loadFromFile(STDFONT))
    {
        printf("Error opening font !");
    }
    sf::Clock deltaClock;
    char buf[100] = { '\0' };
    while (window.isOpen())
    {

        
            
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        mcprot.readPacket();
        ImGui::SFML::Update(window, deltaClock.restart());
        //ImGui::NewFrame();
        
        ImGui::Begin("k");
        ImGui::Text(to_string(mcprot.pingttime).c_str());
        ImGui::InputText("", buf, IM_ARRAYSIZE(buf));
        if (ImGui::Button(">")) {
            MCPacket bbmctest = MCPacket();
            //bbmctest.writeVarInt(15);
            bbmctest.writeVarInt(0x00);
            bbmctest.writeVarInt(0x01);
            
            bbmctest.writeString(string(buf));
            bbmctest.send(socket);
        }
        ImGui::BeginChild("Scrolling");
        string strtext;
        for (auto e : mcprot.chatmesgs) {
           
            ImGui::Text("%s", e.c_str());
        }
       
        ImGui::EndChild();

        
        ImGui::End();
        
        window.clear(sf::Color::Black);
        sf::Text text;

        // select the font
        text.setFont(font); // font is a sf::Font

        // set the string to display
        text.setString("");
        
        

        // set the character size
        text.setCharacterSize(20); // in pixels, not points!
        
        // set the color
        text.setFillColor(sf::Color::White);

        // set the text style
        //text.setStyle(sf::Text::Bold | sf::Text::Underlined);



        // inside the main loop, between window.clear() and window.display()
        
        window.draw(text);
        ImGui::SFML::Render(window);
        window.display();
    } 
    ImGui::SFML::Shutdown();

/*
    char c = 0;
    size_t received = 0;
    //socket.setBlocking(false);
   
    while (socket.receive(&c, 1, received) == sf::Socket::Done) {
        //if (isascii(c))printf("%c", c);
        
        printf("%02x", (uint8_t)c);
    }*/
    return 0;
}