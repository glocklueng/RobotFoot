#include <ostream>
#include <boost/regex.hpp>

#include "SerialInterface.h"
#include "logger.h"

SerialInterface::SerialInterface(boost::asio::io_service &io_service,
                           const std::string &port_name,
                           unsigned int baud)
:  _serialPort(io_service)
{
	try
	{
		_serialPort.open(port_name);
	}
	catch (std::exception &ex)
	{
      Logger::getInstance(Logger::LogLvl::ERROR) << "Error in SerialInterface.cpp : could not open port " << port_name << std::endl;
      std::exit(1);
	}
 
	if(!_serialPort.is_open())
	{
      Logger::getInstance(Logger::LogLvl::ERROR) << "Can't open serial port " << port_name << std::endl;
      std::exit(1);
   }
	boost::asio::serial_port_base::baud_rate baud_option(baud);
	_serialPort.set_option(baud_option);
}

SerialInterface::~SerialInterface()
{
	if(!_serialPort.is_open()) { return; }
    _serialPort.cancel();
    _serialPort.close();
}

void SerialInterface::start_read(std::function<void(std::vector<char>)> function)
{
    read_async(function);
}

void SerialInterface::write_async(std::vector<char>& command)
{
	if(!_serialPort.is_open()) { return; }
    boost::asio::async_write(_serialPort, boost::asio::buffer(command),
        [this](boost::system::error_code ec, std::size_t length)
        {
            if (ec)
                Logger::getInstance(Logger::LogLvl::ERROR) << "Error in SerialInterface::write_async : " << ec.message() << std::endl;
        }); 
}

void SerialInterface::read_async(std::function<void(std::vector<char>)> function)
{
	if(!_serialPort.is_open()) { return; }
    _serialPort.async_read_some(boost::asio::buffer(_read_msg, MAX_SIZE),
        [this, function](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                function(std::vector<char>(_read_msg, _read_msg + sizeof(_read_msg)));
                read_async(function);  
            }
            else
            {
                Logger::getInstance(Logger::LogLvl::ERROR) << "Error in SerialInterface::read_async : " << ec.message() << std::endl;
            }
        }); 
}

// Deprecated
std::vector<char> SerialInterface::read_sync(std::vector<char>& command)
{
	if(!_serialPort.is_open()) { return std::vector<char>(); }

    boost::asio::streambuf streamBuffer;
    std::ostream os(&streamBuffer);
    os.write(command.data(), command.size());
    boost::asio::write(_serialPort, streamBuffer);

    streamBuffer.consume(streamBuffer.size());
    boost::asio::read_until(_serialPort, streamBuffer, "\r\n");

	const char* start_ptr = boost::asio::buffer_cast<const char*>(streamBuffer.data());
	return std::vector<char>(start_ptr, start_ptr + streamBuffer.size());
}

// Deprecated
void SerialInterface::write(std::vector<char>& command)
{
	if(!_serialPort.is_open()) { return; }

    boost::asio::streambuf streamBuffer;
    std::ostream os(&streamBuffer);
    os.write(command.data(), command.size());

    boost::asio::write(_serialPort, streamBuffer);
}


