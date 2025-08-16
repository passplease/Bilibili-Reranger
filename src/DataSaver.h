#include <fstream>

using namespace std;

void createAndSave(const string& content,const string& filePath){
    ofstream file;
    file.open(filePath,ofstream::app);
    if(file.is_open()){
        file << content;
    }else cout << "将数据写入文件（" << filePath << "）失败" << endl;
    file.close();
}
void clean(const string& filePath){
    ofstream file;
    file.open(filePath);
    if(file.is_open()){
        file << "";
    }else cout << "清除文件（" << filePath << "）失败" << endl;
    file.close();
}
