/* STL */
#include <iostream> // std::cout
#include <string> // std::string
#include <queue> // std::queue for kicks

/* Boost */
#include <boost/filesystem.hpp> // boost::filesystem::path
#include <boost/program_options.hpp> // boost::program_options::options_description, boost::program_options::positional_options_description, boost::program_options::command_line_parser, boost::program_options::variables_map, boost::program_options::store

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
	std::queue<std::string> fLines; // Holds lines of the file so that we can move the top line to the back and rewrite it to the file
	boost::filesystem::path nameFilePath(vm["file"].as<std::string>()); // Convert the name which was given to a path

	if (!boost::filesystem::exists(nameFilePath)) // Path DNE, error
	{
		std::cerr << ourName << ": file " << nameFilePath << " doesn't exist." << std::endl;
		return 3;
	}

	// If we got here, the file exists
	boost::filesystem::ifstream nameFileStream(nameFilePath); // Open the file for reading
	std::string curLine; // Used to hold the current line

	while (nameFileStream.good()) // Loop until the end of the file
	{
		std::getline(nameFileStream, curLine); // Fetch a line from the file
		fLines.push(curLine); // Store it in the vector
	}

	nameFileStream.close(); // Close the input stream - we have read everything

	// Print the name of the person who will have to pay this week
	std::cout << fLines.front() << " is paying this week." << std::endl;

	/* Move the first person in the queue to the back */
	std::string newLast = fLines.front(); // Queue's pop doesn't return the value which was popped, so we need to store it first
	fLines.pop(); // Remove the person who was at the front of the queue
	fLines.push(newLast); // Push the person who was at the front of the queue to the back, moving them to the back of the queue for next time

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

	while (!fLines.empty()) // Loop until we have written all of the lines in the new order
	{
		newNameFileStrm << fLines.front();
		
		if (fLines.size() != 1) // Not the last line in the vector, need to write a newline
		{
			newNameFileStrm << std::endl; // Write the name and end the line
		}

		//std::clog << fLines.front() << std::endl;
		fLines.pop(); // Remove the name which we just wrote
	}

	// All done, exit
	return 0;
}