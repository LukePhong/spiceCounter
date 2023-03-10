#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
using namespace std;

string outputDir = "./output/";

//需要手动输入文件名 第一个参数
int main(int argc, char**argv) {
    //收集的信息
    string netName;
    vector<string> portName;
    set<string> streamName;
    vector<string> mosName;
    vector<string> combinationName;

    ifstream inFile;
    inFile.open(argv[1], ios::in);

    ofstream outFile( outputDir + string(argv[1]) + ".csv", ios::out);
    outFile << "netNo.,"<< "netName,"<<"Port Count,"<<"Device Count,"<<"MOS Count,"<<"Combination Count,"<<"Stream Count,"<<"MOS Percentage"<<endl;

    string tmp;
    char a,b,c;
    int netNo = 0;
    while(inFile>>tmp)  //读入文件
    {
        //每一个子图
        if (tmp == ".subckt" || tmp == ".SUBCKT")
        {
//            bool flag=0;  //总输出/输入线flag，0为输出，1为输入
            inFile>>netName;
            int cur11;
            bool portDone = false;
            while (!portDone)    //读入总输入输出线 不区分总输入、总输出线
            {
                cur11 = inFile.tellg();
                inFile.get(a);
                inFile.get(b);
                //结尾也可能是两个连着的空格
                inFile.get(c);
//                cout<<a<<"..."<<b<<"..."<<c<<endl;
                inFile.seekg(cur11, ios_base::beg);    //ios_base::beg：相对于文件开始处的偏移量，回到3个get之前的位置
                //换了行且没有+，这是循环的终止条件
                if (a == '\n' && b != '+' || a==' ' && b==' ' && c=='\n')
                {
//                   portDone = true;
                    break;
                }
                inFile >> tmp;
                if (tmp[0] == '+')  //+代表换行
                    tmp.erase(0, 1);  //把+号去掉
                if(tmp[0] == '/'){
                    continue;   //跳过/号
                }
                if (tmp.find('=') != tmp.npos)  //有“=”
                {
                    getline(inFile, tmp);  //如果读入的单词有=那说明端口定义结束于这一行，且它已经结束，所以要抛弃到这一行结束
                    break;
                }
//                cur11 = inFile.tellg();
//                inFile.get(a);
//                inFile.get(b);
//                //换了行且没有+，这是循环的终止条件
//                if (a == '\n' && b != '+')
//                {
////                   portDone = true;
//                    break;
//                }
//                inFile.seekg(cur11, ios_base::beg);    //ios_base::beg：相对于文件开始处的偏移量
                //tmp会被空过去
                if (tmp != "/")
                {
                    portName.push_back(tmp);
                    streamName.insert(tmp);
                }
//                if(tmp=="/") flag=!flag;
            }
            inFile >> tmp;
            string tmp11 = tmp;
            cur11 = inFile.tellg();
            while (tmp != ".ends" && tmp != ".ENDS")  //读入每个元件和其对应连线
            {
                if (tmp[0] == 'M')  //MOS管
                {
                    mosName.push_back(tmp);
                    for (int i = 0; i < 4; i++)  //读入四条线
                    {
                        inFile >> tmp;
                        streamName.insert(tmp);
                    }
                }
                else if (tmp[0] == 'X')  //组合型
                {
                    combinationName.push_back(tmp);

                    inFile >> tmp;
                    int cur = inFile.tellg();
                    string tmp1,tmp2;

                    inFile.get(a);
                    inFile >> tmp1;//pre-read subcircuit name
                    inFile >> tmp2;//pre-read subcircuit feature
                    //不是换行且拿到的词里面没有=
                    if ((a != '\n') && (tmp2.find('=') == tmp2.npos))  // deal with conditions that there exist no-interface device
                    {
                        do
                        {
                            if (tmp[0] == '+')//deal with the condition that '+' prolong the rank
                                tmp.erase(0, 1);
                            streamName.insert(tmp);

                            tmp = tmp1;
                            tmp1 = tmp2;
                            cur = inFile.tellg();
                            inFile.get(a);
                            inFile.get(b);
                            if (a == '\n' && b != '+')
                            {
                                streamName.insert(tmp);
                                break;
                            }
                            inFile.seekg(cur, ios_base::beg);
                            inFile >> tmp2;
                            if (tmp2.find('=') != tmp2.npos)
                            {
                                streamName.insert(tmp);
                                break;
                            }
                        } while (true);
                    }
                    inFile.seekg(cur, ios_base::beg);// give back the pre-read

                }
                //忽略之后这一行的所有内容
                getline(inFile, tmp);
                inFile >> tmp;
            }


            size_t deviceNum = mosName.size() + combinationName.size(),
                    mosNum = mosName.size(),
                    combinationNum = combinationName.size(),
                    streamNum = streamName.size();
            double mosPercent = 1.0 * mosName.size() / (mosName.size() + combinationName.size());
            //输出收集到的信息
/*            cout<<"name of Net: "<<netName<<endl;
            cout<<"number of ports: "<<portName.size()<<endl;
            cout<<"print names of ports: "<<endl;
//            for (const auto& s:portName) {
//                cout<<s<<"\t";
//            }
            cout<<endl;

            cout<<"number of devices: "<<deviceNum<<endl;
            cout<<"mos number: "<<mosNum<<endl;
//            for (const auto& s:mosName) {
//                cout<<s<<"\t";
//            }
            cout<<endl;
            cout<<"mos percentage: "<<mosPercent<<endl;

            cout<<"combination number: "<<combinationNum<<endl;

//            for (const auto& s:combinationName) {
//                cout<<s<<"\t";
//            }
            cout<<endl;

            cout<<"stream number: "<<streamNum<<endl;
//            for (const auto& s:streamName) {
//                cout<<s<<"\t";
//            }
            cout<<endl;
            cout<<"===================================================="<<endl<<endl;
*/

            //文件写入
            outFile<<netNo<<",";
            outFile<<netName<<",";
            outFile<<portName.size()<<",";
            outFile<<deviceNum<<",";
            outFile<<mosNum<<",";
            outFile<<combinationNum<<",";
            outFile<<streamNum<<",";
            outFile<<mosPercent<<endl;

            netNo++;

            //清空上面收集的信息的容器
            vector<string>().swap(portName);
            vector<string>().swap(combinationName);
            vector<string>().swap(mosName);
            set<string>().swap(streamName);
        }
    }

    inFile.close();

    return 0;
}
