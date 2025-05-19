#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>

struct MatrixItem {
	bool isOrganism = false;
	int idOfOrganism = -1;
};

struct Organism {
	int id = -1;
	int rotation = 0;
	std::vector<std::vector<bool>> matrix;
};

int main(int argc, char* argv[])
{
    //Args
    std::string fileName;
    if (argc != 2) {
        std::cout << "Primer zagona programa: ./program.exe test01.in" << std::endl;
        exit(-1);
    }
    else {
        fileName = std::string(argv[1]);
    }

    //Initialization of values used to read from file
    std::string line;
    int X = NULL;
    int Y = NULL;
    std::ifstream ReadFile(fileName);
    
	//Get first line of the file
    std::getline(ReadFile, line);
    std::string item;
    std::stringstream ss(line);
    while (std::getline(ss, item, ' ')) {
        if (X == NULL) {
            X = std::stoi(item);
        }
        else {
            Y = std::stoi(item);
        }
    }

	std::vector<std::vector<MatrixItem>> matrix(X, std::vector<MatrixItem>(Y));

    //Read matrix line by line
	int countLine = 0;
    while (std::getline(ReadFile, line)) {
        std::string item;
        std::stringstream ss(line);
        while (std::getline(ss, item, ' ')) {
            for(int i = 0; i < item.length(); i++) {
                if (item[i] == '1') {
                    matrix[countLine][i].isOrganism = true;
                }
			}
        }
        countLine++;
    }

    // Close the file
    ReadFile.close();

	//Label organisms with ids, checking top and left side of each cell
	int countOrganismsForId = 1;
    std::set<int> setOfIds = std::set<int>();
    for(int i = 0; i < X; i++) {
        for(int j = 0; j < Y; j++) {
            if (matrix[i][j].isOrganism) {
                int leftIndex = j - 1;
                int topIndex = i - 1;

                if (topIndex >= 0 && matrix[topIndex][j].isOrganism == true) {
					matrix[i][j].idOfOrganism = matrix[topIndex][j].idOfOrganism;					

					//Check left side if same id as top, otherwise change 
                    if (leftIndex >= 0) {
                        if (matrix[i][leftIndex].isOrganism == true && matrix[i][leftIndex].idOfOrganism != matrix[i][j].idOfOrganism) {
							int k = leftIndex;
                            setOfIds.erase(matrix[i][k].idOfOrganism);
                            while (true) {
								matrix[i][k].idOfOrganism = matrix[i][j].idOfOrganism;
								k--;
                                if (k < 0 || matrix[i][k].isOrganism == false) {
                                    break;
                                }
                            }
                        }
                    }
				}
                else if (leftIndex >= 0 && matrix[i][leftIndex].isOrganism == true) {
					matrix[i][j].idOfOrganism = matrix[i][leftIndex].idOfOrganism;
                }
                else {
					matrix[i][j].idOfOrganism = countOrganismsForId;
                    setOfIds.insert(countOrganismsForId);
                    countOrganismsForId++;
                }
            }			
        }
	}

	//Count organisms in the matrix
	int countOrganisms = 0;
	std::vector<int> foundOrganisms = std::vector<int>();
    for(int i= 0; i < X; i++) {
        for(int j = 0; j < Y; j++) {
            if (matrix[i][j].isOrganism) {
                if (std::find(foundOrganisms.begin(), foundOrganisms.end(), matrix[i][j].idOfOrganism) == foundOrganisms.end()) {
					countOrganisms++;
					foundOrganisms.push_back(matrix[i][j].idOfOrganism);
                }
            }
        }
	}

	std::cout << "Stevilo organizmov: " << countOrganisms << std::endl;

	//Isolate organisms in the matrix
	std::vector<Organism> organisms = std::vector<Organism>();
    for (int searchId : setOfIds) {
        Organism organism = Organism();
        std::vector<std::vector<bool>> organismMatrix(X, std::vector<bool>(Y, false));
        for (int i = 0; i < X; i++) {
            for (int j = 0; j < Y; j++) {
                if (matrix[i][j].isOrganism && matrix[i][j].idOfOrganism == searchId) {
                    organismMatrix[i][j] = true;
                }
            }
        }

        organism.id = searchId;
        organism.matrix = organismMatrix;
        organism.rotation = 0;
        organisms.push_back(organism);
    }


	//Print organisms
    for (int i = 0; i < organisms.size(); i++) {
		std::cout << "Organism id: " << organisms[i].id << std::endl;
        for (int j = 0; j < X; j++) {
            for (int k = 0; k < Y; k++) {
                if (organisms[i].matrix[j][k]) {
                    std::cout << "1";
                }
                else {
                    std::cout << "0";
                }
            }
            std::cout << std::endl;
        }
		std::cout << std::endl;
    }
   
}
