#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <iostream>
#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <random>
#include <openssl/hmac.h>
#include <openssl/sha.h>


using namespace std;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::websockets::client;

// Global token to store authentication token
std::string access_token;

class deribitManagement{
        websocket_client ws_client;
        const utility::string_t wsUrl = U("wss://test.deribit.com/ws/api/v2");
    public:
        // Function to connect to WebSocket
        deribitManagement(string clientId, string clientSecret){
            try {
                // Connect to WebSocket
                websocket_outgoing_message outgoing_msg;
                ws_client.connect(wsUrl).then([&]() {
                    std::cout << "WebSocket connected!" << std::endl;

                    // Authenticate WebSocket connection
                    json::value auth_msg;
                    auth_msg[U("jsonrpc")] = json::value::string(U("2.0"));
                    auth_msg[U("id")] = json::value::number(1);
                    auth_msg[U("method")] = json::value::string(U("public/auth"));
                    auth_msg[U("params")] = json::value::object();
                    auth_msg[U("params")][U("grant_type")] = json::value::string(U("client_credentials"));
                    auth_msg[U("params")][U("client_id")] = json::value::string(U(clientId));
                    auth_msg[U("params")][U("client_secret")] = json::value::string(U(clientSecret));

                    outgoing_msg.set_utf8_message(auth_msg.serialize());
                    ws_client.send(outgoing_msg).wait();
                    // ws_client.send(outgoing_msg.set_utf8_message(auth_msg.serialize())).wait();
                    std::cout << "Auth message sent!" << std::endl;
                }).wait();

                // Listen for server messages
                ws_client.receive().then([](websocket_incoming_message msg) {
                    auto response = msg.extract_string().get();
                    std::cout << "Received message: " << response << std::endl;
                }).wait();
            } catch (const std::exception &e) {
                std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
            
        }
        
        // Function to send buy request
        void placeOrder(string instrumentName, int amount, int id) {
            try{
                // Place buy order
                json::value buy_msg;
                buy_msg[U("method")] = json::value::string(U("private/buy"));
                buy_msg[U("params")] = json::value::object();
                buy_msg[U("params")][U("instrument_name")] = json::value::string(U(instrumentName));
                buy_msg[U("params")][U("amount")] = json::value::number(amount);
                buy_msg[U("params")][U("type")] = json::value::string(U("market"));
                // buy_msg[U("params")][U("label")] = json::value::string(U("market0000234"));
                buy_msg[U("params")][U("otoco_config")] = json::value::array();
                buy_msg[U("jsonrpc")] = json::value::string(U("2.0"));
                buy_msg[U("id")] = json::value::number(id);

                
                // ws_client.send(websocket_outgoing_message().set_utf8_message(buy_msg.serialize())).wait();
                websocket_outgoing_message buy_msg_out;
                buy_msg_out.set_utf8_message(buy_msg.serialize());
                auto res = ws_client.send(buy_msg_out).wait();
                std::cout << "Buy order sent!" << std::endl;
                // cout<<res;
                ws_client.receive().then([](websocket_incoming_message msg) {
                    auto response = msg.extract_string().get();
                    std::cout << "Received message: " << response << std::endl;
                }).wait();

                // Close the WebSocket connection
                // ws_client.close().wait();
                // std::cout << "WebSocket connection closed!" << std::endl;

            } catch (const std::exception &e) {
                std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
        }

        // Function to sell request
        void sellOrder(string instrumentName, int amount, int id) {
            try{
                json::value sell_msg;
                sell_msg[U("method")] = json::value::string(U("private/sell"));
                sell_msg[U("params")] = json::value::object();
                sell_msg[U("params")][U("instrument_name")] = json::value::string(U(instrumentName));
                sell_msg[U("params")][U("amount")] = json::value::number(amount);
                sell_msg[U("params")][U("type")] = json::value::string(U("stop_limit"));
                sell_msg[U("params")][U("price")] = json::value::number(3690);
                sell_msg[U("params")][U("trigger")] = json::value::string(U("last_price"));
                sell_msg[U("params")][U("trigger_price")] = json::value::number(3600);
                sell_msg[U("params")][U("otoco_config")] = json::value::array();
                sell_msg[U("jsonrpc")] = json::value::string(U("2.0"));
                sell_msg[U("id")] = json::value::number(id);

                websocket_outgoing_message sell_msg_out;
                sell_msg_out.set_utf8_message(sell_msg.serialize());
                auto res = ws_client.send(sell_msg_out).wait();
                
                std::cout << "Sell order sent!" << std::endl;
                
                ws_client.receive().then([](websocket_incoming_message msg) {
                    auto response = msg.extract_string().get();
                    std::cout << "Received message: " << response << std::endl;
                }).wait();

            } catch (const std::exception &e) {
                std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
        }

        void cancelOrder(string orderId){
            try{
                json::value cancel_msg;
                cancel_msg[U("method")] = json::value::string(U("private/cancel"));
                cancel_msg[U("params")] = json::value::object();
                cancel_msg[U("params")][U("order_id")] = json::value::string(U(orderId));
                cancel_msg[U("jsonrpc")] = json::value::string(U("2.0"));
                cancel_msg[U("id")] = json::value::number(2929);
                
                websocket_outgoing_message cancel_msg_out;
                cancel_msg_out.set_utf8_message(cancel_msg.serialize());
                auto res = ws_client.send(cancel_msg_out).wait();
                
                std::cout << "Sell order sent!" << std::endl;
                
                ws_client.receive().then([](websocket_incoming_message msg) {
                    auto response = msg.extract_string().get();
                    std::cout << "Received message: " << response << std::endl;
                }).wait();

            } catch (const std::exception &e) {
                    std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
        }

        void getOrderBook(string instrumentName, int depth){
            try{
                json::value cancel_msg;
                cancel_msg[U("method")] = json::value::string(U("public/get_order_book"));
                cancel_msg[U("params")] = json::value::object();
                cancel_msg[U("params")][U("instrument_name")] = json::value::string(U(instrumentName));
                cancel_msg[U("params")][U("depth")] = json::value::number(depth);
                cancel_msg[U("jsonrpc")] = json::value::string(U("2.0"));
                cancel_msg[U("id")] = json::value::number(2929);
                
                websocket_outgoing_message cancel_msg_out;
                cancel_msg_out.set_utf8_message(cancel_msg.serialize());
                auto res = ws_client.send(cancel_msg_out).wait();
                
                std::cout << "Sell order sent!" << std::endl;
                
                ws_client.receive().then([](websocket_incoming_message msg) {
                    auto response = msg.extract_string().get();
                    std::cout << "Received message: " << response << std::endl;
                }).wait();

            } catch (const std::exception &e) {
                    std::cerr << "Exception occurred: " << e.what() << std::endl;
            }
        }
};



int main() {
    // Setup API credentials
    std::string apiKey = "osXNyugb";
    std::string secretKey = "chwkEigRazYTFX03X_9LsWZ6-j8v4YfcowPJu3jEaH0";
    
    // fetchAuthToken();
    try{
        // placeOrder();
        deribitManagement test(apiKey, secretKey);
        // test.placeOrder("ETH-PERPETUAL", 1, 5909);
        // test.sellOrder("ETH-PERPETUAL", 1, 5909);
        // test.cancelOrder("ETH-SLTS-514823");
        
    } catch (std::exception e) {
        std::cerr << "Failed to fetch access token. Exiting." << std::endl;
    }

    // return 0;
}
