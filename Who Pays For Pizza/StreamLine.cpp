/* Standard C++ */
#include <string> // std::getline, std::string
#include <istream> // std::istream

/* Our headers */
#include "StreamLine.hpp" // Class definition

/**
* @name operator std::string
* @desc Returns the actual string which we read from the stream.
* @return The string which we read from the stream.
**/
StreamLine::operator std::string() const
{
	return data; // Return the data which we read
}

/**
* @name operator>>
* @desc Reads a StreamLine object from an input stream.
* @param is The stream which we will read the line from.
* @param l The line object into which we will read the data.
**/
std::istream& operator>>(std::istream& is, StreamLine& l)
{
	std::getline(is, l.data); // Read a line from the stream into the object's member
	return is; // Return the stream so that we can continue chaining operator>>
}