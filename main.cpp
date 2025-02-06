#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

int dotProduct(vector<int> v1, vector<int> v2) {
    int result = 0;
    for (int i = 0; i < v1.size(); i++) {
        result += v1[i] * v2[i];
    }

    return result;
}

int main()
{
    vector<int> v1 = {2, 3, 1};
    vector<int> v2 = {4, 2, 5};

    cout << dotProduct(v1, v2) << endl;

    return 0;
}
