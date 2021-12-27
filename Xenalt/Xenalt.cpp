
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

string strChoice = "";
fs::path rootPath = fs::path();
fs::path mirrorPath = fs::path();
fs::path paifPath = fs::path();


void prompt(string message) {
	cout << endl << message;
}

string getInputString() {
	string input;
	while (input.empty()) {
		getline(cin, input);
	}

	return input;
}

char getInputChar() {
	char input;
	cin >> input;

	return input;
}

void resetAll() {
	strChoice = "";
	rootPath = fs::path();
	mirrorPath = fs::path();
}

void deleteSymLinks(fs::path path) {
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_symlink()) {
			remove(entry.path());
		}
	}
}

void makeSymLink(fs::directory_entry original) {

	bool isDirectory = original.is_directory();
	string fileName = original.path().filename().string();

	if (!isDirectory && fileName == "paif.dat") {
		return;
	}

	string target = mirrorPath.string() + "\\" + fileName;

	CreateSymbolicLinkA(target.c_str(), original.path().string().c_str(), 0);
	
}


void getRootPath() {
	prompt("Please enter the Xenimus path to mirror: ");
		
	rootPath = fs::path(getInputString());

	if (!fs::exists(rootPath) || !fs::is_directory(rootPath)) {
		rootPath = fs::path();
		prompt("Please enter a valid path.");
		getRootPath();
	}
}

void getMirrorPath() {
	prompt("Please enter the mirror path: ");
	
	mirrorPath = fs::path(getInputString());

	if (!fs::exists(mirrorPath)) {
		prompt("The specified path does not exist, would you like to create it? (Y)es/(N)o: ");
		getline(cin, strChoice);

		if ("Y" == strChoice || "y" == strChoice) {
			fs::create_directories(mirrorPath);
		}
		else {
			mirrorPath = fs::path();
			getMirrorPath();
		}
	}
}

void promptForExistingPaif() {
	prompt("Would you like to use an existing paif.dat (account number) file? (Y)es/(N)o: ");
	getline(cin, strChoice);

	if ("Y" == strChoice || "y" == strChoice) {
		prompt("Please enter the paif.dat path: ");
		
		paifPath = fs::path(getInputString());

		if (!fs::exists(paifPath) || fs::is_directory(paifPath)) {
			prompt("Pleast enter a valid path.");
			promptForExistingPaif();
		}
	}
	else {
		paifPath = fs::path();
	}
}

void createSymLinks() {
	for (const auto& entry : fs::directory_iterator(rootPath)) {
		makeSymLink(entry);
	}
}

void handlePaif() {
	if (paifPath.empty()) {
		paifPath = rootPath.append("paif.dat");
	}

	fs::copy_file(paifPath, mirrorPath.append("paif.dat"));
}

void promptSuccess() {
	prompt("Success!");
	prompt("Would you like to create another? (Y)es/(N)o: ");
	getline(cin, strChoice);

	if ("Y" == strChoice || "y" == strChoice) {
		promptOptions();
	}
}

void createNewMirror() {
	getRootPath();
	getMirrorPath();
	promptForExistingPaif();
	deleteSymLinks(mirrorPath);
	createSymLinks();
	handlePaif();
	promptSuccess();
}

void repairMirror() {
	getRootPath();
	getMirrorPath();
	deleteSymLinks(mirrorPath);
	createSymLinks();
	promptSuccess();
}



void promptOptions() {
	
	std::cout << std::endl << "Select an action from the choices below:" << std::endl
		<< "1. Create a new alt directory." << std::endl
		<< "2. Repair alt directory." << std::endl
		<< "3. Exit." << std::endl << std::endl;

	cin >> strChoice;

	int intChoice = atoi(strChoice.c_str());

	switch (intChoice) {
	case 1:
		createNewMirror();
		break;
	case 2:
		repairMirror();
		break;
	case 3:
		break;
	default:
		std::cout << std::endl << "Please select a valid option." << std::endl << std::endl;
		promptOptions();
	}

}


int main() {
	promptOptions();
	return 0;
}