/* Standard C++ */
#include <iostream> // std::cout
#include <string> // std::string
#include <deque> // std::deque to move the first line to the last one
#include <iterator> // std::istream_iterator, std::ostream_iterator, std::back_inserter
#include <algorithm> // std::copy

/* Boost */
#include <boost/filesystem.hpp> // boost::filesystem::path
#include <boost/program_options.hpp> // boost::program_options::options_description, boost::program_options::positional_options_description, boost::program_options::command_line_parser, boost::program_options::variables_map, boost::program_options::store

/* Our headers */
#include "StreamLine.hpp" // StreamLine class to read lines from a file

/**
* A small program which tells us who should pay for pizza each week. Reads the file which is given on the command line into memory, and
* then prints the first line (the name of the person who will pay this week). It then moves the top of the vector to the back, and writes it
* back to the file.
**/
int main(int argc, char* argv[])
{
	std::string ourName = boost::filesystem::path(argv[0]).filename().string(); // Fetch our name

	/** Set up option handling **/

	/* Options which should be shown to the user on the command line*/
	boost::program_options::options_description cmdOpts("Options");
	cmdOpts.add_options()
		("help,h", "Print this help message");

	/* Options which positional options are converted to */
	boost::program_options::options_description posConv("Options which positional options are converted to.");
	posConv.add_options()
		("file,f", "The name of the file which contains the player list, separated by newlines.");

	/* Describe how to convert positional options to named options */
	boost::program_options::positional_options_description posOpts;
	posOpts.add("file", 1); // Convert the first positional option (there should only be 1) to a "file" option

	/* Merge all options into 1 for parsing */
	boost::program_options::options_description all("All options");
	all.add(cmdOpts).add(posConv);

	/* Parse the command line and store options */
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv) // Parse the command line
		.options(all) // Use the combined options which we declared above
		.positional(posOpts) // Use the positional options conversion list which we described above
		.run(), vm); // Parse the command line and store the results in the variables map which we declared above

	/** Handle options **/

	if (vm.count("help")) // Help requested
	{		
		std::cout << ourName << ": a program which prints the next person who will pay for pizza and shifts the order for the next time it is run." << std::endl << std::endl << "Usage: " << ourName << " [file to parse]" << std::endl << std::endl << "The program expects the file to contain a list of members who will pay in turn which is separated by spaces." << std::endl;
		return 1;
	}

	if (!vm.count("file")) // No filename given
	{
		std::cerr << ourName << ": need the name of a file which contains the members of the group with 1 on each line." << std::endl;
		return 2;
	}

	// If we got here,we have a file name
	std::deque<std::string> fLines; // Holds lines of the file so that we can move the top line to the back and rewrite it to the file
	boost::filesystem::path nameFilePath(vm["file"].as<std::string>()); // Convert the name which was given to a path

	if (!boost::filesystem::exists(nameFilePath)) // Path DNE, error
	{
		std::cerr << ourName << ": file " << nameFilePath << " doesn't exist." << std::endl;
		return 3;
	}

	// If we got here, the file exists
	boost::filesystem::ifstream nameFileStream(nameFilePath); // Open the file for reading
	std::copy(std::istream_iterator<StreamLine>(nameFileStream), std::istream_iterator<StreamLine>(), std::back_inserter(fLines)); // Read lines from the file using our custom class. We can store the results directly into the queue because the class can be implicitly converted to an std::string.
	nameFileStream.close(); // Close the input stream - we have read everything
	std::cout << fLines.front() << " is paying this week." << std::endl; // Print the name of the person who will have to pay this week
	fLines.push_back(fLines.front()); // Copy the first person in the queue to the back
	fLines.pop_front(); // Remove the reference to the person from the front of the queue 

	/* Delete the existing file so that we can overwrite it with our queue's data */
	try
	{
		boost::filesystem::remove(nameFilePath); // Try to delete the path
	}

	catch (boost::filesystem::filesystem_error& bffe)
	{
		std::cerr << "Encountered an error while trying to delete the file " << nameFilePath << ": " << bffe.what() << std::endl;
		return 4;
	}

	/* Write the data to the file */
	boost::filesystem::ofstream newNameFileStrm(nameFilePath); // Create an object to write to the same file
	std::copy(fLines.cbegin(), fLines.cend()-1, std::ostream_iterator<std::string>(newNameFileStrm, "\n")); // Write the data back to the file, separated by newlines. Stop at the second-to-last person so that we can write the last person without a newline
	std::copy(fLines.cbegin(), fLines.cend() - 1, std::ostream_iterator<std::string>(std::clog, "\n")); // Debugging - write the lines to std::clog as well
	newNameFileStrm << fLines.back(); // Write the last person to the file without a new line

	// All done, exit
	return 0;
}