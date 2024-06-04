#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>
using namespace std;


// ユークリッド距離計算
double Calc_Edistance(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

// ある点pの半径eps以内に存在する隣接点を求める
set<int> RangeQuery(double data[][2], int p, double eps){
    set<int> Neighbors;
    // 全てのデータについて操作
    for(int i=0; i<200; i++){
        // データqとの距離を算出
        double d = Calc_Edistance(data[p][0], data[p][1], data[i][0], data[i][1]);
        // 半径eps以内に存在すれば
        if(d <= eps){
            Neighbors.insert(i);
        }
    }
    return Neighbors;
}

// DBSCAN
void DBSCAN(double data[][2], double eps, int minPts, int* visited, int* label){
    int c = 0;  // クラスタカウンタ
    for(int i=0; i<200; i++){
        if(visited[i] != 0) continue;
        visited[i] = 1;
        set<int> Neighbors = RangeQuery(data, i, eps); // 隣接点を求める

        // 隣接点がminPts個以上存在しなければ外れ値として判定(暫定的に)
        if(Neighbors.size() < minPts){
            label[i] = 0;
            //continue;
        }
        else{
            // そうでなければ新たにクラスタを形成
            c++;
            label[i] = c;

            // 自身を除いた隣接点のみの集合を生成
            set<int> SeedSet = Neighbors;
            SeedSet.erase(i);

            // 走査の都合上,vectorに変換
            vector<int> SeedVec(SeedSet.begin(), SeedSet.end());
            // 隣接点についての処理
            for(int j=0; j<SeedVec.size(); j++){
                // 既に外れ値として判定されていたなら同じクラスタの端点に分類
                if(label[SeedVec[j]] == 0) label[SeedVec[j]] = c;
                if(visited[SeedVec[j]] != 0) continue;

                // まだ訪れていない隣接点に対する処理
                visited[SeedVec[j]] = 1;
                label[SeedVec[j]] = c;

                // 隣接点SeedVec[j]の隣接点を見つける
                set<int> Neighbors_seed = RangeQuery(data, SeedVec[j], eps);
                // 隣接点SeedVec[j]がコア点かどうかを判定
                if(Neighbors_seed.size() >= minPts){
                    //コア点の場合，SeedVec[j]の隣接点を走査対象に追加
                    for(auto itr2=Neighbors_seed.begin(); itr2!=Neighbors_seed.end(); itr2++)
                        SeedVec.push_back(*itr2);
                }
            }
        }
    }
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
    int label[200];
    int visited[200];   // データを訪れたかどうか
    for(int i=0; i<200; i++)
        visited[i] = 0;
    
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

    /* DBSCAN実行 */
    double eps;
    int minPts;
    cout << "半径epsの値を入力" << endl;
    cin >> eps;
    cout << "minPtsの値を入力" << endl;
    cin >> minPts;
    DBSCAN(data, eps, minPts, visited, label);

    // ファイル出力
    ofstream ofs_csv(argv[2]);
    if(!ofs_csv){
        cerr << "ファイルの読み込みに失敗." << endl;
        return 1;
    }

    // 出力データをcsvファイルに書き込む
    for(int i=0; i<200; i++){
        ofs_csv << data[i][0] << ',' << data[i][1] << ',' << label[i];
        ofs_csv << endl;
    }
        
    return 0;
}