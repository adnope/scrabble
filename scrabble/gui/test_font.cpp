#include <fstream>
#include<bits/stdc++.h>
std::ifstream test("assets/fonts/Aptos.ttf");
int main(){

    if (!test.is_open()) {
        std::cout << "Font file not found at assets/fonts/Aptos.ttf" << std::endl;
    }
    else{
        std::cout<<"Found"<<std::endl;
    }
}