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
	int area = 0;
	std::vector<std::vector<bool>> matrix;
};

void printMatrix(std::vector<std::vector<bool>>& mat);
void rotate90Clockwise(std::vector<std::vector<bool>>& mat);

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
        int leftMost = Y;
		int rightMost = 0;
		int topMost = X;
		int bottomMost = 0;
        int area = 0;
        std::vector<std::vector<bool>> organismMatrixFull(X, std::vector<bool>(Y, false));
        for (int i = 0; i < X; i++) {
            for (int j = 0; j < Y; j++) {
                if (matrix[i][j].isOrganism && matrix[i][j].idOfOrganism == searchId) {
                    organismMatrixFull[i][j] = true;
                    area++;
                    if (i < topMost) {
						topMost = i;
                    }

                    if (i > bottomMost) {
						bottomMost = i;
                    }

                    if (j < leftMost) {
						leftMost = j;
                    }

                    if (j > rightMost) {
                        rightMost = j;
                    }
                }
            }
        }

		int Xsmaller = bottomMost - topMost + 1;
		int Ysmaller = rightMost - leftMost + 1;
        std::vector<std::vector<bool>> organismMatrix(Xsmaller, std::vector<bool>(Ysmaller, false));
        for (int i = 0; i < Xsmaller; i++) {
            for (int j = 0; j < Ysmaller; j++) {
				organismMatrix[i][j] = organismMatrixFull[topMost + i][leftMost + j];
            }
        }


        organism.id = searchId;
        organism.matrix = organismMatrix;
        organism.rotation = 0;
		organism.area = area;
        organisms.push_back(organism);
    }


	//Print organisms
    for (Organism organism : organisms) {
        std::cout << "Organism id: " << organism.id << std::endl;
		printMatrix(organism.matrix);
        std::cout << std::endl;
    }

	/*std::cout << "Rotated organisms before:" << std::endl;
    printMatrix(organisms[0].matrix);
    std::cout << "Rotated organisms after:" << std::endl;
    rotate90Clockwise(organisms[0].matrix);
	printMatrix(organisms[0].matrix);*/

	//Find unique organisms
	int countUniqueOrganisms = 0;
	std::set<int> matchingOrganismsId = std::set<int>();
    for (Organism organism : organisms) {
        bool found = false;
		//Search if any have the same area
        for (int i = 0; i < organisms.size(); i++) {
			Organism organismComparing = organisms[i];

            //If any have the same area, then rotate and compare
            if (organismComparing.area == organism.area && organismComparing.id != organism.id) {

				//Rotate the organismComparing matrix 4 times and compare with the original organism matrix
                for (int j = 0; j < 4; j++) {
                    if (!found && organismComparing.matrix == organism.matrix) {
                        bool foundComparing = matchingOrganismsId.find(organismComparing.id) == matchingOrganismsId.end();
						bool foundOriginal = matchingOrganismsId.find(organism.id) == matchingOrganismsId.end();
                        if (foundComparing && foundOriginal) {
                            matchingOrganismsId.insert(organismComparing.id);
                            matchingOrganismsId.insert(organism.id);
                            countUniqueOrganisms++;
                        }
                        else if(foundComparing) {
                            matchingOrganismsId.insert(organismComparing.id);
                        }
                        else if(foundOriginal) {
                            matchingOrganismsId.insert(organism.id);                            
						}
                        found = true;
                    }

                    rotate90Clockwise(organismComparing.matrix);                    				
                }                
            }            
        }

        if (!found) {
            countUniqueOrganisms++;
        }
    }

	std::cout << "Stevilo unikatnih organizmov: " << countUniqueOrganisms << std::endl;

   
}

void printMatrix(std::vector<std::vector<bool>>& mat) {
    for (int i = 0; i < mat.size(); i++) {
        for (int j = 0; j < mat[i].size(); j++) {
            if (mat[i][j]) {
                std::cout << "1";
            }
            else {
                std::cout << "0";
            }
        }
        std::cout << std::endl;
    }
}

void flipHorizontal(std::vector<std::vector<int>>& mat) {
    for (auto& row : mat)
        std::reverse(row.begin(), row.end());
}
void flipVertical(std::vector<std::vector<int>>& mat) {
    std::reverse(mat.begin(), mat.end());
}
void rotate90Clockwise(std::vector<std::vector<bool>>& mat) {
    int n = mat.size(), m = mat[0].size();
    std::vector<std::vector<bool>> res(m, std::vector<bool>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            res[j][n - 1 - i] = mat[i][j];
    mat = std::move(res);
}
