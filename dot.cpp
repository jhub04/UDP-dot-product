#include <iostream>
#include <numeric>
#include <vector>
#include <asio.hpp>

using namespace std;

const size_t max_udp_message_size = 0xffff - 20 - 8; // 16 bit UDP length field - 20 byte IP header - 8 byte UDP header


int dotProduct(vector<int> v1, vector<int> v2) {
    int result = 0;
    for (int i = 0; i < v1.size(); i++) {
        result += v1[i] * v2[i];
    }

    return result;
}

class EchoServer {
    asio::ip::udp::socket socket;

    public:
    EchoServer(asio::io_context &io_context) : socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v6(), 3000)) {}

    asio::awaitable<void> handle_request(asio::ip::udp::endpoint endpoint, string message) {
        // Parse the string into two vectors
        std::vector<int> v1, v2;
        std::stringstream ss(message);
        std::string firstPart, secondPart;

        if (std::getline(ss, firstPart, ';') && std::getline(ss, secondPart)) {
            std::stringstream firstStream(firstPart);
            std::stringstream secondStream(secondPart);
            std::string num;

            while (std::getline(firstStream, num, ',')) {
                v1.push_back(std::stoi(num));
            }

            while (std::getline(secondStream, num, ',')) {
                v2.push_back(std::stoi(num));
            }
        };

        // Calculate the dot product
        int dp = dotProduct(v1, v2);
        cout << dp << endl;


        co_await socket.async_send_to(asio::buffer(message, message.length()), endpoint, asio::use_awaitable);
        cout << "Server: sent: " << message
             << ", to " << endpoint.address() << ":" << endpoint.port() << endl;
    }

    asio::awaitable<void> start() {
        for (;;) {
            char buffer[max_udp_message_size];
            asio::ip::udp::endpoint endpoint;
            auto bytes_transferred = co_await socket.async_receive_from(asio::buffer(buffer, max_udp_message_size), endpoint, asio::use_awaitable);

            auto message = string(buffer, bytes_transferred);
            cout << "Server: received: " << message
                 << ", from " << endpoint.address() << ":" << endpoint.port() << endl;

            co_spawn(socket.get_executor(), handle_request(std::move(endpoint), std::move(message)), asio::detached);
        }
    }
};

class EchoClient {
    public:
    asio::awaitable<void> start() {
        auto executor = co_await asio::this_coro::executor;
        asio::ip::udp::socket socket(executor, asio::ip::udp::endpoint(asio::ip::udp::v6(), 0));

        // Resolve host (DNS-lookup if needed)
        auto endpoint = (co_await asio::ip::udp::resolver(executor)
                             .async_resolve(asio::ip::udp::v6(), "localhost", to_string(3000), asio::use_awaitable))
                            .begin()
                            ->endpoint();

        // Send the vectors
        //std::string message("hello");
        std::string message("1,2,3;4,5,6"); // 1*4 + 2*5 + 3*6 = 32
        auto bytes_transferred = co_await socket.async_send_to(asio::buffer(message, message.length()), endpoint, asio::use_awaitable);
        cout << "Client: sent: " << message
             << ", to " << endpoint.address() << ":" << endpoint.port() << endl;

        char buffer[max_udp_message_size];
        bytes_transferred = co_await socket.async_receive_from(asio::buffer(buffer, max_udp_message_size), endpoint, asio::use_awaitable);
        cout << "Client: received: " << string(buffer, bytes_transferred)
             << ", from " << endpoint.address() << ":" << endpoint.port() << endl;
    }
};

int main() {
    // Provides asynchronous I/O functionality
    asio::io_context event_loop(1);

    EchoServer echo_server(event_loop);
    co_spawn(event_loop, echo_server.start(), asio::detached);

    EchoClient echo_client;
    co_spawn(event_loop, echo_client.start(), asio::detached);

    event_loop.run();
}




