#include <iostream>
#include <map>
#include <vector>
#include "rx.h"
#include <stdlib.h>
using namespace std;
using namespace textutil;

template <class T>
ostream& operator<< (ostream& os, const vector<T>& v) {
    for (auto& val:  v) os << val << ' ';
    return os;
}

template <class T>
ostream& operator<< (ostream& os, const map<string,T>& v) {
    for (auto& p:  v) os << p.first << ':' << p.second << ' ';
    return os;
}

typedef map<string,string> StringMap;

int main()
{
    
    // custom string literal!
    Rxl d("[a-z]+");
    bool ok = d.matches("hello dolly");
    if (! ok) {
        cerr << "did not match" << endl;
        return 1;
    }

    // a Useful Quote
    const char *text = 
    "Some people, when confronted with a problem, think "
    "\"I know, I'll use regular expressions.\" "
    "Now they have two problems.";
    
    Rxl words("%a+");
    const char *s2 = "baad! bad! argh"; 

    // equivalent to Lua's string.find
    int start=0,end;
    if (words.find(s2,start,end)) {
        cout << string(s2,end-start) << endl;
    }    
    // --> baad
    
    // looping over a word match
    // explictly using a match
    Rx::match m(words,s2);
    while (m.matches()) {
        cout << m[0] << endl;
        m.next();
    }
    
    // sexier version of the above, applied to the Useful Quote
    // all words longer than 6 chars
    for (auto M: words.gmatch(text)) {
        if (M[0].size() > 6)
            cout << M[0] << endl;
    }
    
    // fill a vector with matches!
    //~ vector<string> strings;
    //~ words.gmatch(text).append_to(strings);
    //~ cout << strings << endl;
    // --> Some people when confronted with a problem think ...
    
    vector<int> numbers;
    Rxl digits("%d+");
    digits.gmatch("10 and a 20 plus 30 any 40").append_to(numbers);
    cout << numbers << endl;
    // --> 10 20 30 40
    
    // fill a map with matches - uses M[1] and M[2]    
    Rxl word_pairs("(%a+)=([^;]+)");
    StringMap config;
    word_pairs.gmatch("dog=juno;owner=angela").fill_map(config);
    cout << config << endl;
    // --> dog:juno owner:angela

    // the 3 kinds of global substitution:
    // (1) the replacement is a string with group references
//    /*
    Rxl R2("<(%a+)>");
    auto S = "hah <hello> you, hello <dolly> yes!";    
    cout << R2.gsub(S,"[%1]") << endl;    
    // -> hah [hello] you, hello [dolly] yes!
 //   */
    // (2) the replacement is an associative array
     Rxl R3("%$(%a+)");
     string res;
    
    StringMap lookup = {
       {"bonzo","BONZO"},
       {"dog","DOG"}
    };   
    res = R3.gsub("$bonzo is here, $dog! Look sharp!",lookup);
    cout << res << endl;
    // --> BONZO is here, DOG! Look sharp!
    
    //Rxl("%$(%a+)")
    res = R3.gsub("$bonzo is here, $dog! Look sharp!",lookup);
    cout << res << endl;
    
    
    ///*
    // result of lookup may be anything that std::to_string can convert
    map<string,int> ints = {{"frodo",50},{"bilbo",120}};
    res = R3.gsub("$frodo and $bilbo",ints);
    cout << res << endl;
    // --> 50 and 120
    //*/
    
    ///*
    // (3) the replacement is a function which receives a reference to
    // the match object
    // this will be prettier in C++14 (since GCC 4.9) using a generic 'auto' lambda.
    res = R3.gsub_fun("$HOME and $PATH",[](const Rx::match& m) { 
        auto res = getenv(m.group().c_str()); // we want a 'safe' getenv!
        return res ? res : "";
    });
    cout << res << endl;
    // --> /home/steve and /home/steve/bin:/usr/local/sbin:/.....
    //*/
    return 0;
}
