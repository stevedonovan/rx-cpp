#include <iostream>
#include "rx.h"
using namespace std;
using namespace textutil;

int main(int argc, char **argv) {
    if (argc > 2) {
        Rxp R(argv[1],Rx::lua);
        if (! R) { // bad regexp! bad!
            cerr << R.error() << endl;
            return 1;
        }
        const char *text = argv[2];
        Rx::match M(R,text);
        if (M.matches()) {
            for (int i = 0; i < R.n_matches(); i++) {
                int m1,m2;
                M.range(i,m1,m2);
                cout << i << " '" << M[i] << "' " << m1 << ' ' << m2 << endl;
            }
        } else {
            cout << "no match" << endl;
        }        
    } else {
        cerr << "rx-match <regexp> <string>" << endl;
        return 1;
    }
    return 0;
}