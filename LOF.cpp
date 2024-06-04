#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
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

    string input_fname = argv[1];
    string output_fname = argv[2];

    int count=0;
    double data[200][2]; // the number of data is 200
    double LOF_k[200]; // LOF値を格納
    
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


    /* LOF算出 */
    int k;
    cout << "kの値を入力" << endl;
    cin >> k;
    
    /* k-dist配列は,あるデータにおけるk-distanceとなるようなデータ(k番目に近い点)のラベルとユークリッド距離を格納する */
    vector<pair<int, double>> k_dist(200);
    vector<vector<int>> Nk(200);
    vector<double> lrd_k(200);

    /* 各データのk-distanceを求める */
    // 点iから各点についての全距離を格納
    vector<vector<pair<double, int>>> all_dist(200);
    for(int i=0; i<200; i++){
        for(int j=0; j<200; j++){
            if(i != j){
                double d = Calc_Edistance(data[i][0], data[i][1], data[j][0], data[j][1]);
                all_dist[i].push_back({d, j});
            }
        }
        // 距離の昇順にソート
        sort(all_dist[i].begin(), all_dist[i].end());
        k_dist[i] = make_pair(all_dist[i][k-1].second, all_dist[i][k-1].first);
    }

    /* 各データのN_k集合を求める */
    for(int i=0; i<200; i++){
        for(int j=0; j<all_dist[i].size(); j++){
            // d(A,B) <= k-distance(A)ならNk(A)にBのラベルを追加
            if(all_dist[i][j].first <= k_dist[i].second)
                Nk[i].push_back(all_dist[i][j].second);
        }
    }

    /* 各データのlrd_kを求める */
    for(int i=0; i<200; i++){
        //reach-dist_k(A,B)の総和をとる
        double sum = 0;
        for(int j=0; j<Nk[i].size(); j++){
            //Nk(A)に含まれるデータを利用
            double d = Calc_Edistance(data[i][0], data[i][1], data[Nk[i][j]][0], data[Nk[i][j]][1]);
            // reach_dist_k(A,B) := max{d(A,B), k-distance(B)}
            double rch_dist_k = max(d, k_dist[Nk[i][j]].second);
            sum += rch_dist_k;
        }
        lrd_k[i] = Nk[i].size()/sum;
    }

    /* lrd_kをもとに,各データのLOF値を求める */
    for(int i=0; i<200; i++){
        double sum = 0;
        for(int j=0; j<Nk[i].size(); j++)
            sum += lrd_k[Nk[i][j]]/Nk[i].size();
        
        LOF_k[i] = sum/lrd_k[i];
        //LOF_k[i] = (sum/Nk[i].size())/lrd_k[i];
    }

    // ファイル出力
    ofstream ofs_csv(argv[2]);
    if(!ofs_csv){
        cerr << "ファイルの読み込みに失敗." << endl;
        return 1;
    }

    for(int i=0; i<200; i++){
        // csvファイルに書き込む
        ofs_csv << data[i][0] << ',' << data[i][1] << ',' << LOF_k[i];
        ofs_csv << endl;
    }
        
    return 0;
}