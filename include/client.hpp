// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_CLIENT_HPP_
#define INCLUDE_CLIENT_HPP_
#include <beast/core.hpp>
#include <beast/http.hpp>
#include <beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
int client(){
   try {
    std::string const host_ip = "127.0.0.1";
    std::string const port = "80";
    std::string const target = "/v1/api/suggest";
    int version = 11;
    nlohmann::json j;
    std::string input;
    std::cin >> input;
    j["input"] = input;

    // The io_context is required for all I/O
    net::io_service ios;

    // These objects perform our I/O
    tcp::resolver resolver{ios};
    tcp::socket socket{ios};

    // Make the connection on the IP address we get from a lookup
    net::connect(socket,
                 resolver.resolve(boost::asio::ip::tcp::resolver::query
                                  {host_ip, "http"}));

    // Set up an HTTP POST request message
    http::request<http::string_body> req{};
    req.version = version;
    req.method(http::verb::post);
    req.target(target);
    req.set(http::field::host, host_ip + ":" + port);
    req.set(http::field::user_agent, BEAST_VERSION_STRING);
    req.set(beast::http::field::content_type, "application/json");
    req.body = to_string(j);
    req.prepare_payload();

    // Send the HTTP request to the remote host
    http::write(socket, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(socket, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;

    // Gracefully close the socket
    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes
    // so don't bother reporting it.
    //
    if (ec && ec != beast::errc::not_connected) throw beast::system_error{ec};

    // If we get here then the connection is closed gracefully
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#endif // INCLUDE_CLIENT_HPP_
