#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct User {
    string name;
    string regNo;
    string password;
};

string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) { return str; }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<User> database = {
    {"Ajay k", "RA2511003010757", "Ajay k@123"},
    {"Gourav", "RA2511003010758", "Gourav@123"},
    {"Piyush Dhal", "RA2511003010759", "Piyush Dhal@123"},
    {"Rudra Singh", "RA2511003010760", "Rudra Singh@123"},
    {"Swarnim saini", "RA2511003010761", "Swarnim saini@123"}
};

string urlDecode(string str) {
    string result;
    char ch;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            int hex;
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &hex);
            ch = static_cast<char>(hex);
            i += 2;
        } else if (str[i] == '+') {
            ch = ' ';
        } else {
            ch = str[i];
        }
        result += ch;
    }
    return result;
}

string getParam(string body, string key) {
    string searchKey = key + "=";
    size_t pos = body.find(searchKey);
    
    if (pos == string::npos) return ""; 
    
    pos += searchKey.length();
    size_t endPos = body.find("&", pos); 
    
    if (endPos == string::npos) endPos = body.length();
    
    return urlDecode(body.substr(pos, endPos - pos));
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "Server running on port 8080..." << endl;

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (int*)&addrlen);

        char buffer[3000];
        memset(buffer, 0, sizeof(buffer)); 

        int valread = recv(new_socket, buffer, 3000, 0);
        
        if (valread > 0) {
            string req(buffer);
            size_t pos = req.find("\r\n\r\n");
            string body = (pos != string::npos) ? req.substr(pos + 4) : "";

            string inputRegNo = trim(getParam(body, "regno"));
            string inputName = trim(getParam(body, "name"));
            string inputPass = trim(getParam(body, "password"));

            cout << "------------------------------------------------" << endl;
            cout << "INPUT RECEIVED:" << endl;
            cout << "RegNo: [" << inputRegNo << "]" << endl;
            cout << "Name: [" << inputName << "]" << endl;
            cout << "Pass: [" << inputPass << "]" << endl;

            if (inputRegNo.empty() || inputName.empty() || inputPass.empty()) {
                cout << ">> ERROR: Empty fields received." << endl;
                send(new_socket, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nfail", 86, 0);
                closesocket(new_socket);
                continue;
            }

            bool loginSuccess = false;
            bool userFound = false;

            for (const auto& user : database) {
                if (user.regNo == inputRegNo) {
                    userFound = true;
                    if (user.name == inputName) {
                        if (user.password == inputPass) {
                            loginSuccess = true;
                        } else {
                            cout << ">> LOGIN FAILED: Password Mismatch!" << endl;
                            cout << "   Expected: " << user.password << endl;
                        }
                    } else {
                        cout << ">> LOGIN FAILED: Name Mismatch!" << endl;
                        cout << "   Expected: " << user.name << endl;
                    }
                    break;
                }
            }

            if (!userFound) {
                cout << ">> LOGIN FAILED: RegNo '" << inputRegNo << "' not found!" << endl;
            }

            string response;
            if (loginSuccess) {
                cout << "✅ LOGIN SUCCESSFUL" << endl;
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nsuccess";
            } else {
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nfail";
            }

            send(new_socket, response.c_str(), response.length(), 0);
        }

        closesocket(new_socket);
    }

    WSACleanup();
    return 0;
}