## Wrapping POSIX regexps and Lua string patterns

C++11 does have a regular expression library, inherited from Boost,
but in my experience this can add serious compile-time overhead
to projects. `textutil::Rxp` wraps the standard POSIX regexp interface,
and `textutil::Rxl` wraps Lua string patterns. Although less powerful,
Lua string patterns are much faster when dealing with large amounts
of text.  For this project I've extracted the pattern matching from lstrlib.c
from Lua 5.2.

The API design is based on the [Lua string library](http://www.lua.org/pil/20.1.html);
 there are _matches_,
which may have multiple submatches (or 'captures'). `gmatch` is an 
iterator over all the matches in a string, and `gsub` is a global
substitution using either replacement strings, map-like types or
functions.

```C
    Rxp d("[a-z]+");
    if (! d) {
       cerr << "bad regex " << d.error() << endl;
       return 1;
    }
    bool ok = d.matches("hello dolly");
    if (! ok) {
        cerr << "did not match" << endl;
        return 1;
    }
```

Using Lua string patterns is similar, but since they aren't pre-compled,
we only get bad pattern errors after a match is attempted:

```C
    Rxl d("[a-z]+");
    bool ok = d.matches("hello dolly");    
    if (! ok) {
       if (! d) {
          cerr << "bad pattern " << d.error() << endl;
          return 1;
       }    
       cerr << "did not match" << endl;
       return 1;
    }
```

Apart from error-handling, there is the same interface for both of
these classes. Even if using classic regexps, you may choose to use Lua notation
('%' instead of '\\', '%a' instead of '[[:alpha:]]'.) Otherwise the 
semantics are not changed at all.

The following is equivalent to Lua's `string.find`, although we lack multiple
returns and any 'captures' are not returned:

```C
    Rxp words("%a+",Rx::lua); // regexp in Lua clothing...
    const char *s2 = "baad! bad! argh"; 
    int start=0,end;
    if (words.find(s2,start,end)) {
        cout << string(s2,end-start) << '\n';
    }    
    // --> baad bad argh
```

This design makes it
convenient for testing text against a number of matches and then
extracting substrings from that text. 

Using an explicit match object allows iteration:

```C
    Rx::match m(words,s2);
    while (m.matches()) {
        cout << m[0] << '\n';
        m.next();
    }
```

(If you had any submatches they would be `m[1]` etc.)

This is the equivalent of `string.gmatch`, which essentially wraps up
the above explicit loop as a STL-style iterator:

```C
    for (auto M: words.gmatch(text)) {
       cout << M[0] << '\n';
    }

```

## Text Substitution

Lua's `string.gsub` has three overloaded forms, depending on the
type of the replacement:

 - it is a string, perhaps containing group references: "[%0]"
 - it is a 'table'; in C++ terms, any map-like type
 - it is a function which is passed the 'captures'. (In the C++ case,
 we pass the match object and let the function extract the submatches.)
 
The first two are handled by the `gsub` method of both `Rxp` and `Rxl`.
The map-like case is generic, for any associative array that can map
strings to a type that `std::to_string` can convert. 

The second is the generic method `gsub_fun` (template overloading cannot
distinguish the cases) and it is likely to be much more useful now that
closures have come to C++:

```C
   Rxp dollar("%$(%a+)");
   string res = dollar.gsub_fun("$HOME and $PATH",[](const Rx::match& m) { 
       auto res = getenv(m[0].c_str()); // we want a 'safe' getenv!
       return res ? res : "";
    });
    // --> "/home/steve and /home/steve/bin:/usr/local/sbin:/....."
```

## Iterator Extraction

Any semi-decent string library has a `split` utility function, but
I resisted this temptation. It would have to return a concrete type
like `std::vector<std::string>`, which would become a dependency of
this library. There is an alternative pattern which is much more
flexible:

```C
    vector<int> numbers;
    Rxp digits("%d+",Rx::lua);
    string text = "10 and a 20 plus 30 any 40";
    digits.gmatch(text).append_to(numbers);
    cout << numbers << '\n';
    // --> 10 20 30 40

```

The target type can be anything that:

  - understands `push_back`
  - has defined `value_type`
  - where `value_type` can be stringified by `textutil::from_string`
  
The definition of `Rx::match::append_to` is straightforward:

```C
        template <class C>
        void append_to(C& c) {
           typedef typename C::value_type value_type;
           while(matches()) { 
              c.push_back(from_string<value_type>(group()));
              next();
           }            
        }

```

There is a generalization of this idea for _filling maps_. 
`fill_map` assumes that the first
submatch is the key, and the second submatch is the value. With a 
little thought, this can make simple configuration files a bit easier ;)

```C
    // fill a map with matches - uses M[1] and M[2]    
    Rxp word_pairs("(%a+)=([^;]+)",Rx::lua);
    string text = "dog=juno;owner=angela";
    map<string,string> config;
    word_pairs.gmatch(text).fill_map(config);
    assert(config["dog"] == "juno" && config["owner"] == "angela");

```

