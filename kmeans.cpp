#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

// ユークリッド距離計算
double Calc_Edistance(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

int main(int argc, char* argv[]){
    // コマンドライン引数のチェック
    if(argc != 3){
        cerr << "引数の数が間違っています．" << endl;
        return 1;
    }

    // 乱数seed値を現在時刻で決定
    srand(time(NULL));

    string input_fname = argv[1];
    string output_fname = argv[2];

    int count=0;
    double data[200][2];// the number of data is 200
    int label[200];
    
    // ファイル入力
    ifstream ifs_csv(argv[1]);
    // ファイルを開くのに失敗したときの処理
    if(!ifs_csv){
        cerr << "ファイルの読み込みに失敗." << endl;
        return 1;
    }
    string line;
    while (getline(ifs_csv, line)) {

        istringstream stream(line);
        string field;
        vector<string> strvec;
        while (getline(stream, field, ',')) {
            strvec.push_back(field);
        }

        for (int i=0; i<strvec.size();i++){
            data[count][0] = stold(strvec[0]);
            data[count][1] = stold(strvec[1]);
        }
        count++;
    }


    /* k-means */

    int k;
    cout << "kの値を入力" << endl;
    cin >> k;
    vector<vector<double>> center_data(k, vector<double>(2)); // 重心ラベルとデータ
    
    /*
    クラスと重心データ番号の連想配列
    key:クラスk, value:元データの番号
    */
    map<int,int> center_mp; 

    /* k個の点をランダムに選択 */
    int klabel=0;
    while(klabel < k){
        int t = rand()%200;
        //重複したらスキップ
        auto itr = center_mp.find(klabel);
        if( itr != center_mp.end() && itr->second == t )
            continue;
        
        center_data[klabel][0] = data[t][0];
        center_data[klabel][1] = data[t][1];
        center_mp[klabel] = t;
        klabel++;
    }

    bool isEnd; // 操作を終了するかどうかのフラグ
    vector<bool> center_update(k); // 各クラスタの重心に更新があるかのフラグ配列
    int loop_count = 0; // ループ処理のカウント
    while(!isEnd){
        isEnd = true;
        for(int i=0; i<k; i++)
            center_update[i] = false;

        /* 各点を一番近い重心のクラスタに変更 */
        for(int i=0; i<200; i++){
            double min_dist = 10000;
            for(int j=0; j<k; j++){
                double temp = Calc_Edistance(data[i][0], data[i][1], center_data[j][0], center_data[j][1]);
                if(temp < min_dist){
                    label[i] = j;
                    min_dist = temp;
                }
            }
        }

        /* クラスタの重心を計算 */
        for(int i=0; i<k; i++){
            double cx = 0;
            double cy = 0;
            int k_num = 0;
            for(int j=0; j<200; j++){
                if(label[j] == i){
                    cx += data[j][0];
                    cy += data[j][1];
                    k_num++;
                }
            }
            // 重心を更新できる場合, 新たに変更
            if(center_data[i][0]!=cx/k_num || center_data[i][1]!=cy/k_num){
                //i番目のクラスタ重心更新フラグを更新
                center_update[i] = true;
                center_data[i][0] = cx / k_num;
                center_data[i][1] = cy / k_num;
            }
        }
        for(int i=0; i<k; i++){
            //1つでもフラグ更新があれば再度ループ
            if(center_update[i]){
                isEnd = false;
                break;
            }
        }
        loop_count++;
        // cout << "loop : " << loop_count << endl;
    }

    // ファイル出力
    ofstream ofs_csv(argv[2]);
    if(!ofs_csv){
        cerr << "ファイルの読み込みに失敗." << endl;
        return 1;
    }

    // 出力データをcsvファイルに書き込む
    for(int i=0; i<200; i++){
        ofs_csv << data[i][0] << ',' << data[i][1] << ',' << label[i] << ',' << center_data[label[i]][0] << ',' << center_data[label[i]][1];
        ofs_csv << endl;
    }
        
    return 0;
}