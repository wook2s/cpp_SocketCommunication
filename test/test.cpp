#include <iostream>
#include <string>
#include <vector>


std::vector<int> v1;

int main()
{
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	v1.push_back(4);
	v1.push_back(5);

	v1.erase(v1.begin()+0);

	for(int i = 0; i < v1.size(); i++)
	{
		std::cout<<v1[i];
		std::cout<<"  ";
	}
}