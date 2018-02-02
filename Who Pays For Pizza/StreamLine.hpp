#ifndef STREAMLINE_HPP
#define STREAMLINE_HPP

	/* Standard C++ */
	#include <string> // std::string
	#include <istream> // std::istream

	/**
	* This class is a string proxy which allows us to read lines from a stream using iterators.
	**/
	class StreamLine
	{
		public:
			/**
			* @name operator>>
			* @desc Reads a StreamLine object from an input stream.
			* @param is The stream which we will read the line from.
			* @param l The line object into which we will read the data.
			**/
			friend std::istream& operator>>(std::istream& is, StreamLine& l);

			/**
			* @name operator std::string
			* @desc Returns the actual string which we read from the stream.
			* @return The string which we read from the stream.
			**/
			operator std::string() const;

		private:
			std::string data; // The actual line which we read from the stream
	};

#endif // STREAMLINE_HPP