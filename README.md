# longest-animeloop-cli
Different from animeloop-cli, this repo aims at founding the longest loop in given duration instead of finding all possible loop.

## How to use
1. clone the repo
2. if you'd like use make, then ```cd loop && make```
3. if you prefer Xcode, then just open ```loop.xcodeproj``` and compile
4. ```loop [video file] [clips directory]```

### FAQ
Q: It's easy to pick the longest clips from animeloop-cli, why create a new repo?
=======

A: The search methods are different between these two repos. 

animeloop-cli tries to give you all the possible loops with duration from minimum to maximum. 

However, as the name suggests, longest-animeloop-cli tries to only give you the longest animeloop with duration from minimum to maximum. 

Q: Well, but still confused about the difference, isn't it just a minor problem? 
=======

A: First, since animeloop-cli gives you all the possible loops, it searches from the begin to the end while longest-animeloop-cli searches from back to the start. 

Second, both projects need further refactoring and refinements. It's hard to keep pace because of the different search goal.
(Maybe these two repo will combine in the future)